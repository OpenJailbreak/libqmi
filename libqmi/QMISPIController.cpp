//
//  QMISPIController.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//
//

extern "C" {
    #include <stdio.h>
    #include <unistd.h>
    #include <stdlib.h>
    #include <fcntl.h>
}

#include "QMIPacket.h"
#include "QMIControlPacket.h"
#include "QMISPIController.h"

namespace QMI {

    SPIController::SPIController() : Controller() {
    
    }

    SPIController::~SPIController() {
    
    }
    
    int SPIController::open() {
        if(mSocket) {
            return 0;
        }
        
        mSocket = ::open("/dev/dlci.spi-baseband.qmux2_c", O_RDWR | O_NOCTTY);
        if(mSocket > 0) {
            printf("yay, connected!!\n");
        } else {
            return -1;
        }
        
        return 0;
    }
    
    int SPIController::close() {
        if(mSocket > 0) {
            ::close(mSocket);
            mSocket = 0;
        }
        return 0;
    }
    
    void SPIController::write(Packet* packet) {
        packet->setSequence(mSequence);
        unsigned char* data = packet->serialize();
        Packet::Header* header = (Packet::Header*) data;
        if(packet->getService() == Service::CTL) {
            ControlPacket::Header* ctl = (ControlPacket::Header*) data;
            ctl->sequence = (unsigned char) mSequence;
        } else {
            header->sequence = mSequence;
        }
        int x = ::write(mSocket, data, header->size+1);
        if(x > 0) {
            mSequence++;
        }
        if(data) delete data;
    }
    
    Packet* SPIController::read() {
        uint32_t size = 0x2000;
        Packet* packet = NULL;
        Packet::Header* header = NULL;
        unsigned char* data = new unsigned char[size];
        int x = ::read(mSocket, data, size);
        if(x > 0 && data) {
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

}
