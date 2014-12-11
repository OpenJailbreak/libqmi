//
//  QMISPIController.h
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//
//

#ifndef __qmiclient__QMISPIController__
#define __qmiclient__QMISPIController__

#include "QMIPacket.h"
#include "QMIController.h"

namespace QMI {

class SPIController: public Controller {
public:
    SPIController();
    virtual ~SPIController();
    
    virtual int open();
    virtual int close();
    
    virtual void write(Packet* packet);
    virtual Packet* read();
    
private:
    int mSocket;
    
};
    
}

#endif /* defined(__qmiclient__QMISPIController__) */
