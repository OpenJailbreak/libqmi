////////// QMI Fuzzer //////////
//
//  QMIFuzzer.cpp
//  
//  Created by Joshua Hill on 4/15/13.
//  Copyright (c) 2013 PosixNinja LLC. All rights reserved.
//
//////// QMI Architecture Into
//
//   Essentially we need a way to boot the baseband up without using CommCenter so we can send QMI packets without
// any problems. Most of that work has already been done, so I'm not going to try to fix it or make it look pretty.
//
//   First thing that needs to be done is to check and see which mode the baseband is currently in. Based on that we
// can call the approiate handler to boot it to the next stage. Once we finally reach AMSS we can then start our fuzz
// run.
//
//   For data to send there's a few ways we could do it. The more time consuming route would be generation based fuzzing
// where I go through the reference manuals and pick out the known services and commands plus their arguments. The
// problem with this approch is there's many non documented services in the iphone, and in qualcomm protocols as a whole.
// A more simple but probably less effective method would be random or mutational based fuzzing. This might not be too
// effective because the QMI protocol is very strict so even limiting it to the TLV portion of the device would still
// throw some rejections. The best way (IMHO) would be to sniff QMI packets, parse and use the data to create new
// malformed packets to test with. This might take a little longer, and, although the wait would be worth it, I don't
// have the time right now. So I plan on mixing a little bit of the first 2 and see what I come up with.
//
//   Now that I've collected all the code and fixed any compile errors, the main thing lacking from my toolkit is a way
// to monitor the baseband for crashes and grab the crashreports off of it whenever there is a crash. Hopefully this this
// won't be too difficult to achieve.
//
/////// QMI Client Architecture
//
//   QMI packets are rather complex so creating a reuseable library to parse and generate them was more difficult than
// I had imagined at first. After a couple of test projects I settled on C++ as my langauge of choice for this project.
// Due to different devices using different communication protocols I needed to abstract away the difference so the lib
// would work on all devices. iPhone4 uses SPI to communicate with the baseband using a device node on the filesystem.
// iPhone4s (and I'm guessing iPhone5 also) they switched to HSIC, a USB type communication for microchips. For this I
// decided to create a simple abstract QMI::Controller class and two concrete classes named QMI::SPIController and
// QMI::USBController. To decide which controller should be created I needed a root level class to handle the differences
// and choose the best one. I decided to go with QMI::Client as the name of this class, although as you'll see this
// name is a little misleading.
//
//   To start the communication with the baseband you only need to call QMI::Client::create() function. This will decide
// the correct controller for this device and initialize all other things. Once QMI::Client is created the next step is
// to create the correct services you'll need to communicate with during your session. This can be done with a simple
// "client->createService(QMI::Service::TYPE)" call, where TYPE is some QMI service such as DMS, PIM, etc. The call to
// create service will then send a QMI control packet to the device requesting a client ID for that service. The client
// will then take that client ID and associate it with the QMI::Service object before returning it to the caller. After
// this all remaining calls to the requested service should be done through this QMI::Service interface.
//
/////// QMI Service Architecture
//
//   The QMI::Service objects main job is to hold the information relating to the service and client, and accept which
// commands are to be sent to the service it's representing. The service is going to need to take a reference to the
// controller to be able to send packets to the baseband. The data sent to the controller will just be a command plus
// TLV data, either raw or through a concreate QMI::Packet object class. The QMI::Packet sent from QMI::Service to
// QMI::Controller should contain the data for the client ID, service ID, command, and TLV data for the command. Once
// received by the QMI::Controller, the controller will update the sequence number within the packet and figure out
// which QMux the service/client should be associated with then serialize the packet to be sent to the baseband.
//
/////// QMI Packet Architecture
//
//   The QMI::Packet object is a virtual class. By itself it can contain all the data to be sent to the controller for
// serialization, but it can also be overridden to create concreate packet types suck as the NVItem class. Essentially
// the QMI::Packet object is the message object to be sent from the high level QMI::Service class down to the low level
// QMI::Controller classes for transmission to the baseband.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <CoreFoundation/CoreFoundation.h>

#include "bb.h"
#include "usb.h"
#include "dbl.h"
#include "util.h"
#include "diag.h"
#include "dload.h"

#include "TLV.h"
#include "QMIClient.h"
#include "QMIService.h"
#include "NVItem.h"

#include "QMIConsole.h"

#define VENDOR_QUALCOMM  0x05c6
#define PRODUCT_AMSS     0x9001
#define PRODUCT_DLOAD    0x9008
#define PRODUCT_DBL      0x900e

using namespace QMI;

int QMIFuzz() {
    unsigned int loop = 0;
    Client* client = NULL;
    //Service* service = NULL;
    client = Client::create();
    if(client) {
        printf("Client created\n");
        Service* dms = client->createService(Service::DMS);
        Service* nas = client->createService(Service::NAS);
        Service* wms = client->createService(Service::WMS);
        Service* pds = client->createService(Service::PDS);
        Service* pbm = client->createService(Service::PBM);
        Service* wds = client->createService(Service::WDS);
        Service* uim = client->createService(Service::UIM);
        Service* cat = client->createService(Service::CAT2);
        Service* voice = client->createService(Service::VOICE);
        if(dms && nas && wms && pds && pbm && wds && uim && cat && voice) {
            printf("Service created\n");
            Packet* response = NULL;
            Packet* request = new Packet();
            if(request) {
                while(1) {
                    unsigned short cmd = random_short() % 0x100;
                    request->setCommand(cmd);
                    unsigned int num = 0;
                    unsigned int max = 1;//random_int() % 0x10;
                    for(num = 0; num < max; num++) {
                        unsigned char buffer[0x400];
                        unsigned char type = 1;//random_char() % 0x10;
                        unsigned short len = random_short() % 0x8;
                        request->addTLV(TLV::createFromBuffer(type, buffer, len));
                    }
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::DMS, cmd, loop);
                    fflush(stdout);
                    response = dms->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::WMS, cmd, loop);
                    fflush(stdout);
                    response = wms->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::PDS, cmd, loop);
                    fflush(stdout);
                    response = pds->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::PBM, cmd, loop);
                    fflush(stdout);
                    response = pbm->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::WDS, cmd, loop);
                    fflush(stdout);
                    response = wds->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::UIM, cmd, loop);
                    fflush(stdout);
                    response = uim->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::CAT2, cmd, loop);
                    fflush(stdout);
                    response = cat->send(request);
                    if(response) delete response;
                    
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::VOICE, cmd, loop);
                    fflush(stdout);
                    response = voice->send(request);
                    if(response) delete response;
                    
                    //while(cmd == 0x3 || cmd == 0x21 || cmd == 0x38 || cmd == 0x3a || cmd == 0x3c) {
                    //    cmd = random_short() % 0x100;
                    //}
                    //request->setCommand(cmd);
                    printf("\rService: 0x%02x Command: 0x%04x Loop: 0x%08x", Service::NAS, cmd, loop);
                    fflush(stdout);
                    response = nas->send(request);
                    if(response) delete response;
                    
                    request->clearTLV();
                    loop++;
                }
                delete request;
            }
            //service->close();
        }
        
    } // client
    if(client) delete client;
    return 0;
}

#define MODE_FUZZ 0
#define MODE_CONSOLE 1
#define MODE_DIAG 2

int main(int argc, char* argv[]) {
    // Find which mode the device is running in
    int mode = 0;
    int wait = 0;
    UInt8 config = 0;
    USBDevice device;
    USBInterface iface;
    
    int arg = 0;
    for(arg = 0; arg < argc; arg++) {
        if(strcmp(argv[arg], "-s") == 0) {
            arg++;
            srand(atoi(argv[arg]));
        } else 
        if(strcmp(argv[arg], "-c") == 0) {
            mode = MODE_CONSOLE;
        } else
        if(strcmp(argv[arg], "-d") == 0) {
            mode = MODE_DIAG;
        }
    }
    
    printf("Starting QMIFuzzer\n");
    
    bb_reset();
    bb_enter_download_mode();
    
    device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DLOAD);
    if(device) {
        // Found device in DLOAD mode
        printf("Found device in DLOAD mode\n");
        // Set to configuration 1
        printf("Setting configuration to 1\n");
        config = USBSetConfiguration(device, 1);
        if(config == 1) {
            // Open up interface 0
            printf("Openning interface 0 to DLOAD\n");
            iface = USBOpenInterface(device, 0, 0);
            if(iface) {
                dload_boot(iface);
                USBCloseInterface(iface);
                iface = NULL;
            }
        }
        USBCloseDevice(device);
        device = NULL;
        sleep(2);
    }
    
    device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_DBL);
    if(device) {
        // Found device in DBL mode
        printf("Found device in DBL mode\n");
        printf("Setting configuration to 1\n");
        config = USBSetConfiguration(device, 1);
        if(config == 1) {
            // Open up interface 0
            printf("Opening interface 0 to DBL\n");
            iface = USBOpenInterface(device, 0, 0);
            if(iface) {
                dbl_boot(iface);
                USBCloseInterface(iface);
                iface = NULL;
            }
        }
        USBCloseDevice(device);
        device = NULL;
        sleep(2);
        wait = 1;
    }

    
    // Boot the device to AMSS mode
    device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_AMSS);
    while(!device && wait) {
        if(wait > 0 && wait < 30) wait++;
        else break;
        printf("Waiting....\n");
        sleep(1);
        device = USBOpenDevice(VENDOR_QUALCOMM, PRODUCT_AMSS);
    }
    if(device) {
        // Found device in AMSS mode,
        printf("Found device in AMSS mode\n");
        USBCloseDevice(device);
        if(mode == MODE_CONSOLE) {
            QMIConsole(argc, argv);
        } else if(mode == MODE_DIAG) {
            diag_fuzz();
        } else {
            QMIFuzz();
        }
    }
    
	return 0;
}

