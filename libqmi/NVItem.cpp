//
//  NVItem.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#include "TLV.h"
#include "NVItem.h"

namespace QMI {

    NVItem::NVItem(unsigned short item) {
        this->setCommand(0x5557);
        this->addTLV(TLV::createFromInteger(0x01, (unsigned int) item));
    }

    NVItem::~NVItem() {
    }

}