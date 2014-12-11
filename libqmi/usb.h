//
//  usb.h
//  dloadtool
//
//  Created by Joshua Hill on 1/31/13.
//
//

#ifndef dloadtool_usb_h
#define dloadtool_usb_h

#include <IOKit/usb/IOUSBLib.h>

typedef IOUSBDeviceInterface** USBDevice;
typedef IOUSBInterfaceInterface** USBInterface;

void USBCloseDevice(USBDevice device);
USBDevice USBOpenDevice(SInt32 vendorId, SInt32 productId);

UInt8 USBSetConfiguration(USBDevice device, UInt8 configuration);

USBInterface USBOpenInterface(USBDevice device, UInt8 interface, UInt8 alt_interface);
void USBCloseInterface(USBInterface interface);

int USBWriteBulk(USBInterface iface, UInt8 pipe, void* buf, UInt32 size);
int USBReadBulk(USBInterface iface, UInt8 pipe, void* buf, UInt32* size);

int USBReadControl(USBInterface iface, UInt8 pipe, void* buf, UInt32* size);
int USBWriteControl(USBInterface iface, UInt8 pipe, void* buf, UInt32 size);

#endif
