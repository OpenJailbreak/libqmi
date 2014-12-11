//
//  usb.cpp
//  dloadtool
//
//  Created by Joshua Hill on 1/31/13.
//
//

#include <iostream>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <CoreFoundation/CoreFoundation.h>

#include "usb.h"
#include "util.h"

void USBCloseDevice(USBDevice device) {
    (*device)->USBDeviceClose(device);
    (*device)->Release(device);
}

USBDevice USBOpenDevice(SInt32 vendorId, SInt32 productId) {
    SInt32 score;
    HRESULT result;
    kern_return_t kr;
    io_service_t service;
    mach_port_t masterPort;
    IOCFPlugInInterface** plugin;
    IOUSBDeviceInterface** device;
    CFMutableDictionaryRef matchingDict;
    
    SInt32 vendor = vendorId;
    SInt32 product = productId;
    
    kr = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (kr || !masterPort) {
        qmilog("Couldn’t create a master IOKit port\n");
        return NULL;
    }
    
    matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    if (!matchingDict) {
        qmilog("Couldn’t create a USB matching dictionary\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        return NULL;
    }
    
    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorName),
                         CFNumberCreate(kCFAllocatorDefault,
                                        kCFNumberSInt32Type, &vendor));
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductName),
                         CFNumberCreate(kCFAllocatorDefault,
                                        kCFNumberSInt32Type, &product));
    
    service = IOServiceGetMatchingService(masterPort, matchingDict);
    if(!service) {
        //qmilog("Couldn't find matching USB service\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        return NULL;
    }
    
    kr = IOCreatePlugInInterfaceForService(service, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugin, &score);
    if(kr != kIOReturnSuccess) {
        qmilog("Couldn't create USB device interface plugin\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        return NULL;
    }
    IOObjectRelease(service);
    
    result = (*plugin)->QueryInterface(plugin, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID*) &device);
    (*plugin)->Release(plugin);
    
    if(result || !device) {
        qmilog("Couldn't query USB device interface plugin\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        return NULL;
    }
    
    kr = (*device)->USBDeviceOpen(device);
    if(kr) {
        qmilog("Couldn't open USB device\n");
        mach_port_deallocate(mach_task_self(), masterPort);
        (*device)->Release(device);
        return NULL;
    }
    
    return device;
}


UInt8 USBSetConfiguration(USBDevice device, UInt8 configuration) {
    UInt8 num;
    UInt8 config;
    UInt8 current;
    kern_return_t kr;
    if(device) {
        kr = (*device)->GetConfiguration(device, &current);
        if(current != configuration) {
            kr = (*device)->GetNumberOfConfigurations(device, &num);
            if(configuration <= num) {
                (*device)->SetConfiguration(device, configuration);
                config = configuration;
            } else config = current;
        } else config = current;
    } else config = -1;
    
    return config;
}


USBInterface USBOpenInterface(USBDevice device, UInt8 interface, UInt8 alt_interface) {
    UInt8 alt;
    UInt8 current;
    SInt32 score;
    HRESULT result;
    kern_return_t kr;
    io_service_t service;
    io_iterator_t iterator;
    IOCFPlugInInterface** plugin;
    IOUSBInterfaceInterface** iface;
    IOUSBFindInterfaceRequest request;
    
    if(device) {
        request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
        request.bAlternateSetting = kIOUSBFindInterfaceDontCare;
        request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
        request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
        
        kr = (*device)->CreateInterfaceIterator(device, &request, &iterator);
        if(kr != kIOReturnSuccess) {
            qmilog("Couldn't create USB device interface iterator\n");
            return NULL;
        }
        
        while((service = IOIteratorNext(iterator))) {
            kr = IOCreatePlugInInterfaceForService(service, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID,
                                                   &plugin, &score);
            if(kr != kIOReturnSuccess) {
                qmilog("Couldn't create USB device interface plugin\n");
                continue;
            }
            IOObjectRelease(service);
            
            result = (*plugin)->QueryInterface(plugin, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID), (LPVOID*) &iface);
            (*plugin)->Release(plugin);
            
            if(result || !iface) {
                qmilog("Couldn't query USB device interface plugin\n");
                continue;
            }
            
            kr = (*iface)->GetInterfaceNumber(iface, &current);
            if(kr != kIOReturnSuccess) {
                qmilog("Couldn't get current USB device interface\n");
                (*iface)->Release(iface);
                continue;
            }
            
            //qmilog("Checking interface %d\n", current);
            
            if(current == interface) {
                kr = (*iface)->USBInterfaceOpen(iface);
                if(kr != kIOReturnSuccess) {
                    qmilog("Couldn't open USB device interface\n");
                    (*iface)->Release(iface);
                    return NULL;
                }
                
                if(alt_interface != 0) {
                    kr = (*iface)->GetAlternateSetting(iface, &alt);
                    if(kr != kIOReturnSuccess) {
                        qmilog("Couldn't get USB device interface alternate setting\n");
                        (*iface)->USBInterfaceClose(iface);
                        (*iface)->Release(iface);
                        return NULL;
                    }
                    if(alt_interface == alt) {
                        kr = (*iface)->SetAlternateInterface(iface, alt);
                    } else {
                        qmilog("Invalid alternate interface\n");
                    }
                }
                break;
            }
        }
    }
    
    return iface;
}

void USBCloseInterface(USBInterface iface) {
    (*iface)->USBInterfaceClose(iface);
    //(*iface)->Release(iface);
}

int USBWriteBulk(USBInterface iface, UInt8 pipe, void* buf, UInt32 size) {
    kern_return_t kr;
    if(iface) {
        kr = (*iface)->WritePipe(iface, pipe, buf, size);
        if(kr != kIOReturnSuccess) {
            qmilog("Couldn't write to USB device interface\n");
            return -1;
        }
    }
    return 0;
}

int USBReadBulk(USBInterface iface, UInt8 pipe, void* buf, UInt32* size) {
    kern_return_t kr;
    if(iface) {
        kr = (*iface)->ReadPipe(iface, pipe, buf, size);
        if(kr != kIOReturnSuccess) {
            qmilog("Couldn't read from USB device interface\n");
            return -1;
        }
    }
    return 0;
}

int USBReadControl(USBInterface iface, UInt8 pipe, void* buf, UInt32* size) {
    kern_return_t kr;
    if(iface) {
        IOUSBDevRequest req;
        req.bmRequestType = 0xa1;
        req.bRequest = 0x1;
        req.pData = buf;
        req.wIndex = 3;
        req.wValue = 0;
        req.wLenDone = 0;
        req.wLength = *size;
        kr = (*iface)->ControlRequest(iface, 0, (IOUSBDevRequest*)&req);
        if(kr == kIOReturnSuccess) {
            *size = req.wLenDone;
            hexdump((unsigned char*) req.pData, req.wLenDone);
            return 0;
        }
        *size = 0;
    }
    return -1;
}

int USBWriteControl(USBInterface iface, UInt8 pipe, void* buf, UInt32 size) {
    kern_return_t kr;
    if(iface) {
        IOUSBDevRequest req;
        req.bmRequestType = 0x21;
        req.bRequest = 0x0;
        req.pData = buf;
        req.wIndex = 3;
        req.wValue = 0;
        req.wLenDone = 0;
        req.wLength = size;
        kr = (*iface)->ControlRequest(iface, 0, (IOUSBDevRequest*)&req);
        if(kr == kIOReturnSuccess) {
            hexdump((unsigned char*) req.pData, req.wLength);
            return 0;
        }
    }
    return -1;
}
