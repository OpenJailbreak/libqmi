//
//  QMIClient.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/14/13.
//
//

#include <unistd.h>

#include <iostream>

#include "bb.h"

#include "QMIClient.h"
#include "QMIPacket.h"
#include "QMISPIController.h"
#include "QMIUSBController.h"
#include "QMILibController.h"

#include "util.h"

namespace QMI {
    
    Client::Client(Controller* controller): mController(controller) {
        
    }
    
    Client::~Client() {
        if(mController) delete mController;
    }
    
    Client* Client::create() {
        int x = 0;
        Client* client = NULL;
        Controller* controller = NULL;
        
        // Use this controller is on iPhone4 CDMA
        if(access("/dev/dlci.spi-baseband.qmux2_c", F_OK) == 0) {
            controller = new SPIController();
        } else {
            // Use this controller is CommCenter is unloaded
            controller = new USBController();
        }
        //
        // Experimental: Use this controller is CommCenter is still loaded
        //controller = new LibController();
        if(controller) {
             x = controller->open();
            if(x == 0) {
                client = new Client(controller);
            }
        }
        
        return client;
    }
    
    Service* Client::createService(unsigned char type) {
        Service* service = Service::open(mController, type);
        return service;
    }
    
    Packet* Client::readPacket() {
        return mController->read();
    }
    
    int Client::writePacket(Packet* packet) {
        mController->write(packet);
        return 0;
    }
    
    NVItem* Client::readNVItem(unsigned short item) {
        return NULL;
    }
    
    int Client::writeNVItem(NVItem* item) {
        return 0;
    }
    
}