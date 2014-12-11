//
//  dload.cpp
//  dloadtool
//
//  Created by Joshua Hill on 1/31/13.
//
//


#include <IOKit/IOKitLib.h>

#include "etl.h"
#include "usb.h"
#include "util.h"
#include "dload.h"

int dload_boot(USBInterface iface) {
    UInt32 addr = 0x20012000;
    dload_get_params(iface);
    dload_get_sw_version(iface);
    dload_upload_firmware(iface, addr, "/usr/local/standalone/firmware/Baseband/Trek/dbl.mbn");
    dload_send_execute(iface, addr);
    return 0;
}

int dload_get_params(USBInterface interface) {
    UInt8 output[0x100];
    UInt8 request = 0x0;
    dload_params* response = NULL;
    
    request = DLOAD_PARAM_REQ;
    memset(output, '\0', sizeof(output));
    dload_write(interface, &request, sizeof(request));
    dload_read(interface, output, sizeof(output));
    if(output[0] == DLOAD_PARAM_RESP) {
        response = (dload_params*) output;
        qmilog("Protocol Version: 0x%hhx\n", response->version);
        qmilog("Min Protocol Version: 0x%hhx\n", response->min_version);
        qmilog("Max Write Size: 0x%hx\n", flip_endian16(response->max_write));
        qmilog("Model: 0x%hhx\n", response->model);
        qmilog("Device Size: 0x%hhx\n", response->device_size);
        qmilog("Device Type: 0x%hhx\n", response->device_type);
        
    } else {
        qmilog("Error receiving software parameters!!\n");
        return -1;
    }
    return 0;
}

int dload_get_sw_version(USBInterface interface) {
    UInt8 output[0x100];
    UInt8 request = 0x0;
    dload_sw_version* response = NULL;
    
    request = DLOAD_SW_VER_REQ;
    memset(output, '\0', sizeof(output));
    dload_write(interface, &request, sizeof(request));
    dload_read(interface, output, sizeof(output));
    if(output[0] == DLOAD_SW_VERS_RESP) {
        response = (dload_sw_version*) output;
        qmilog("Software Version: %s\n", response->version);
    } else {
        qmilog("Error receiving software version!!\n");
        return -1;
    }
    return 0;
}

//Frame      Code      Address                Size
//           0x0f       0x20012000            0x100
//7e         0x05       0x20012000
unsigned char done[] = "\x7e\x05\x20\x01\x20\x00\x9f\x1f\x7e";
int dload_upload_firmware(USBInterface interface, UInt32 address, const char* path) {
    FILE* fd = NULL;
    UInt8 input[0x100];
    UInt8 output[0x100];
    UInt32 addr = 0x20012000;
    
    dload_write_addr* packet = (dload_write_addr*) malloc(sizeof(dload_write_addr) + sizeof(output));
    fd = fopen(path, "rb");
    if(fd != NULL) {
        //qmilog("Opened firmware at %s\n", path);
        while(!feof(fd)) {
            memset(input, '\0', sizeof(input));
            memset(output, '\0', sizeof(output));
            size_t len = fread(output, 1, sizeof(output), fd);
            //qmilog("Read buffer of size %zd\n", len);
            if(len != 0) {
                memset(packet, '\0', sizeof(dload_write_addr) + sizeof(output));
                memcpy(packet->buffer, output, sizeof(output));
                packet->code = DLOAD_WRITE_ADDR;
                packet->size = flip_endian16(len);
                packet->address = flip_endian32(addr);
                
                dload_write(interface, (UInt8*) packet, sizeof(dload_write_addr) + len);
                dload_read(interface, input, sizeof(input));
                if(input[0] != 0x2) {
                    qmilog("Error 0x%hhx!!!\n", input[0]);
                    free(packet);
                    return -1;
                }
                addr += len;
                
            }
        }
        
        fclose(fd);
    }
    free(packet);
    
    return 0;
}

int dload_send_execute(USBInterface interface, UInt32 address) {
    UInt8 input[0x100];
    memset(input, '\0', sizeof(input));
    
    dload_execute* request = (dload_execute*) malloc(sizeof(dload_execute));
    request->code = DLOAD_EXECUTE;
    request->address = flip_endian32(address);
    
    dload_write(interface, (UInt8*) request, sizeof(dload_execute));
    dload_read(interface, input, sizeof(input));
    
    free(request);
    return 0;
}

int dload_read(USBInterface interface, UInt8* buffer, UInt32 size) {
    UInt32 insize = 0;
    UInt32 outsize = 0;
    UInt8* inbuf = NULL;
    UInt8* outbuf = NULL;
    
    insize = size;
    inbuf = (UInt8*) malloc(size);
    
    //qmilog("Recv:\n");
    USBReadBulk(interface, 1, inbuf, &insize);
    if(insize > 0) {
        etl_response((uint8_t*)inbuf, (uint32_t)insize, (uint8_t**)&outbuf, (uint32_t*)&outsize);
        if(outsize <= size) {
            //hexdump(outbuf, outsize);
            memcpy(buffer, outbuf, outsize);
        }
    }
    
    free(inbuf);
    free(outbuf);
    return outsize;
}

int dload_write(USBInterface interface, UInt8* buffer, UInt32 size) {
    UInt32 outsize = 0;
    UInt8* outbuf = NULL;
    
    //qmilog("Send:\n");
    etl_request((uint8_t*)buffer, (uint32_t)size, (uint8_t**)&outbuf, (uint32_t*)&outsize);
    if(outsize > 0) {
        USBWriteBulk(interface, 2, outbuf, outsize);
        //hexdump(buffer, size);
    }
    
    free(outbuf);
    return outsize;
}
