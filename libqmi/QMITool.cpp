//
//  main.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>

#include "TLV.h"
#include "NVItem.h"
#include "QMIClient.h"
#include "QMIService.h"

using namespace QMI;

int QMITool(int argc, char* argv[]) {
    // When we create our client, like program figures out which device it is running on and returns
    //  The proper controller interface for that device. the controller interface creates the qmux object
    //  And  returns it to the client.
    Client* client = Client::create();
    
    // readNVItem takes a nv items short and returns a type NVItem. An NVItem is a concreate type of the
    //  QMI::Packet. The concreate object should pass all type, length, and values. It should create a new service class if one is needed. the client will then pass the QMI::Packet into the QMI::Controller interface. either Client or Controller will set the qmux class pointer. It will then call the packet's serialize function to then it into data.
    NVItem* meid = client->readNVItem(NVItem::MEID);
    
    // this can be overwritten by NVItem
    //meid->setValue("0987654321");
    
    // writeNVItem take an NVItem and writes it back to memory. The client will create the tlv objects and associate the proper service with it. The client then writes the packet to the device interface. the device interface will set and client of qmux questions before serializing the packet and sending it off
    client->writeNVItem(meid);
    
    // Service needs to have client passed into it so the service can access the qmux for the packet???
    Service* service = client->createService(Service::DMS);
    
    // The service write function takes a Packet type, adds service info and passes it to the client. The client can then add the Service info to the Packet before passing the Packet to the. controller. The controller then adds any qmux info before passing it to the device
    //service->write(new NVItem(NVItem::MEID));
    
	return 0;
}

