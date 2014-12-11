//
//  QMIClient.h
//  qmiclient
//
//  Created by Joshua Hill on 2/14/13.
//
//

#ifndef __qmiclient__QMIClient__
#define __qmiclient__QMIClient__

#include "bb.h"

#include "NVItem.h"
#include "QMIPacket.h"
#include "QMIService.h"
#include "QMIController.h"

namespace QMI {
    class Client {
    public:
        virtual ~Client();
        
        static Client* create();
        
        Service* createService(unsigned char type);
        
        Packet* readPacket();
        int writePacket(Packet* packet);
        
        NVItem* readNVItem(unsigned short item);
        int writeNVItem(NVItem* item);
        
        static void resetBaseband() {
            bb_reset();
        }
        static void enterDownloadMode() {
            bb_enter_download_mode();
        }

    protected:
        Client(Controller* controller);
        
    private:
        Controller* mController;
        
    };
}

#endif /* defined(__qmiclient__QMIClient__) */
