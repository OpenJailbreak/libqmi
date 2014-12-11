//
//  TLV.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#include <string>
#include <vector>

#include "TLV.h"

namespace QMI {
    
    typedef std::vector<TLV*> TLVList;
    typedef std::vector<TLV*>::iterator TLVListIterator;
    
    TLV::TLV(unsigned char type, unsigned int length, unsigned char* value) :
        mType(type), mLength(length) {
            mValue = new unsigned char[length];
            memcpy(mValue, value, length);
    }
    
    TLV::~TLV() {
        mType = 0;
        mLength = 0;
        delete mValue;
    }
    
    TLV* TLV::createFromInteger(unsigned char type, unsigned int integer) {
        return new TLV(type, sizeof(unsigned int), (unsigned char*) &integer);
    }
    
    TLV* TLV::createFromBuffer(unsigned char type, unsigned char* buffer, unsigned int size) {
        return new TLV(type, size, buffer);
    }
    
}