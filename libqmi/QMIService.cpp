//
//  QMIService.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

extern "C" {
#include <unistd.h>
}

#include "QMIService.h"
#include "QMIController.h"
#include "QMIControlPacket.h"

#include "util.h"

namespace QMI {
    
    Service::Service(Controller* controller, unsigned char service, unsigned char client):
        mController(controller), mService(service), mClient(client) {
        
    }
    
    Service::~Service() {
        
    }
    
    Service* Service::open(Controller* controller, unsigned char type) {
        int i = 0;
        unsigned char client;
        
        ControlPacket* request = new ControlPacket;
        ControlPacket* response = new ControlPacket;

        request->setService(Service::CTL);
        request->setClient(0);
        request->setCommand(0x22);
        request->setSequence(0x04);
        request->addTLV(TLV::createFromBuffer(0x01, (unsigned char*) &type, 0x1));
        controller->write(request);

        for(i = 0; i < 30; i++) {
            response = (ControlPacket*) controller->read();
            if(response) {
                break;
            } else {
                sleep(1);
            }
        }
        
        if(response) {
            TLV* result = response->getTLV(0x01);
            if(result) {
                unsigned char* value = result->getValue();
                if(value[0] == type) {
                    client = value[1];
                }
            }
        }
        Service* service = new Service(controller, type, client);
        
        if(request) delete request;
        if(response) delete response;
        return service;
    }
    
    void Service::close() {
        delete this;
    }
    
    Packet* Service::send(Packet* packet) {
        int i = 0;
        unsigned short sequence = 0;
        packet->setClient(mClient);
        packet->setService(mService);
        Packet* response = NULL;
        mController->write(packet);
        sequence = packet->getSequence();
        for(i = 0; i < 30; i++) {
            response = mController->read();
            if(response && response->getSequence() == sequence) {
                return response;
            } sleep(1);
        }
        return NULL;
    }
    
}