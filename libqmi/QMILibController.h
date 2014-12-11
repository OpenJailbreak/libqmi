//
//  QMILibController.h
//  libqmi
//
//  Created by Joshua Hill on 7/10/13.
//
//

#ifndef __libqmi__QMILibController__
#define __libqmi__QMILibController__

extern "C" {
    #include <dlfcn.h>
    #include <mach/mach.h>
#include <mach-o/nlist.h>
#include <dispatch/dispatch.h>
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <set>
#include <list>
#include <string>
#include <iostream>

#include "util.h"

#include "QMIPacket.h"
#include "QMIController.h"

// Preferences
#define LOAD_IOKIT 0

// Libraries
#define USBControlPath "/usr/lib/libUSBControlDynamic.dylib"
//                           /usr/lib/libATCommandStudioDynamic.dylib
#define ATCommandStudioPath "/usr/lib/libATCommandStudioDynamic.dylib"
#define IOKitPath "/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit"

// Accessors
/*
#ifdef LOAD_IOKIT
#define IOKit_MasterPortDefault kIOMasterPortDefault
#define IOKit_ServiceGetMatchingService IOServiceGetMatchingService
#define IOKit_ServiceMatching IOServiceMatching
#define IOKit_ServiceOpen IOServiceOpen
#define IOKit_ServiceClose IOServiceClose
#define IOKit_ConnectCallScalarMethod IOConnectCallScalarMethod
#endif
 */
//             _ZN4QMuxC1EP13ATCSIPCDriverPvRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEP16ATCSResetInvokerbb
//  _ZN4QMuxC1EP13ATCSIPCDriverPvRKSsP16ATCSResetInvokerbb
#define QMux_QMux _ZN4QMuxC1EP13ATCSIPCDriverPvRKSsP16ATCSResetInvokerbb
#define QMIClient_requestClient _ZN9QMIClient13requestClientERKSsN3qmi11ServiceTypeERK4QMuxP17QMIClientCallbackbb

#define USBQMIInterface_engage _ZN15USBQMIInterface6engageEv
#define USBQMIInterface_create _ZN15USBQMIInterface6createEPNS_18USBInterfaceStructE
#define USBQMIInterface_readUSBController _ZN15USBQMIInterface17readUSBControllerEiPhi
#define USBQMIInterface_writeUSBController _ZN15USBQMIInterface18writeUSBControllerEiPhi
#define USBQMIInterface_createUSBController _ZN15USBQMIInterface19createUSBControllerEPKc
//#define USBQMIInterface_getFatalErrorReason _ZN15USBQMIInterface19getFatalErrorReasonEv
//#define USBQMIInterface_hasFatalErrorOccured _ZN15USBQMIInterface21hasFatalErrorOccurredEv
#define USBQMIInterface_getInterface _ZN15USBQMIInterface12getInterfaceEPv

class USBQMIInterface
{
public:
    enum USBState {
        
    };
    typedef void* WCallback;
    //typedef struct USBInterfaceStruct USBInterfaceStruct;
#pragma pack(push, 1)
    struct USBInterfaceStruct
    {
        std::string name0;
        bool primaryOwner4;
        bool enablePS5;
        char pad6[2];
        double recoverTime8;
        unsigned int deviceIdleTimeout10;
        unsigned int IOIdleTimeout14;
        unsigned int disconnectTimeout18;
        std::string clientprop1c;
        void (*getTimestamp20)(unsigned *, unsigned *);
        void (*dumpClientLog24)(const char *);
    };
    struct USBInterfaceStruct6
    {
        std::string name0;
        bool primaryOwner4;
        bool enablePS5;
        char pad6[2];
        double recoverTime8;
        unsigned int deviceIdleTimeout10;
        unsigned int IOIdleTimeout14;
        unsigned int disconnectTimeout18;
        bool resetOnEnterLowPowerFailure1C;
        char pad1D[3];
        std::string clientprop20;
        void (*getTimestamp24)(unsigned *, unsigned *);
        void (*dumpClientLog28)(const char *);
    };
#pragma pack(pop)
};

class QMIClient
{
public:
    class State {
    public:
        enum ClientState{};
        typedef void* Deleter;
    };
};

struct QMIServiceMsg {
    
};

namespace boost {
    template<class _Ty>
    class weak_ptr {
        
    };
}

class ATCSLogger {
    
};

class ATCSCounted {
    
};

class ATCSDPCQueue {
public:
    typedef void* Callback;
};

class ATCSCondition {
    
};

class ATCSMutex {
    
};

class QMIWakeReason {
    
};

class _ATCSException {
    
};

class ATCSUSBIPCDriver {
    
};

class ATCSTextConverter {
public:
    enum Conversion{};
    typedef void* Ucs2ToGsm7Tables;
    class MessageClass {
        
    };
    enum MessageEncoding{};
};

class ATCSFileDescriptorIPCDriver {
    
};

namespace Tlv {
    class Reader {
        
    };
    class Writer {
        
    };
}

class ATCSIPCDriver {
    
};

class ATCSResetInvoker {
public:
    virtual ~ATCSResetInvoker() {}
    virtual void reset(const char* resonString, bool wantCrashLog = true) = 0;
};

class ATCSTimer {
public:
    typedef void* Callback;
};

class ATCSTimerDriver {
    
};

class USBQMIBudder {
    
};


class USBQMIControl {
    // +4 name
    // +8 interface no.
    // +1C state
    // +2C buffer
    // +30 readbuffer[4]
    // +40 writebuffer[4]
};

//typedef unsigned dispatch_group_t; // void*/class?

class QMIClientCallback {
public:
	enum QMIPowerProfile {
		kQMIPower_Normal,
		kQMIPower_Low,
		kQMIPower_MaxValue
	};
	
	virtual ~QMIClientCallback() {}
    
	virtual void registered(QMIClient *) = 0;
	
	virtual void deregistered(QMIClient *) = 0;
	
	virtual void enterPowerMode(QMIClient *client, dispatch_group_t group, QMIPowerProfile targetProfile);
    
	virtual void exitPowerMode(QMIClient *client);
};

typedef unsigned USBQMIStatistics;
typedef struct __CFRunLoopTimer __CFRunLoopTimer;
//typedef void* QMIClientCallback;
typedef void* QCallback;
typedef struct dispatch_group_s dispatch_group_s;
typedef struct QMux QMux;

typedef struct {
    std::string name0;
    bool primaryOwner4;
    bool enablePS5;
    char pad6[2];
    double recoverTime8;
    unsigned int deviceIdleTimeout10;
    unsigned int IOIdleTimeout14;
    unsigned int disconnectTimeout18;
    bool resetOnEnterLowPowerFailure1C;
    char pad1D[3];
    std::string clientprop20;
    void (*getTimestamp24)(unsigned *, unsigned *);
    void (*dumpClientLog28)(const char *);
} __attribute__((packed)) USBInterfaceStruct;


// USBQMIInterface::getInterface(void *)
extern "C" unsigned int (*USBQMIInterface_getInterface)(USBQMIInterface* me, void * a2);

// USBQMIInterface::createUSBController(char  const*)
extern "C" unsigned int (*USBQMIInterface_createUSBController)(USBQMIInterface* me, char  const* name);

// USBQMIInterface::getFatalErrorReason(void)
//extern "C" unsigned int (*USBQMIInterface_getFatalErrorReason)(void);

// USBQMIInterface::hasFatalErrorOccurred(void)
//extern "C" unsigned int (*USBQMIInterface_hasFatalErrorOccured)(void);

// USBQMIInterface::create(USBQMIInterface::USBInterfaceStruct *)
extern "C" unsigned int (*USBQMIInterface_create)(USBInterfaceStruct* a1);

// USBQMIInterface::engage(void)
extern "C" unsigned int (*USBQMIInterface_engage)(USBQMIInterface* me);

// USBQMIInterface::tearDown(void)
//extern "C" unsigned int (*USBQMIInterface_tearDown)(void);

// QMux::QMux(ATCSIPCDriver *,void *,std::string  const&,ATCSResetInvoker *,bool,bool)
extern "C" unsigned int (*QMux_QMux)(QMux* a1, ATCSIPCDriver* a2, void * a3, std::string  const& a4, ATCSResetInvoker* a5, bool a6, bool a7);

// QMIClient::requestClient(std::string  const&,qmi::ServiceType,QMux  const&,QMIClientCallback *,bool,bool)
extern "C" unsigned int (*QMIClient_requestClient)(std::string  const& a2, QMI::Service::Type a3, QMux const& a4, QMIClientCallback* a5, bool a6, bool a7);

// USBQMIInterface::readUSBController(int,unsigned char *,int)
extern "C" unsigned int( *USBQMIInterface_readUSBController)(int interface, unsigned char* data, int size);

// USBQMIInterface::writeUSBController(int,unsigned char *,int)
extern "C" unsigned int (*USBQMIInterface_writeUSBController)(int interface, unsigned char* data, int size);



#ifdef LOAD_IOKIT
//mach_port_t kIOMasterPortDefault
extern "C" unsigned _kIOMasterPortDefault;

//io_service_t IOServiceGetMatchingService(
//                            mach_port_t     masterPort,
//                            CFDictionaryRef matching );
extern "C" unsigned (*_IOServiceGetMatchingService)(int masterPort, int matching);

//CFMutableDictionaryRef IOServiceMatching(
//                  const char *    name );
extern "C" unsigned (*_IOServiceMatching)(const char* name);

//kern_return_t IOServiceOpen(
//              io_service_t    service,
//              task_port_t     owningTask,
//              uint32_t        type,
//              io_connect_t  * connect );
extern "C" unsigned (*_IOServiceOpen)(int service, int owningTask, uint32_t type, int* connect);

//kern_return_t IOServiceClose(
//               io_connect_t    connect );
extern "C" unsigned (*_IOServiceClose)(int connection);

//kern_return_t IOConnectCallScalarMethod(
//                          mach_port_t      connection,            // In
//                          uint32_t         selector,              // In
//                          const uint64_t  *input,                 // In
//                          uint32_t         inputCnt,              // In
//                          uint64_t        *output,                // Out
//                          uint32_t        *outputCnt)             // In/Out
extern "C" unsigned (*_IOConnectCallScalarMethod)(int connection, uint32_t selector, uint64_t* input, uint32_t inputCnt, uint64_t* output, uint32_t* outputCnt);
#endif

namespace QMI {
    
    class LibController: public Controller {
    public:
        LibController();
        virtual ~LibController();
        
        virtual int open();
        virtual int close();
        
        virtual void write(Packet* packet);
        virtual Packet* read();
        
        template <typename Type_>
        static bool dlset(Type_ &function, void* dl, const char* sym) {
            void* value = dlsym(dl, sym);
            if(value == NULL) {
                qmilog("WARN: Unable to find %s\n", sym);
                function = NULL;
                return false;
            } else {
                qmilog("INFO: Found %s at %p\n", sym, value);
                function = reinterpret_cast<Type_>(value);
            }
            return true;
        }
        
/*
#ifdef LOAD_IOKIT
        int loadIOKit();
        int closeIOKit();
        
        
        void ResetBaseband() {
            int connect = 0;
            int match = IOKit_ServiceMatching("AppleBaseband");
            int service = IOKit_ServiceGetMatchingService(0, match);
            IOKit_ServiceOpen(service, mach_task_self(), 0, &connect);
            IOKit_ConnectCallScalarMethod(connect, 0, 0, 0, 0, 0);
            IOKit_ServiceClose(connect);
        }
#endif
 */
        int loadAtCommandStudio();
        int closeAtCommandStudio();
        unsigned int createControllerWithQMux(const char* name, USBQMIControl** outcontrol, QMux** outmux);
        
        static void getTimestamp(unsigned int* stamp1, unsigned int* stamp2) {
            *stamp1 = 1;
            *stamp2 = 2;
        }
        
    protected:
        void* mIOKit;
        void* mATCommandStudio;
        unsigned short mSequence;
        unsigned int mInterface1;
        unsigned int mInterface2;
        unsigned int mInterface3;
        unsigned int mInterface4;
        
        QMux* mQMux1;
        QMux* mQMux2;
        QMux* mQMux3;
        QMux* mQMux4;
        USBQMIControl* mControl1;
        USBQMIControl* mControl2;
        USBQMIControl* mControl3;
        USBQMIControl* mControl4;
        
        USBQMIInterface* mQMIInterface;
        
    };
    
}

#endif /* defined(__libqmi__QMILibController__) */
