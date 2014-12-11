//
//  main.cpp
//  qcommtool
//
//  Created by Joshua Hill on 2/16/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <CoreFoundation/CoreFoundation.h>

#include "usb.h"
#include "util.h"

int QCommTool (int argc, const char * argv[]) {
    UInt8 config = 0;
    USBDevice device;
    USBInterface iface;
    
    printf("Starting QCommTool\n");
    
    //Vendor ID: 0x5c6
    //Product ID: 0x9001
    device = USBOpenDevice(0x5c6, 0x9001);
    if(device) {
        printf("Device Opened\n");
        config = USBSetConfiguration(device, 1);
        if(config == 1) {
            printf("Configuration %hhx set\n", config);
            //for(i = atoi(argv[1]); i < 0xC; i++) {
                iface = USBOpenInterface(device, atoi(argv[1]), 0);
                if(iface) {
                    printf("Opened device with interface %d\n", atoi(argv[1]));
                    int y = 0;
                    UInt8 buffer[0x800];
                    UInt32 size = 0x800;
                    
                    

                    size = 0x800;
                    y = USBReadBulk(iface, 1, buffer, &size);
                    if(y == 0 && size > 0) {
                        hexdump(buffer, size);
                    }
                    
                    size = 0x800;
                    y = USBReadControl(iface, 0, buffer, &size);
                    if(size == 0) {
                        printf("First read returned 0 bytes\n");
                    }
                    
                    size = 0x800;
                    y = USBWriteBulk(iface, 3, (unsigned char*) "\x01\x0f\x00\x00\x00\x00\x00\x01\x22\x00\x04\x00\x01\x01\x00\x02", sizeof("\x01\x0f\x00\x00\x00\x00\x00\x01\x22\x00\x04\x00\x01\x01\x00\x02"));
                    if(y == 0) {
                        if(size == 0) {
                            printf("Wrote 0 bytes\n");
                        } else {
                            hexdump(buffer, size);
                        }
                    }
                    
                    
                    y = USBWriteControl(iface, 0, (unsigned char*) "\x01\x0f\x00\x00\x00\x00\x00\x01\x22\x00\x04\x00\x01\x01\x00\x02", sizeof("\x01\x0f\x00\x00\x00\x00\x00\x01\x22\x00\x04\x00\x01\x01\x00\x02"));
                    
                    
                    
                    /*
                     y = WriteControl(iface, 0, (unsigned char*) "\x01\x0b\x00\x00\x00\x00\x00\x1c\x27\x00\x00\x00", sizeof("\x01\x0b\x00\x00\x00\x00\x00\x1c\x27\x00\x00\x00"));
                    
                    
                    size = 0x800;
                    y = ReadControl(iface, 0, buffer, &size);
                    if(y == 0 && size > 0) {
                        hexdump(buffer, size);
                    }
                    
                    y = WriteControl(iface, 0, (unsigned char*) "\x01\x14\x00\x00\x00\x00\x00\x1d\x26\x00\x09\x00\x01\x01\x00\x00\x10\x02\x00\x02\x00", sizeof("\x01\x14\x00\x00\x00\x00\x00\x1d\x26\x00\x09\x00\x01\x01\x00\x00\x10\x02\x00\x02\x00"));

                    size = 0x800;
                    y = ReadControl(iface, 0, buffer, &size);
                    if(y == 0 && size > 0) {
                        hexdump(buffer, size);
                    }
                    //
                     */
                    //printf("Trying next interface\n");
                    USBCloseInterface(iface);
                }
            //}
        }
        
        USBCloseDevice(device);
    }
    
    return 0;
}

