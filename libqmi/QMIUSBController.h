//
//  QMIUSBController.h
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//
//

#ifndef __qmiclient__QMIUSBController__
#define __qmiclient__QMIUSBController__

#include "QMIPacket.h"
#include "QMIController.h"

#include <iostream>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <CoreFoundation/CoreFoundation.h>

#include "usb.h"

namespace QMI {

class USBController: public Controller {
public:
    USBController();
    virtual ~USBController();
    
    virtual int init();
    virtual int open();
    virtual int close();
    
    virtual void write(Packet* packet);
    virtual Packet* read();
    
protected:
    USBDevice mDevice;
    USBInterface mInterface;
    unsigned short mSequence;
    bool mInitialized;
    
};
    
}

#endif /* defined(__qmiclient__QMIUSBController__) */
