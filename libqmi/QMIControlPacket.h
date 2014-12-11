//
//  QMIControlPacket.h
//  qmifuzzer
//
//  Created by Joshua Hill on 4/16/13.
//
//

#ifndef __qmifuzzer__QMIControlPacket__
#define __qmifuzzer__QMIControlPacket__

#include <iostream>

#include "QMIPacket.h"

namespace QMI {
    class ControlPacket: public Packet {
    public:
        
        typedef struct {
            unsigned char code;
            unsigned short size;
            unsigned char qmi_flags;
            unsigned char service;
            unsigned char client;
            unsigned char sdu_flags;
            unsigned char sequence;
            unsigned short command;
            unsigned short length;
            unsigned char data[0];
        } __attribute__((packed)) Header;
        
        ControlPacket();
        virtual ~ControlPacket();
        ControlPacket(unsigned char* data);
        
        virtual unsigned char* serialize();
        
    protected:
        
    };
}

#endif /* defined(__qmifuzzer__QMIControlPacket__) */
