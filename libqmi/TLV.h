//
//  TLV.h
//  qmiclient
//
//  Created by Joshua Hill on 2/13/13.
//
//

#ifndef __qmiclient__TLV__
#define __qmiclient__TLV__

#include <vector>
#include <iterator>

namespace QMI {
    
    class TLV {
    public:
        typedef std::vector<TLV*> List;
        typedef std::vector<TLV*>::iterator ListIterator;
        
        typedef struct {
            unsigned char type;
            unsigned short length;
            unsigned char value[0];
        } __attribute__((packed)) Info;
        
        TLV(unsigned char type, unsigned int length, unsigned char* value);
        virtual ~TLV();

        static TLV* createFromInteger(unsigned char type, unsigned int integer);
        static TLV* createFromBuffer(unsigned char type, unsigned char* buffer, unsigned int size);
        
        // Type Accessors
        unsigned char getType() {
            return mType;
        }
        void setType(unsigned char type) {
            mType = type;
        }
        
        // Length Accessors
        unsigned int getLength() {
            return mLength;
        }
        void setLength(unsigned int length) {
            mLength = length;
        }
        
        // Value Accessors
        unsigned char* getValue() {
            return mValue;
        }
        void setValue(unsigned char* value, unsigned int length) {
            if(mValue) {
                delete mValue;
                mValue = NULL;
            }
            mLength = length;
            mValue = new unsigned char[length];
            memcpy(mValue, value, mLength);
        }
        
        // Returns TLV Struct
        Info* getInfo() {
            unsigned char* buffer = new unsigned char[sizeof(Info) + mLength];
            Info* info = (Info*) buffer;
            info->type = mType;
            info->length = mLength;
            memcpy(info->value, mValue, mLength);
            return info;
        }
        void setInfo(unsigned char type, unsigned int length, unsigned char* value) {
            if(mValue) {
                delete mValue;
                mValue = NULL;
            }
            mType = type;
            mLength = length;
            mValue = new unsigned char[length];
            memcpy(mValue, value, mLength);
        }
    
        
    private:
        unsigned char mType;
        unsigned int mLength;
        unsigned char* mValue;
        
    };

}

#endif /* defined(__qmiclient__TLV__) */
