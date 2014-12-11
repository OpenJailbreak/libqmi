//
//  QMIPacket.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#include "QMIPacket.h"

#include "TLV.h"

#include "util.h"

namespace QMI {
    
    Packet::Packet(): mSequence(0), mCommand(0), mService(0), mTLVList(0), mError(0) {
        mTLVList = new TLV::List;
    }
    
    Packet::~Packet() {
        mTLVList->clear();
        if(mTLVList) delete mTLVList;
    }
    
    Packet::Packet(unsigned char* data): mSequence(0), mCommand(0), mService(0), mTLVList(0), mError(0) {
        mTLVList = new TLV::List;
        Packet::Header* header = (Packet::Header*) data;
        setSequence(header->sequence);
        setService(header->service);
        setCommand(header->command);
        setClient(header->client);
        TLV::Info* info = NULL;
        unsigned int offset = sizeof(Packet::Header);
        while(offset < header->size) {
            info = (TLV::Info*) &data[offset];
            if(info->type == 0x2) {
                // This is an error
                //mError = (unsigned short)info->value;
            }
            addTLV(TLV::createFromBuffer(info->type, info->value, info->length));
            offset += sizeof(TLV::Info) + info->length;
        }
    }
    
    
    Packet::Packet(unsigned short cmd): mSequence(0), mCommand(cmd), mService(0), mTLVList(0), mError(0) {
        mTLVList = new TLV::List;
    }
    
    Packet* Packet::create(unsigned short cmd) {
        return new Packet(cmd);
    }
    
    Packet* Packet::create(unsigned char* data) {
        return new Packet(data);
    }
    
    unsigned char* Packet::serialize() {
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
        
        unsigned int qmi_size = sizeof(Packet::Header) + tlv_size;
        unsigned char* qmi_data = new unsigned char[qmi_size];
        Packet::Header* header = (Packet::Header*) qmi_data;
        header->code = 1;
        header->size = qmi_size - 1;
        header->qmi_flags = 0;
        header->service = mService;
        header->client = mClient;
        header->sequence = mSequence;
        header->sdu_flags = 0;
        header->length = tlv_size;
        header->command = mCommand;
        memcpy(header->data, tlv_data, tlv_size);
        if(tlv_data) delete tlv_data;
        return qmi_data;
    }
    
    void Packet::addTLV(TLV* tlv) {
        mTLVList->push_back(tlv);
    }
    
    void Packet::clearTLV() {
        mTLVList->clear();
        //mTLVList->erase(mTLVList->begin(), mTLVList->end());
    }
    
    
    TLV* Packet::getTLV(unsigned char type) {
        TLV::ListIterator it;
        for(it = mTLVList->begin(); it != mTLVList->end(); ++it) {
            if((*it)->getType() == type) {
                return *it;
            }
        }
        return NULL;
    }
    
    void Packet::printError() {
        switch(mError) {
            case QMI_ERR_NONE:
                printf("No Error\n");
                break;
            case QMI_ERR_MALFORMED_MSG:
                printf("Malformed Message\n");
                break;
            case QMI_ERR_NO_MEMORY:
                printf("No Memory\n");
                break;
            case QMI_ERR_INTERNAL:
                printf("Internal Error\n");
                break;
            case QMI_ERR_ABORTED:
                printf("Aborted\n");
                break;
            case QMI_ERR_CLIENT_IDS_EXHAUSTED:
                printf("Client IDs Exhausted\n");
                break;
            case QMI_ERR_UNABORTABLE_TRANSACTION:
                printf("Unabortable Transaction\n");
                break;
            case QMI_ERR_INVALID_CLIENT_ID:
                printf("Invalid Client ID\n");
                break;
            case QMI_ERR_NO_THRESHOLDS:
                printf("No Thresholds\n");
                break;
            case QMI_ERR_INVALID_HANDLE:
                printf("Invalid Handle\n");
                break;
            case QMI_ERR_INVALID_PROFILE:
                printf("Invalid Profile\n");
                break;
            case QMI_ERR_INVALID_PINID:
                printf("Invalid PIN ID\n");
                break;
            case QMI_ERR_INCORRECT_PIN:
                printf("Incorrect PIN\n");
                break;
            case QMI_ERR_NO_NETWORK_FOUND:
                printf("No Network Found\n");
                break;
            case QMI_ERR_CALL_FAILED:
                printf("Call Failed\n");
                break;
            case QMI_ERR_OUT_OF_CALL:
                printf("Out Of Call\n");
                break;
            case QMI_ERR_NOT_PROVISIONED:
                printf("Not Provisioned\n");
                break;
            case QMI_ERR_MISSING_ARG:
                printf("Missing Argument\n");
                break;
            case QMI_ERR_ARG_TOO_LONG:
                printf("Argument Too Long\n");
                break;
            case QMI_ERR_INVALID_TX_ID:
                printf("Invalid Transfer Use\n");
                break;
            case QMI_ERR_DEVICE_IN_USE:
                printf("Device In Use\n");
                break;
            case QMI_ERR_OP_NETWORK_UNSUPPORTED:
                printf("Operator Network Unsupported\n");
                break;
            case QMI_ERR_OP_DEVICE_UNSUPPORTED:
                printf("Operator Device Unsupported\n");
                break;
            case QMI_ERR_NO_EFFECT:
                printf("No Effect\n");
                break;
            case QMI_ERR_NO_FREE_PROFILE:
                printf("No Free Profile\n");
                break;
            case QMI_ERR_INVALID_PDP_TYPE:
                printf("Invalid PDP Type\n");
                break;
            case QMI_ERR_INVALID_TECH_PREF:
                printf("Invalid Tech Pref\n");
                break;
            case QMI_ERR_INVALID_PROFILE_TYPE:
                printf("Invalid Profile Type\n");
                break;
            case QMI_ERR_INVALID_SERVICE_TYPE:
                printf("Invalid Service Type\n");
                break;
            case QMI_ERR_INVALID_REGISTER_ACTION:
                printf("Invalid Register Action\n");
                break;
            case QMI_ERR_INVALID_PS_ATTACH_ACTION:
                printf("Invalid PS Attach Action\n");
                break;
            case QMI_ERR_AUTHENTICATION_FAILED:
                printf("Authentication Failed\n");
                break;
            case QMI_ERR_PIN_BLOCKED:
                printf("PIN Blocked\n");
                break;
            case QMI_ERR_PIN_PERM_BLOCKED:
                printf("PIN Permissions Blocked\n");
                break;
            case QMI_ERR_UIM_NOT_INITIALIZED:
                printf("UIM Not Initialized\n");
                break;
            case QMI_ERR_MAX_QOS_REQUESTS_IN_USE:
                printf("Max QOS Requests In Use\n");
                break;
            case QMI_ERR_INCORRECT_FLOW_FILTER:
                printf("Incorrect Flow Filter\n");
                break;
            case QMI_ERR_NETWORK_QOS_UNAWARE:
                printf("Network QOS Unaware\n");
                break;
            case QMI_ERR_INVALID_ID:
                printf("Invalid ID\n");
                break;
            case QMI_ERR_REQUESTED_NUM_UNSUPPORTED:
                printf("Requested Number Unsupported\n");
                break;
            case QMI_ERR_INTERFACE_NOT_FOUND:
                printf("Interface Not Found\n");
                break;
            case QMI_ERR_FLOW_SUSPENDED:
                printf("Flow Suspended\n");
                break;
            case QMI_ERR_INVALID_DATA_FORMAT:
                printf("Invalid Data Format\n");
                break;
            case QMI_ERR_GENERAL:
                printf("General Error\n");
                break;
            case QMI_ERR_UNKNOWN:
                printf("Unknown Error\n");
                break;
            case QMI_ERR_INVALID_ARG:
                printf("Invalid Argument\n");
                break;
            case QMI_ERR_INVALID_INDEX:
                printf("Invalid Index\n");
                break;
            case QMI_ERR_NO_ENTRY:
                printf("No Entry\n");
                break;
            case QMI_ERR_DEVICE_STORAGE_FULL:
                printf("Device Storage Full\n");
                break;
            case QMI_ERR_DEVICE_NOT_READY:
                printf("Device Not Ready\n");
                break;
            case QMI_ERR_NETWORK_NOT_READY:
                printf("Network Not Ready\n");
                break;
            case QMI_ERR_CAUSE_CODE:
                printf("Cause Code!!\n");
                break;
            case QMI_ERR_MESSAGE_NOT_SENT:
                printf("Message Not Sent\n");
                break;
            case QMI_ERR_MESSAGE_DELIVERY_FAILURE:
                printf("Message Delivery Failure\n");
                break;
            case QMI_ERR_INVALID_MESSAGE_ID:
                printf("Invalid Message ID\n");
                break;
            case QMI_ERR_ENCODING:
                printf("Encoding Error\n");
                break;
            case QMI_ERR_AUTHENTICATION_LOCK:
                printf("Authentication Lock\n");
                break;
            case QMI_ERR_INVALID_TRANSITION:
                printf("Invalid Transition\n");
                break;
            case QMI_ERR_SESSION_INACTIVE:
                printf("Session Inactive\n");
                break;
            case QMI_ERR_SESSION_INVALID:
                printf("Session Invalid\n");
                break;
            case QMI_ERR_SESSION_OWNERSHIP:
                printf("Session Ownership\n");
                break;
            case QMI_ERR_INSUFFICIENT_RESOURCES:
                printf("Insufficient Resources\n");
                break;
            case QMI_ERR_DISABLED:
                printf("Disabled\n");
                break;
            case QMI_ERR_INVALID_OPERATION:
                printf("Invalid Operation\n");
                break;
            case QMI_ERR_INVALID_QMI_CMD:
                printf("Invalid QMI Command\n");
                break;
            case QMI_ERR_TPDU_TYPE:
                printf("TPDU Type\n");
                break;
            case QMI_ERR_SMSC_ADDR:
                printf("SMSC Address\n");
                break;
            case QMI_ERR_INFO_UNAVAILABLE:
                printf("Info Unavailable\n");
                break;
            case QMI_ERR_SEGMENT_TOO_LONG:
                printf("Segment Too Long\n");
                break;
            case QMI_ERR_SEGMENT_ORDER:
                printf("Segment Order\n");
                break;
            case QMI_ERR_BUNDLING_NOT_SUPPORTED:
                printf("Bundling Not Supported\n");
                break;
            case QMI_ERR_POLICY_MISMATCH:
                printf("Policy Mismatch\n");
                break;
            case QMI_ERR_SIM_FILE_NOT_FOUND:
                printf("SIM File Not Found\n");
                break;
            case QMI_ERR_EXTENDED_INTERNAL:
                printf("Extended Internal\n");
                break;
            case QMI_ERR_ACCESS_DENIED:
                printf("Access Denied\n");
                break;
            case QMI_ERR_HARDWARE_RESTRICTED:
                printf("Hardware Restricted\n");
                break;
            case QMI_ERR_ACK_NOT_SENT:
                printf("Ack Not Sent\n");
                break;
            case QMI_ERR_INJECT_TIMEOUT:
                printf("Injection Timeout\n");
                break;
            default:
                printf("Unknown Error\n");
                break;
        }
    }
    
}