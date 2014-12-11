//
//  QMIControlPacket.cpp
//  qmifuzzer
//
//  Created by Joshua Hill on 4/16/13.
//
//

#include "QMIControlPacket.h"

#include "util.h"

namespace QMI {
    ControlPacket::ControlPacket(): Packet() {
    }
    
    ControlPacket::~ControlPacket() {
        
    }
    
    ControlPacket::ControlPacket(unsigned char* data): Packet() {
        ControlPacket::Header* header = (ControlPacket::Header*) data;
        setSequence(header->sequence);
        setService(header->service);
        setCommand(header->command);
        setClient(header->client);
        TLV::Info* info = NULL;
        unsigned int offset = sizeof(ControlPacket::Header);
        while(offset < header->size) {
            info = (TLV::Info*) &data[offset];
            addTLV(TLV::createFromBuffer(info->type, info->value, info->length));
            offset += sizeof(TLV::Info) + info->length;
        }

    }
    
    unsigned char* ControlPacket::serialize() {
        TLV::ListIterator it;
        unsigned int tlv_size = 0;
        for(it = mTLVList->begin(); it != mTLVList->end(); ++it) {
            tlv_size += (*it)->getLength() + 3;
        }
        
        unsigned int offset = 0;
        unsigned char* tlv_data = new unsigned char[tlv_size];
        for(it = mTLVList->begin(); it != mTLVList->end(); ++it) {
            memcpy(&tlv_data[offset], (*it)->getInfo(), sizeof(TLV::Info) + (*it)->getLength());
            offset += sizeof(TLV::Info) + (*it)->getLength();
        }
        
        unsigned int qmi_size = sizeof(ControlPacket::Header) + tlv_size;
        unsigned char* qmi_data = new unsigned char[qmi_size];
        ControlPacket::Header* header = (ControlPacket::Header*) qmi_data;
        header->code = 1;
        header->size = qmi_size - 1;
        header->qmi_flags = 0;
        header->service = mService;
        header->client = mClient;
        header->sequence = (unsigned char) mSequence;
        header->sdu_flags = 0;
        header->length = tlv_size;
        header->command = mCommand;
        memcpy(header->data, tlv_data, tlv_size);
        if(tlv_data) delete tlv_data;
        return qmi_data;
    }
}