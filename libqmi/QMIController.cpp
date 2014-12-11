//
//  QMIClient.cpp
//  qmiclient
//
//  Created by Joshua Hill on 2/8/13.
//
//

#include <iostream>

#include "QMIController.h"
#include "QMIUSBController.h"
#include "QMISPIController.h"

namespace QMI {
    
Controller* Controller::create() {
    return new USBController();
}

    
}