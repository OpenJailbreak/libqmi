//
//  NVItem.h
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#ifndef __qmiclient__NVItem__
#define __qmiclient__NVItem__

#include "QMIPacket.h"
#include "QMIService.h"

namespace QMI {

    typedef unsigned short NVItemType;
    class NVItem: public Packet {
    public:
        enum {
            ESN  = 0x0,
            AKEY = 0x19,
            SPC  = 0x55,
            MSN  = 0xB2,
            MEID = 0x797
        };
        
        NVItem(unsigned short type);
        virtual ~NVItem();
        
        TLV* getResult() {
            TLV* value = this->getTLV(0x10);
            if(value) {
                return value;
            }
            return NULL;
        }
        
    protected:
        char* mValue;
    };
}

#endif /* defined(__qmiclient__NVItem__) */
