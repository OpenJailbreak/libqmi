//
//  QMIPacket.h
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#ifndef __qmiclient__QMIPacket__
#define __qmiclient__QMIPacket__

#include <vector>

#include "TLV.h"
#include "QMIService.h"

#define QMI_ERR_NONE                              0x0000
#define QMI_ERR_MALFORMED_MSG                     0x0001
#define QMI_ERR_NO_MEMORY                         0x0002
#define QMI_ERR_INTERNAL                          0x0003
#define QMI_ERR_ABORTED                           0x0004
#define QMI_ERR_CLIENT_IDS_EXHAUSTED              0x0005
#define QMI_ERR_UNABORTABLE_TRANSACTION           0x0006
#define QMI_ERR_INVALID_CLIENT_ID                 0x0007
#define QMI_ERR_NO_THRESHOLDS                     0x0008
#define QMI_ERR_INVALID_HANDLE                    0x0009
#define QMI_ERR_INVALID_PROFILE                   0x000A
#define QMI_ERR_INVALID_PINID                     0x000B
#define QMI_ERR_INCORRECT_PIN                     0x000C
#define QMI_ERR_NO_NETWORK_FOUND                  0x000D
#define QMI_ERR_CALL_FAILED                       0x000E
#define QMI_ERR_OUT_OF_CALL                       0x000F
#define QMI_ERR_NOT_PROVISIONED                   0x0010
#define QMI_ERR_MISSING_ARG                       0x0011
#define QMI_ERR_ARG_TOO_LONG                      0x0013
#define QMI_ERR_INVALID_TX_ID                     0x0016
#define QMI_ERR_DEVICE_IN_USE                     0x0017
#define QMI_ERR_OP_NETWORK_UNSUPPORTED            0x0018
#define QMI_ERR_OP_DEVICE_UNSUPPORTED             0x0019
#define QMI_ERR_NO_EFFECT                         0x001A
#define QMI_ERR_NO_FREE_PROFILE                   0x001B
#define QMI_ERR_INVALID_PDP_TYPE                  0x001C
#define QMI_ERR_INVALID_TECH_PREF                 0x001D
#define QMI_ERR_INVALID_PROFILE_TYPE              0x001E
#define QMI_ERR_INVALID_SERVICE_TYPE              0x001F
#define QMI_ERR_INVALID_REGISTER_ACTION           0x0020
#define QMI_ERR_INVALID_PS_ATTACH_ACTION          0x0021
#define QMI_ERR_AUTHENTICATION_FAILED             0x0022
#define QMI_ERR_PIN_BLOCKED                       0x0023
#define QMI_ERR_PIN_PERM_BLOCKED                  0x0024
#define QMI_ERR_UIM_NOT_INITIALIZED               0x0025
#define QMI_ERR_MAX_QOS_REQUESTS_IN_USE           0x0026
#define QMI_ERR_INCORRECT_FLOW_FILTER             0x0027
#define QMI_ERR_NETWORK_QOS_UNAWARE               0x0028
#define QMI_ERR_INVALID_ID                        0x0029
#define QMI_ERR_REQUESTED_NUM_UNSUPPORTED         0x002A
#define QMI_ERR_INTERFACE_NOT_FOUND               0x002B
#define QMI_ERR_FLOW_SUSPENDED                    0x002C
#define QMI_ERR_INVALID_DATA_FORMAT               0x002D
#define QMI_ERR_GENERAL                           0x002E
#define QMI_ERR_UNKNOWN                           0x002F
#define QMI_ERR_INVALID_ARG                       0x0030
#define QMI_ERR_INVALID_INDEX                     0x0031
#define QMI_ERR_NO_ENTRY                          0x0032
#define QMI_ERR_DEVICE_STORAGE_FULL               0x0033
#define QMI_ERR_DEVICE_NOT_READY                  0x0034
#define QMI_ERR_NETWORK_NOT_READY                 0x0035
#define QMI_ERR_CAUSE_CODE                        0x0036
#define QMI_ERR_MESSAGE_NOT_SENT                  0x0037
#define QMI_ERR_MESSAGE_DELIVERY_FAILURE          0x0038
#define QMI_ERR_INVALID_MESSAGE_ID                0x0039
#define QMI_ERR_ENCODING                          0x003A
#define QMI_ERR_AUTHENTICATION_LOCK               0x003B
#define QMI_ERR_INVALID_TRANSITION                0x003C
#define QMI_ERR_SESSION_INACTIVE                  0x0041
#define QMI_ERR_SESSION_INVALID                   0x0042
#define QMI_ERR_SESSION_OWNERSHIP                 0x0043
#define QMI_ERR_INSUFFICIENT_RESOURCES            0x0044
#define QMI_ERR_DISABLED                          0x0045
#define QMI_ERR_INVALID_OPERATION                 0x0046
#define QMI_ERR_INVALID_QMI_CMD                   0x0047
#define QMI_ERR_TPDU_TYPE                         0x0048
#define QMI_ERR_SMSC_ADDR                         0x0049
#define QMI_ERR_INFO_UNAVAILABLE                  0x004A
#define QMI_ERR_SEGMENT_TOO_LONG                  0x004B
#define QMI_ERR_SEGMENT_ORDER                     0x004C
#define QMI_ERR_BUNDLING_NOT_SUPPORTED            0x004D
#define QMI_ERR_POLICY_MISMATCH                   0x004F
#define QMI_ERR_SIM_FILE_NOT_FOUND                0x0050
#define QMI_ERR_EXTENDED_INTERNAL                 0x0051
#define QMI_ERR_ACCESS_DENIED                     0x0052
#define QMI_ERR_HARDWARE_RESTRICTED               0x0053
#define QMI_ERR_ACK_NOT_SENT                      0x0054
#define QMI_ERR_INJECT_TIMEOUT                    0x0055

namespace QMI {
    
    class Packet {
    public:
        typedef struct {
            unsigned char code;
            unsigned short size;
            unsigned char qmi_flags;
            unsigned char service;
            unsigned char client;
            unsigned char sdu_flags;
            unsigned short sequence;
            unsigned short command;
            unsigned short length;
            unsigned char data[0];
        } __attribute__((packed)) Header;
        
        Packet();
        virtual ~Packet();
        Packet(unsigned short cmd);
        Packet(unsigned char* data);
        
        static Packet* create(unsigned short cmd);
        static Packet* create(unsigned char* data);
        
        virtual unsigned char* serialize();
        
        void setSequence(unsigned short sequence) {
            mSequence = sequence;
        }
        
        void setCommand(unsigned short command) {
            mCommand = command;
        }
        
        void setService(unsigned char service) {
            mService = service;
        }
        
        void setClient(unsigned char client) {
            mClient = client;
        }
        
        unsigned short getSequence() {
            return mSequence;
        }
        
        unsigned short getCommand() {
            return mCommand;
        }
        
        unsigned char getService() {
            return mService;
        }
        
        unsigned char getClient() {
            return mClient;
        }
        
        void clearTLV();
        void addTLV(TLV* tlv);
        TLV* getTLV(unsigned char type);
        
        void printError();
        
    protected:
        unsigned short mError;
        unsigned short mCommand;
        unsigned short mSequence;
        unsigned char mService;
        unsigned char mClient;
        unsigned int mNumTLV;
        TLV::List* mTLVList;
        
    };
    
}

#endif /* defined(__qmiclient__QMIPacket__) */
