//
//  QMIService.h
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#ifndef __qmiclient__QMIService__
#define __qmiclient__QMIService__

#include "TLV.h"
#include "QMIController.h"

namespace QMI {
    
    class Client;
    class Service {
    public:
        typedef enum {
            CTL     = 0x00,
            WDS     = 0x01,
            DMS     = 0x02,
            NAS     = 0x03,
            QOS     = 0x04,
            WMS     = 0x05,
            PDS     = 0x06,
            AUTH    = 0x07,
            AT      = 0x08,
            VOICE   = 0x09,
            CAT2    = 0x0A,
            UIM     = 0x0B,
            PBM     = 0x0C,
            RMTFS   = 0x0D,
            LOC     = 0x0F,
            SAR     = 0x11,
            CSD     = 0x14,
            EFS     = 0x15,
            TS      = 0x17,
            TMD     = 0x18,
            CAT     = 0xE0,
            RMS     = 0xE1,
            OMA     = 0xE2
        } Type;
        
        static Service* open(Controller* controller, unsigned char type);
        
        virtual Packet* send(Packet* packet);
        virtual void close();
        
    protected:
        Service(Controller* controller, unsigned char service, unsigned char client);
        virtual ~Service();
        
        Controller* mController;
        unsigned char mService;
        unsigned char mClient;
    };
    
    
}

#endif /* defined(__qmiclient__QMIService__) */
