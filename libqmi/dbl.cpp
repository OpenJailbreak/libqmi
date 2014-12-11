//
//  dbl.cpp
//  dbltool
//
//  Created by Joshua Hill on 2/5/13.
//
//

#include <sys/socket.h>
#include <iostream>

#include "dbl.h"
#include "usb.h"
#include "sock.h"
#include "util.h"

int dbl_boot(USBInterface iface, const char* bbticket, const char* osbl, const char* amss) {
    int done = 0;
    unsigned char input[0x2000];
    dbl_header_t* request = NULL;
    dbl_execute_resp_t* response = NULL;
    int mode = 0;
    while(!done) {
        //qmilog("Recv:\n");
        memset(input, '\0', 0x2000);
        UInt32 insize = sizeof(input);
        USBReadBulk(iface, 1, input, &insize);
        if(insize > 0) {
            if(insize == -1) done = 1;
            //hexdump(input, insize);
            request = (dbl_header_t*) input;
            switch(request->code) {
                case DBL_PARAM_REQ:
                    //qmilog("Got DBL Parameter request\n");
                    mode = ((dbl_param_req_t*) input)->file;
                    dbl_send_params(iface, (dbl_param_req_t*) input);
                    break;
                    
                case DBL_MEMORY_REQ:
                    //qmilog("Got DBL Memory Request\n");
                    dbl_send_memory(iface, (dbl_memory_req_t*) input, osbl, amss);
                    break;
                    
                case DBL_MEMORY_RESP:
                    //qmilog("Got DBL Memory Response\n");
                    dbl_send_execute(iface);
                    break;
                    
                case DBL_EXECUTE_REQ:
                    //qmilog("Got DBL Execute Request\n");
                    break;
                    
                case DBL_EXECUTE_RESP:
                    //qmilog("Got DBL Execute Response\n");
                    response = (dbl_execute_resp_t*) input;
                    if(response->file == 0x1) done = 1;
                    break;
                    
                case DBL_BBTICKET_REQ:
                    //qmilog("Got BBTicket Request\n");
                    dbl_send_bbticket_params(iface);
                    break;
                    
                case DBL_BBTICKET_DATA_REQ:
                    //qmilog("Got BBTicket Data Request\n");
                    dbl_send_bbticket_memory(iface, (dbl_bbticket_data_req_t*) request, bbticket);
                    break;
                    
                case DBL_BBTICKET_EXECUTE_REQ:
                    //qmilog("Got BBTicket Execute Request\n");
                    dbl_send_bbticket_execute(iface);
                    break;
                    
                case DBL_BBTICKET_EXECUTE_RESP:
                    //qmilog("Got BBTicket Execute Response\n");
                    break;
                    
                case DBL_BBTICKET_RESULT:
                    //qmilog("Got BBTicket Result\n");
                    dbl_send_execute(iface);
                    break;
                    
                default:
                    qmilog("Got Unknown Request\n");
                    break;
            }
        }
    }
    
    return 0;
}

int dbl_send_params(USBInterface interface, dbl_param_req_t* request) {
    dbl_param_resp_t response;
    //02 00 00 00 // Command
    response.header.code = DBL_PARAM_RESP;
	//30 00 00 00 // Size
    response.header.size = sizeof(dbl_param_resp_t);
	//01 00 00 00 // 0x01??
    response.unknown1 = 0x1;
	//01 00 00 00 // 0x01??
    response.unknown2 = 0x1;
	//00 00 00 00 // 0x00??
    response.unknown3 = 0x0;
	//00 01 00 00 // 0x100??
    response.file = request->file;
	//00 00 00 00 // 0x0
    response.unknown5 = 0x0;
	//00 00 00 00 // 0x0
    response.unknown6 = 0x0;
	//00 00 00 00 // 0x0
    response.unknown7 = 0x0;
	//00 00 00 00 // 0x0
    response.unknown8 = 0x0;
	//00 00 00 00 // 0x0
    response.unknown9 = 0x0;
	//00 00 00 00 // 0x0
    response.unknown10 = 0x0;
    USBWriteBulk(interface, 2, &response, sizeof(response));
    //hexdump((unsigned char*) &response, sizeof(response));
    return 0;
}

int dbl_send_memory(USBInterface interface, dbl_memory_req_t* request, const char* osbl, const char* amss) {
    FILE* fd = NULL;
    unsigned char* buffer = NULL;
    if(request->file == DBL_MEMORY_REQ_OSBL) {
        // Send OSBL data
        fd = fopen(osbl, "rb");
        if(fd) {
            buffer = (unsigned char*) malloc(request->length);
            if(buffer) {
                fseek(fd, request->offset, SEEK_SET);
                int x = fread(buffer, 1, request->length, fd);
                if(x == request->length) {
                    USBWriteBulk(interface, 2, buffer, x);
                    //hexdump((unsigned char*) buffer, x);
                }
            }
            fclose(fd);
        }
    } else if(request->file == DBL_MEMORY_REQ_AMSS) {
        // Send AMSS data
        fd = fopen(amss, "rb");
        if(fd) {
            buffer = (unsigned char*) malloc(request->length);
            if(buffer) {
                fseek(fd, request->offset, SEEK_SET);
                int x = fread(buffer, 1, request->length, fd);
                if(x == request->length) {
                    USBWriteBulk(interface, 2, buffer, x);
                    //hexdump((unsigned char*) buffer, x);
                }
            }
            fclose(fd);
        }
    } else {
        qmilog("Request from unknown file\n");
    }
    
    if(buffer != NULL) free(buffer);
    return 0;
}

int dbl_send_execute(USBInterface interface) {
    dbl_execute_req_t response;
    response.header.code = DBL_EXECUTE_REQ;
    response.header.size = sizeof(dbl_execute_req_t);
    USBWriteBulk(interface, 2, &response, sizeof(response));
    //hexdump((unsigned char*) &response, sizeof(response));
    return 0;
}

int dbl_send_bbticket_params(USBInterface interface) {
    dbl_bbticket_resp_t response;
    response.header.code = DBL_BBTICKET_RESP;
    response.header.size = sizeof(dbl_bbticket_resp_t);
    response.unknown1 = 0x0;
    USBWriteBulk(interface, 2, &response, sizeof(response));
    //hexdump((unsigned char*) &response, sizeof(response));
    return 0;
}

int dbl_send_bbticket_memory(USBInterface interface, dbl_bbticket_data_req_t* request, const char* bbticket) {
    if(request->bbchipid != 0 && request->bbsernum != 0) {
        qmilog("Found a NONCE, please connect\n");
        
        int x = 0;
        int s = sock_listen(10000);
        if(s > 0) {
            x = sock_accept_connection(s);
            if(x > 0) {
                //hexdump(request->nonce, sizeof(request->nonce));
                sock_send(x, request->nonce, sizeof(request->nonce));
                qmilog("Nonce sent\n");
                
                unsigned int size = 0;
                unsigned char data[0x2000];
                size = sock_recv(x, data, sizeof(data));
                qmilog("BBTicket received\n");
                qmilog("Got data of size 0x%x\n", size);
                
                // And then upload it to the baseband
                dbl_bbticket_data_resp_t* response = NULL;
                response = (dbl_bbticket_data_resp_t*) malloc(sizeof(dbl_bbticket_data_resp_t) + size);
                response->header.code = DBL_BBTICKET_DATA_RESP;
                response->header.size = sizeof(dbl_bbticket_data_resp_t) + size;
                memcpy(response->data, data, size);
                USBWriteBulk(interface, 2, response, response->header.size);
                //hexdump((unsigned char*) response, response->header.size);
            }
        }
        
    } else {
        unsigned int size = 0;
        unsigned char data[0x2000];
        dbl_bbticket_data_resp_t* response = NULL;
        FILE* fd = fopen(bbticket, "r");
        if(fd) {
            size = fread(data, 1, 0x2000, fd);
            if(size > 0) {
                response = (dbl_bbticket_data_resp_t*) malloc(sizeof(dbl_bbticket_data_resp_t) + size);
                response->header.code = DBL_BBTICKET_DATA_RESP;
                response->header.size = sizeof(dbl_bbticket_data_resp_t) + size;
                memcpy(response->data, data, size);
                USBWriteBulk(interface, 2, response, response->header.size);
                //hexdump((unsigned char*) response, response->header.size);
            }
            fclose(fd);
        }
    }
    return 0;
}

int dbl_send_bbticket_execute(USBInterface interface) {
    dbl_bbticket_execute_resp_t response;
    response.header.code = DBL_BBTICKET_EXECUTE_RESP;
    response.header.size = sizeof(dbl_bbticket_execute_resp_t);
    response.result = 0x1;
    USBWriteBulk(interface, 2, &response, sizeof(response));
    //hexdump((unsigned char*) &response, sizeof(response));
    return 0;
}