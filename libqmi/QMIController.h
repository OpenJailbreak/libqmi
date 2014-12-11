//
//  QMIController.h
//  qmiController
//
//  Created by Joshua Hill on 2/8/13.
//
//

#ifndef __qmiController__QMIController__
#define __qmiController__QMIController__

namespace QMI {
    
    class Packet;
    
class Controller {
public:
    virtual ~Controller() { }
    
    static Controller* create();
    
    virtual int open() = 0;
    virtual int close() = 0;
    
    virtual void write(Packet* packet) = 0;
    virtual Packet* read() = 0;
    
protected:
    unsigned short mSequence;
};
    
}

#endif /* defined(__qmiController__QMIController__) */
