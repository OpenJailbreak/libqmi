//
//  QMIUSBController.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//
//

#include "bb.h"
#include "dbl.h"
#include "util.h"
#include "dload.h"

#include "QMIPacket.h"
#include "QMIControlPacket.h"
#include "QMIUSBController.h"

#include <iostream>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <CoreFoundation/CoreFoundation.h>

#define VENDOR_QUALCOMM  0x05c6
#define PRODUCT_AMSS     0x9001
#define PRODUCT_DLOAD    0x9008
#define PRODUCT_DBL      0x900e

namespace QMI {

    USBController::USBController(): mDevice(0), mInterface(0), mSequence(0) {
        
    }

    USBController::~USBController() {
        if(mDevice || mInterface) this->close();
    }
    
    int USBController::init() {
            // Find which mode the device is running in
        bool wait = false;
            UInt8 config = 0;
            USBDevice device;
            USBInterface iface;
            
            qmilog("QMI::USBController::init()\n");
            
            //bb_reset();
            bb_enter_download_mode();
            
            device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DLOAD);
            if(device) {
                // Found device in DLOAD mode
                qmilog("Found device in DLOAD mode\n");
                // Set to configuration 1
                qmilog("Setting configuration to 1\n");
                config = USBSetConfiguration(device, 1);
                if(config == 1) {
                    // Open up interface 0
                    qmilog("Openning interface 0 to DLOAD\n");
                    iface = USBOpenInterface(device, 0, 0);
                    if(iface) {
                        dload_boot(iface);
                        USBCloseInterface(iface);
                        iface = NULL;
                    }
                }
                USBCloseDevice(device);
                device = NULL;
                wait = true;
            } else {
                qmilog("Device not in DLOAD Mode");
            }
        
        if(wait == true) {
            device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DBL);
            while(!device) {
                device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DBL);
            }
            if(device) {
                // Found device in DBL mode
                qmilog("Found device in DBL mode\n");
                qmilog("Setting configuration to 1\n");
                config = USBSetConfiguration(device, 1);
                if(config == 1) {
                    // Open up interface 0
                    qmilog("Opening interface 0 to DBL\n");
                    iface = USBOpenInterface(device, 0, 0);
                    if(iface) {
                        dbl_boot(iface, "/usr/local/standalone/firmware/Baseband/Trek/bbticket.der", "/usr/local/standalone/firmware/Baseband/Trek/osbl.mbn", "/usr/local/standalone/firmware/Baseband/Trek/amss.mbn");
                        USBCloseInterface(iface);
                        iface = NULL;
                    }
                }
                USBCloseDevice(device);
                device = NULL;
                sleep(2);
            } else {
                qmilog("Unable to find device in DBL mode\n");
            }
        }
        mInitialized = true;
        return 0;
    }
    

    int USBController::open() {
        int i = 0;
        UInt8 config;
        USBDevice device = NULL;
        
        if(mInitialized == false) {
            device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DLOAD);
            if(device) {
                USBCloseDevice(device);
                device = NULL;
                this->init();
            }
        }
        
        mDevice = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_AMSS);
        if(mDevice) {
            qmilog("Device Opened!!\n");
            config = USBSetConfiguration(mDevice, 1);
            if(config == 1) {
                qmilog("Configuration Set!!!\n");
                mInterface = USBOpenInterface(mDevice, 3, 0);
                if(mInterface) {
                    qmilog("Interface Openned!!!\n");
                    ControlPacket* response = (ControlPacket*) this->read();
                    
                    ControlPacket* request = new ControlPacket();
                    request->setService(Service::CTL);
                    request->setClient(0);
                    
                    request->setCommand(0x27);
                    this->write(request);
                    
                    for(i = 0; i < 30; i++) {
                        response = (ControlPacket*) this->read();
                        if(response) {
                            break;
                        } else {
                            sleep(1);
                        }
                    }
                    
                    request->setCommand(0x27);
                    this->write(request);
                    
                    for(i = 0; i < 30; i++) {
                        response = (ControlPacket*) this->read();
                        if(response) {
                            break;
                        } else {
                            sleep(1);
                        }
                    }
                    
                    request->setCommand(0x26);
                    request->addTLV(TLV::createFromBuffer(0x01, (unsigned char*) "\x00", 0x1));
                    request->addTLV(TLV::createFromBuffer(0x10, (unsigned char*) "\x02\x00", 0x2));
                    this->write(request);
                    request->clearTLV();
                    
                    for(i = 0; i < 30; i++) {
                        response = (ControlPacket*) this->read();
                        if(response) {
                            break;
                        } else {
                            sleep(1);
                        }
                    }
                    return 0;
                }
            }
        }
        return -1;
    }
    
    int USBController::close() {
        if(mInterface) {
            USBCloseInterface(mInterface);
            //qmilog("Interface Closed!!!\n");
            mInterface = NULL;
        }
        if(mDevice) {
            USBCloseDevice(mDevice);
            //qmilog("Device Closed!!!\n");
            mDevice = NULL;
        }
        return 0;
    }
    
    Packet* USBController::read() {
        UInt32 size = 0x2000;
        Packet* packet = NULL;
        Packet::Header* header = NULL;
        //qmilog("Reading packet from USB controller\n");
        unsigned char* data = new unsigned char[size];
        int x = USBReadControl(mInterface, 0, data, &size);
        if(x == 0 && size > 0) {
            header = (Packet::Header*) data;
            if(header->service == Service::CTL) {
                packet = new ControlPacket(data);
            } else {
                packet = new Packet(data);
            }
        }
        if(data) delete data;
        return packet;
    }

    void USBController::write(Packet* packet) {
        //qmilog("Writing packet to USB controller\n");
        packet->setSequence(mSequence);
        unsigned char* data = packet->serialize();
        Packet::Header* header = (Packet::Header*) data;
        if(packet->getService() == Service::CTL) {
            ControlPacket::Header* ctl = (ControlPacket::Header*) data;
            ctl->sequence = (unsigned char) mSequence;
        } else {
            header->sequence = mSequence;
        }
        int x = USBWriteControl(mInterface, 0, data, header->size+1);
        if(x == 0) {
            mSequence++;
        }
        if(data) delete data;
    }

}
