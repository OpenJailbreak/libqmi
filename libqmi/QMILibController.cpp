//
//  QMILibController.cpp
//  libqmi
//
//  Created by Joshua Hill on 7/10/13.
//
//

extern "C" {
    #include <dlfcn.h>
    #include <mach-o/nlist.h>
}

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>
#include <iostream>

#include "bb.h"
#include "util.h"

#include "QMIPacket.h"
#include "QMIController.h"
#include "QMIControlPacket.h"
#include "QMILibController.h"

#define PACKETS 30

unsigned char bb_packets_w[PACKETS+1][0x1000] = {
    "\x01\x0b\x00\x00\x00\x00\x00\x15\x27\x00\x00\x00", // 1
    "\x01\x0b\x00\x00\x00\x00\x00\x1c\x27\x00\x00\x00", // 2
    "\x01\x14\x00\x00\x00\x00\x00\x1d\x26\x00\x09\x00\x01\x01\x00\x00\x10\x02\x00\x02\x00", // 3
    "\x01\x0f\x00\x00\x00\x00\x00\x01\x22\x00\x04\x00\x01\x01\x00\x02", // 4
    "\x01\x0f\x00\x00\x00\x00\x00\x02\x22\x00\x04\x00\x01\x01\x00\xe2", // 5
    "\x01\x0f\x00\x00\x00\x00\x00\x03\x22\x00\x04\x00\x01\x01\x00\xe1", // 6
    "\x01\x0f\x00\x00\x00\x00\x00\x04\x22\x00\x04\x00\x01\x01\x00\x09", // 7
    "\x01\x0f\x00\x00\x00\x00\x00\x05\x22\x00\x04\x00\x01\x01\x00\x03", // 8
    "\x01\x0f\x00\x00\x00\x00\x00\x06\x22\x00\x04\x00\x01\x01\x00\xe2", // 9
    "\x01\x0f\x00\x00\x00\x00\x00\x07\x22\x00\x04\x00\x01\x01\x00\xe2", // 10
    "\x01\x0f\x00\x00\x00\x00\x00\x08\x22\x00\x04\x00\x01\x01\x00\xe4", // 11
    "\x01\x0f\x00\x00\x00\x00\x00\x09\x22\x00\x04\x00\x01\x01\x00\x03", // 12
    "\x01\x0f\x00\x00\x00\x00\x00\x0a\x22\x00\x04\x00\x01\x01\x00\x09", // 13
    "\x01\x0f\x00\x00\x00\x00\x00\x0b\x22\x00\x04\x00\x01\x01\x00\xe1", // 14
    "\x01\x0f\x00\x00\x00\x00\x00\x0c\x22\x00\x04\x00\x01\x01\x00\xe2", // 15
    "\x01\x0f\x00\x00\x00\x00\x00\x0d\x22\x00\x04\x00\x01\x01\x00\x02", // 16
    "\x01\x0f\x00\x00\x00\x00\x00\x0e\x22\x00\x04\x00\x01\x01\x00\x05", // 17
    "\x01\x0f\x00\x00\x00\x00\x00\x0f\x22\x00\x04\x00\x01\x01\x00\x01", // 18
    "\x01\x0f\x00\x00\x00\x00\x00\x10\x22\x00\x04\x00\x01\x01\x00\x0c", // 19
    "\x01\x0f\x00\x00\x00\x00\x00\x11\x22\x00\x04\x00\x01\x01\x00\x09", // 20
    "\x01\x0f\x00\x00\x00\x00\x00\x12\x22\x00\x04\x00\x01\x01\x00\x0a", // 21
    "\x01\x0f\x00\x00\x00\x00\x00\x13\x22\x00\x04\x00\x01\x01\x00\xe2", // 22
    "\x01\x0f\x00\x00\x00\x00\x00\x14\x22\x00\x04\x00\x01\x01\x00\xe2", // 23
    "\x01\x0f\x00\x00\x00\x00\x00\x16\x22\x00\x04\x00\x01\x01\x00\x0b", // 24
    "\x01\x0f\x00\x00\x00\x00\x00\x17\x22\x00\x04\x00\x01\x01\x00\xe2", // 25
    "\x01\x0f\x00\x00\x00\x00\x00\x18\x22\x00\x04\x00\x01\x01\x00\x02", // 26
    "\x01\x0f\x00\x00\x00\x00\x00\x19\x22\x00\x04\x00\x01\x01\x00\x03", // 27
    "\x01\x0f\x00\x00\x00\x00\x00\x1a\x22\x00\x04\x00\x01\x01\x00\xe2", // 28
    "\x01\x0f\x00\x00\x00\x00\x00\x1b\x22\x00\x04\x00\x01\x01\x00\x02",  // 29
    
    //"\x01\x10\x00\x00\x02\x01\x00\x04\x00\x2E\x00\x04\x00\x01\x01\x00\x05", // Reset Baseband
    
    //"\x01\xD0\x00\x00\xE2\x04\x00\x01\x00\x00\xA0\xC4\x00\x01\xC1\x00\x04\x00\x00\x00\x01\x00\x3C\x3F\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x55\x54\x46\x2D\x38\x22\x3F\x3E\x0A\x3C\x21\x44\x4F\x43\x54\x59\x50\x45\x20\x70\x6C\x69\x73\x74\x20\x50\x55\x42\x4C\x49\x43\x20\x22\x2D\x2F\x2F\x41\x70\x70\x6C\x65\x2F\x2F\x44\x54\x44\x20\x50\x4C\x49\x53\x54\x20\x31\x2E\x30\x2F\x2F\x45\x4E\x22\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x61\x70\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x44\x54\x44\x73\x2F\x50\x72\x6F\x70\x65\x72\x74\x79\x4C\x69\x73\x74\x2D\x31\x2E\x30\x2E\x64\x74\x64\x22\x3E\x0A\x3C\x70\x6C\x69\x73\x74\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x3E\x0A\x3C\x64\x69\x63\x74\x3E\x0A\x3C\x2F\x64\x69\x63\x74\x3E\x0A\x3C\x2F\x70\x6C\x69\x73\x74\x3E", // Activate Baseband
    
    "\x01\x13\x00\x00\x02\x04\x00\x64\x00\x57\x55\x07\x00\x01\x04\x00\x97\x07\x00\x00", // Read MEID
    
    //"\x01\x13\x00\x00\x02\x04\x00\x64\x00\x57\x55\x07\x00\x01\x04\x00\xB2\x00\x00\x00", // Read MDN
    
    //"\x01\x13\x00\x00\x02\x04\x00\x64\x00\x57\x55\x07\x00\x01\x04\x00\x19\x00\x00\x00", // Read AKEY
    
    //"\x01\xD0\x00\x00\xE2\x02\x00\x01\x00\x00\xA0\xC4\x00\x01\xC1\x00\x04\x00\x00\x00\x01\x00\x3C\x3F\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x55\x54\x46\x2D\x38\x22\x3F\x3E\x0A\x3C\x21\x44\x4F\x43\x54\x59\x50\x45\x20\x70\x6C\x69\x73\x74\x20\x50\x55\x42\x4C\x49\x43\x20\x22\x2D\x2F\x2F\x41\x70\x70\x6C\x65\x2F\x2F\x44\x54\x44\x20\x50\x4C\x49\x53\x54\x20\x31\x2E\x30\x2F\x2F\x45\x4E\x22\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x61\x70\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x44\x54\x44\x73\x2F\x50\x72\x6F\x70\x65\x72\x74\x79\x4C\x69\x73\x74\x2D\x31\x2E\x30\x2E\x64\x74\x64\x22\x3E\x0A\x3C\x70\x6C\x69\x73\x74\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x3E\x0A\x3C\x64\x69\x63\x74\x3E\x0A\x3C\x2F\x64\x69\x63\x74\x3E\x0A\x3C\x2F\x70\x6C\x69\x73\x74\x3E", // OTA
    
    //"\x01\x88\x01\x00\xE2\x02\x00\x01\x00\x00\xA0\x7C\x01\x01\x79\x01\x04\x00\x00\x00\x01\x00\x3C\x3F\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x55\x54\x46\x2D\x38\x22\x3F\x3E\x0D\x0A\x3C\x21\x44\x4F\x43\x54\x59\x50\x45\x20\x70\x6C\x69\x73\x74\x20\x50\x55\x42\x4C\x49\x43\x20\x22\x2D\x2F\x2F\x41\x70\x70\x6C\x65\x2F\x2F\x44\x54\x44\x20\x50\x4C\x49\x53\x54\x20\x31\x2E\x30\x2F\x2F\x45\x4E\x22\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x61\x70\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x44\x54\x44\x73\x2F\x50\x72\x6F\x70\x65\x72\x74\x79\x4C\x69\x73\x74\x2D\x31\x2E\x30\x2E\x64\x74\x64\x22\x3E\x0D\x0A\x3C\x70\x6C\x69\x73\x74\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x3E\x0D\x0A\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x3C\x6B\x65\x79\x3E\x4E\x41\x4D\x20\x42\x6C\x6F\x63\x6B\x20\x4E\x6F\x64\x65\x73\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x09\x3C\x6B\x65\x79\x3E\x49\x53\x2D\x36\x38\x33\x20\x43\x44\x4D\x41\x20\x4E\x41\x4D\x20\x42\x6C\x6F\x63\x6B\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x09\x3C\x73\x74\x72\x69\x6E\x67\x3E\x45\x4D\x31\x34\x54\x55\x63\x4A\x6B\x76\x71\x41\x46\x63\x6A\x2F\x50\x77\x3D\x3D\x3C\x2F\x73\x74\x72\x69\x6E\x67\x3E\x0D\x0A\x09\x09\x3C\x6B\x65\x79\x3E\x49\x53\x2D\x36\x38\x33\x20\x4D\x44\x4E\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x09\x3C\x73\x74\x72\x69\x6E\x67\x3E\x65\x67\x49\x43\x51\x34\x41\x46\x3C\x2F\x73\x74\x72\x69\x6E\x67\x3E\x0D\x0A\x09\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x70\x6C\x69\x73\x74\x3E", // Write MDN 1
    
    //"\x01\x3B\x01\x00\xE2\x02\x00\x01\x00\x00\xA0\x2F\x01\x01\x2C\x01\x04\x00\x00\x00\x01\x00\x3C\x3F\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x55\x54\x46\x2D\x38\x22\x3F\x3E\x0D\x0A\x3C\x21\x44\x4F\x43\x54\x59\x50\x45\x20\x70\x6C\x69\x73\x74\x20\x50\x55\x42\x4C\x49\x43\x20\x22\x2D\x2F\x2F\x41\x70\x70\x6C\x65\x2F\x2F\x44\x54\x44\x20\x50\x4C\x49\x53\x54\x20\x31\x2E\x30\x2F\x2F\x45\x4E\x22\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x61\x70\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x44\x54\x44\x73\x2F\x50\x72\x6F\x70\x65\x72\x74\x79\x4C\x69\x73\x74\x2D\x31\x2E\x30\x2E\x64\x74\x64\x22\x3E\x0D\x0A\x3C\x70\x6C\x69\x73\x74\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x3E\x0D\x0A\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x3C\x6B\x65\x79\x3E\x33\x47\x50\x44\x20\x50\x72\x6F\x66\x69\x6C\x65\x20\x30\x20\x4E\x6F\x64\x65\x73\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x09\x3C\x6B\x65\x79\x3E\x4D\x44\x4E\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x09\x3C\x73\x74\x72\x69\x6E\x67\x3E\x39\x38\x37\x36\x35\x34\x33\x32\x31\x30\x3C\x2F\x73\x74\x72\x69\x6E\x67\x3E\x0D\x0A\x09\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x70\x6C\x69\x73\x74\x3E", // Write MDN 2
    
    //"\x01\x3B\x01\x00\xE2\x04\x00\x01\x00\x00\xA0\x2F\x01\x01\x2C\x01\x04\x00\x00\x00\x01\x00\x3C\x3F\x78\x6D\x6C\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x20\x65\x6E\x63\x6F\x64\x69\x6E\x67\x3D\x22\x55\x54\x46\x2D\x38\x22\x3F\x3E\x0D\x0A\x3C\x21\x44\x4F\x43\x54\x59\x50\x45\x20\x70\x6C\x69\x73\x74\x20\x50\x55\x42\x4C\x49\x43\x20\x22\x2D\x2F\x2F\x41\x70\x70\x6C\x65\x2F\x2F\x44\x54\x44\x20\x50\x4C\x49\x53\x54\x20\x31\x2E\x30\x2F\x2F\x45\x4E\x22\x20\x22\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x61\x70\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x44\x54\x44\x73\x2F\x50\x72\x6F\x70\x65\x72\x74\x79\x4C\x69\x73\x74\x2D\x31\x2E\x30\x2E\x64\x74\x64\x22\x3E\x0D\x0A\x3C\x70\x6C\x69\x73\x74\x20\x76\x65\x72\x73\x69\x6F\x6E\x3D\x22\x31\x2E\x30\x22\x3E\x0D\x0A\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x3C\x6B\x65\x79\x3E\x33\x47\x50\x44\x20\x50\x72\x6F\x66\x69\x6C\x65\x20\x30\x20\x4E\x6F\x64\x65\x73\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x3C\x64\x69\x63\x74\x3E\x0D\x0A\x09\x09\x3C\x6B\x65\x79\x3E\x4D\x44\x4E\x3C\x2F\x6B\x65\x79\x3E\x0D\x0A\x09\x09\x3C\x73\x74\x72\x69\x6E\x67\x3E\x39\x38\x37\x36\x35\x34\x33\x32\x31\x30\x3C\x2F\x73\x74\x72\x69\x6E\x67\x3E\x0D\x0A\x09\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x64\x69\x63\x74\x3E\x0D\x0A\x3C\x2F\x70\x6C\x69\x73\x74\x3E", // Write MDN 3
    
    //"\x01\x10\x00\x00\x02\x01\x00\x04\x00\x2E\x00\x04\x00\x01\x01\x00\x05", // Reset Baseband
    
    //"\x01\x13\x00\x00\x02\x04\x00\x64\x00\x57\x55\x07\x00\x01\x04\x00\xB2\x00\x00\x00", // Read MDN
    
    //"\x01\x13\x00\x00\x02\x04\x00\x64\x00\x57\x55\x07\x00\x01\x04\x00\x55\x00\x00\x00",  // Read SPC?
    
    "\x00" // END
    
};

unsigned char bb_packets_r[31][31] = {
    "\x01\x0b\x00\x80\x00\x00\x02\x00\x27\x00\x00\x00", // 1
    "\x01\x12\x00\x80\x00\x00\x01\x1c\x27\x00\x07\x00\x02\x04\x00\x00\x00\x00\x00", // 2
    "\x01\x17\x00\x80\x00\x00\x01\x1d\x26\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x10\x02\x00\x02\x00", // 3
    "\x01\x17\x00\x80\x00\x00\x01\x01\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x02\x01", // 4
    "\x01\x17\x00\x80\x00\x00\x01\x02\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x01", // 5
    "\x01\x17\x00\x80\x00\x00\x01\x03\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe1\x01", // 6
    "\x01\x17\x00\x80\x00\x00\x01\x04\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x09\x01", // 7
    "\x01\x17\x00\x80\x00\x00\x01\x05\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x03\x01", // 8
    "\x01\x17\x00\x80\x00\x00\x01\x06\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x02", // 9
    "\x01\x17\x00\x80\x00\x00\x01\x07\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x03", // 10
    "\x01\x17\x00\x80\x00\x00\x01\x08\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe4\x01", // 11
    "\x01\x17\x00\x80\x00\x00\x01\x09\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x03\x02", // 12
    "\x01\x17\x00\x80\x00\x00\x01\x0a\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x09\x02", // 13
    "\x01\x17\x00\x80\x00\x00\x01\x0b\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe1\x02", // 14
    "\x01\x17\x00\x80\x00\x00\x01\x0c\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x04", // 15
    "\x01\x17\x00\x80\x00\x00\x01\x0d\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x02\x02", // 16
    "\x01\x17\x00\x80\x00\x00\x01\x0e\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x05\x01", // 17
    "\x01\x17\x00\x80\x00\x00\x01\x0f\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x01\x01", // 18
    "\x01\x17\x00\x80\x00\x00\x01\x10\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x0c\x01", // 19
    "\x01\x17\x00\x80\x00\x00\x01\x11\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x09\x03", // 20
    "\x01\x17\x00\x80\x00\x00\x01\x12\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x0a\x01", // 21
    "\x01\x17\x00\x80\x00\x00\x01\x13\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x05", // 22
    "\x01\x17\x00\x80\x00\x00\x01\x14\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x06", // 23
    "\x01\x17\x00\x80\x00\x00\x01\x16\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x0b\x01", // 24
    "\x01\x17\x00\x80\x00\x00\x01\x17\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x07", // 25
    "\x01\x17\x00\x80\x00\x00\x01\x18\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x02\x03", // 26
    "\x01\x17\x00\x80\x00\x00\x01\x19\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x03\x03", // 27
    "\x01\x17\x00\x80\x00\x00\x01\x1a\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\xe2\x08", // 28
    "\x01\x17\x00\x80\x00\x00\x01\x1b\x22\x00\x0c\x00\x02\x04\x00\x00\x00\x00\x00\x01\x02\x00\x02\x04"  // 29
};


#ifdef LOAD_IOKIT
//mach_port_t kIOMasterPortDefault
unsigned _kIOMasterPortDefault;

//io_service_t IOServiceGetMatchingService(
//                            mach_port_t     masterPort,
//                            CFDictionaryRef matching );
unsigned (*_IOServiceGetMatchingService)(int masterPort, int matching);

//CFMutableDictionaryRef IOServiceMatching(
//                  const char *    name );
unsigned (*_IOServiceMatching)(const char* name);

//kern_return_t IOServiceOpen(
//              io_service_t    service,
//              task_port_t     owningTask,
//              uint32_t_t        type,
//              io_connect_t  * connect );
unsigned (*_IOServiceOpen)(int service, int owningTask, uint32_t type, int* connect);

//kern_return_t IOServiceClose(
//               io_connect_t    connect );
unsigned (*_IOServiceClose)(int connection);

//kern_return_t IOConnectCallScalarMethod(
//                          mach_port_t      connection,            // In
//                          uint32_t_t         selector,              // In
//                          const uint64_t  *input,                 // In
//                          uint32_t_t         inputCnt,              // In
//                          uint64_t        *output,                // Out
//                          uint32_t_t        *outputCnt)             // In/Out
unsigned (*_IOConnectCallScalarMethod)(int connection, uint32_t selector, uint64_t* input, uint32_t inputCnt, uint64_t* output, uint32_t* outputCnt);
#endif


// USBQMIInterface::getInterface(void *)
unsigned int (*USBQMIInterface_getInterface)(USBQMIInterface* me, void * a2);

// USBQMIInterface::createUSBController(char  const*)
unsigned int (*USBQMIInterface_createUSBController)(USBQMIInterface* me, char  const* name);

// USBQMIInterface::getFatalErrorReason(void)
//unsigned int (*USBQMIInterface_getFatalErrorReason)(void);

// USBQMIInterface::hasFatalErrorOccurred(void)
//unsigned int (*USBQMIInterface_hasFatalErrorOccurred)(void);

// USBQMIInterface::create(USBQMIInterface::USBInterfaceStruct *)
unsigned int (*USBQMIInterface_create)(USBInterfaceStruct* a1);

// USBQMIInterface::engage(void)
unsigned int (*USBQMIInterface_engage)(USBQMIInterface* me);

// USBQMIInterface::tearDown(void)
//unsigned int (*USBQMIInterface_tearDown)(void);

// QMux::QMux(ATCSIPCDriver *,void *,std::string  const&,ATCSResetInvoker *,bool,bool)
unsigned int (*QMux_QMux)(QMux* me, ATCSIPCDriver* driver, void* cookie, std::string  const& name, ATCSResetInvoker* resetInvoker, bool canFlush, bool logIO);

// QMIClient::requestClient(std::string  const&,qmi::ServiceType,QMux  const&,QMIClientCallback *,bool,bool)
unsigned int (*QMIClient_requestClient)(std::string const& a2, QMI::Service::Type a3, QMux const& a4, QMIClientCallback* a5, bool a6, bool a7);

// USBQMIInterface::readUSBController(int,unsigned char *,int)
unsigned int (*USBQMIInterface_readUSBController)(int a1, unsigned char * a2, int a3);

// USBQMIInterface::writeUSBController(int,unsigned char *,int)
unsigned int (*USBQMIInterface_writeUSBController)(int a1, unsigned char * a2, int a3);


namespace QMI {
    
    LibController::LibController(): mIOKit(0), mSequence(0), mATCommandStudio(0)  {
        
    }
    
    LibController::~LibController() {
        //if(mIOKit) closeIOKit();
        if(mATCommandStudio) closeAtCommandStudio();
    }
    
    int LibController::open() {
        int ret = 0;
        qmilog("INFO: Loading ATCommandStudioDynamic...\n");
        ret = loadAtCommandStudio();
        if(!ret) {
            qmilog("INFO: Loaded ATCommandStudio\n");
            
        } else {
            qmilog("ERROR: Unable to load all functions from %s\n", ATCommandStudioPath);
            return -1;
        }
/*
#ifdef LOAD_IOKIT
        qmilog("Loading IOKit...\n");
        ret = loadIOKit();
        if(ret) {
            qmilog("Loaded IOKit\n");
            
        } else {
            qmilog("Unable to load all functions from %s\n", IOKitPath);
            return -1;
        }
#endif
*/
        
        qmilog("INFO: Resetting baseband\n");
        bb_reset();

        USBInterfaceStruct interfacearg;
        // Create Interface
        interfacearg.name0 = "CommCenter";
        interfacearg.clientprop20 = "/var/wireless/Library/Logs/CoreTelephonyTraceScratch/";
        interfacearg.primaryOwner4 = true;
        interfacearg.enablePS5 = true;
        interfacearg.recoverTime8 = 10.0;
        interfacearg.deviceIdleTimeout10 = 150;
        interfacearg.IOIdleTimeout14 = 50;
        interfacearg.disconnectTimeout18 = 2000;
        interfacearg.getTimestamp24 = &getTimestamp;
        interfacearg.resetOnEnterLowPowerFailure1C = false;
        interfacearg.dumpClientLog28 = NULL;
        mQMIInterface = (USBQMIInterface*)USBQMIInterface_create(&interfacearg);
        if(mQMIInterface == NULL) {
            qmilog("ERROR: Unable to create USBQMIInterface\n");
            return -1;
        }
        qmilog("DEBUG: USBQMIInterface::create(struct:0x%08X):0x%08X\n", &interfacearg, mQMIInterface);
        
        mInterface1 = createControllerWithQMux("qmux1", &mControl1, &mQMux1);
        mInterface1 = createControllerWithQMux("qmux2", &mControl2, &mQMux2);
        mInterface1 = createControllerWithQMux("qmux3", &mControl3, &mQMux3);
        mInterface1 = createControllerWithQMux("qmux4", &mControl4, &mQMux4);
       
        
        // Creating QMIClient (Do we even use this?)
        qmilog("INFO: Creating QMIClient\n");
        QMIClient* mClient = (QMIClient*) QMIClient_requestClient("Data Service", QMI::Service::WDS, *mQMux1, NULL, true, true);
        qmilog("DEBUG: QMIClient::requestClient(name:\"%s\", type:0x%08X, transport:0x%08X, callbackObject:0x%08X, a5:%d, start:%d):0x%08X //0x006354A0\n", "Data Service", QMI::Service::WDS, mQMux1, NULL, true, true, mClient);
  
        // Engaging interface for the first time
        unsigned int engaged = 0;
        qmilog("INFO: Engaging USBQMIInterface\n");
        engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
        qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);

        if(engaged) {
            qmilog("INFO: Interface successfully engaged\n");
        } else {
            qmilog("ERROR: Unable to engage our interface\n");
            return -1;
        }
        
/*
        int x = 0;
        int y = 0;
        int index = 0;
        unsigned char buffer[0x800];
        
        unsigned short c = 0;
        unsigned int size = 0;
        x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
        if(x == 0) {
            while(index < PACKETS) {
                c = *((unsigned short*)&bb_packets_w[index][1]);
                size = *((unsigned int*) &c);
                size++;
                y = USBQMIInterface_writeUSBController(mInterface1, bb_packets_w[index], size);
                qmilog(">>> CCLite [%d] USBQMIInterface::writeUSBController(%d, %p, %d)\n", index, mInterface1, bb_packets_w[index], size);
                hexdump(bb_packets_w[index], size);
                qmilog("\n");
                long long looped = 0;
                while(1) {
                    x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
                    looped++;
                    if(x == -1 && index == 0) {
                        // Only engage on first reset?
                        engaged = ((unsigned(*)(USBQMIInterface*))USBQMIInterface_engage)((USBQMIInterface*)mQMIInterface);
                        qmilog("CCLite> USBQMIInterface::engage(this:%p):0x%08X\n", mQMIInterface, engaged);
                        qmilog("engage on %lld retry\n", looped);
                    } else if(x == 0) {
                        //qmilog("DEBUG: USBQMIInterface::readUSBController return 0, continue\n");
                        //continue;
                    } else if(x > 0 && x != -1) {
                        qmilog("data arrived after %lld retry\n", looped);
                        qmilog(">>> CCLite [%d] USBQMIInterface::readUSBController(%d, %p, %d)\n", index, mInterface1, buffer, x);
                        hexdump(buffer, x);
                        qmilog("\n");
                        index++;
                        break;
                    }
                }
                //if (index == 0 && (looped  + 1) % 7000000 == 0) {
                    // Reset
                //    qmilog("we ResetBaseband after %lld failed\n", looped);
                //    bb_reset();
                //}
            }
            qmilog("%d handshake send done.\n", PACKETS);
        } else {
            qmilog("Got a response on our first read packet?!?!?!?\n");
            return false;
        }
 
 
        */
        
        int x = 0;
        int p = 0;
        unsigned char buffer[0x800];
        
        unsigned char* data = NULL;
        Packet::Header* header = NULL;
        ControlPacket* request = new ControlPacket();
        while(p < 2 ) {
            memset(buffer, '\0', sizeof(buffer));
            x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
            if(x == 0) {
                continue;
                
            } else if(x == -1) {
                qmilog("INFO: Engaging USBQMIInterface\n");
                engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
                qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);
                
            } else if(x > 0 && x != -1) {
                qmilog("Received data %x\n", x);
                hexdump(buffer, x);
                mSequence++;
                p++;
                
                x = 0;
                data = NULL;
                memset(buffer, '\0', sizeof(buffer));
                if(p == 0) {
                    request = new ControlPacket();
                    request->setClient(0);
                    request->setCommand(0x27);
                    request->setSequence(mSequence);
                    request->setService(Service::CTL);
                    data = request->serialize();
                    header = (QMI::Packet::Header*)data;
                    hexdump(data, header->size+1);
                    x = USBQMIInterface_writeUSBController(mInterface1, data, header->size+1);
                    if(x == 0) {
                        qmilog("Write successfully\n");
                        delete request;
                        continue;
                    }
                    delete request;
                    continue;
                } else if(p == 1) {
                    request = new ControlPacket();
                    request->setCommand(0x26);
                    request->setSequence(mSequence);
                    request->setService(Service::CTL);
                    request->addTLV(TLV::createFromBuffer(0x01, (unsigned char*) "\x00", 0x1));
                    request->addTLV(TLV::createFromBuffer(0x10, (unsigned char*) "\x02\x00", 0x2));
                    data = request->serialize();
                    header = (QMI::Packet::Header*)data;
                    hexdump(data, header->size+1);
                    x = USBQMIInterface_writeUSBController(mInterface1, data, header->size+1);
                    if(x == 0) {
                        qmilog("Write successfully\n");
                        delete request;
                        continue;
                    }
                    delete request;
                    continue;
                }
            }
        }
/*
        unsigned int x, y = 0;
        x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
        if(x > 0) {
            qmilog("WTF!! Got data on first packet?!\n");
        }

        qmilog("Creating first control packet\n");
        int i = 0;
        ControlPacket* response = NULL;
        
        
        
        qmilog("Writing first control packet\n");
        this->write(request);
        hexdump(request->serialize(), request->)
        while(x <= 0) {
            p++;
            memset(buffer, '\0', sizeof(buffer));
            x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
            if(x == 0) {
                continue;
                
            } else if(x > 0) {
                qmilog("Baseband responded!!! %x\n", x);
                hexdump(buffer, x);
                break;
                
            } else if(x == -1) {
                qmilog("INFO: Engaging USBQMIInterface\n");
                engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
                this->write(request);
                qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);
            }

        }
        
        
        qmilog("Creating second control packet\n");
        request->setCommand(0x26);
        request->addTLV(TLV::createFromBuffer(0x01, (unsigned char*) "\x00", 0x1));
        request->addTLV(TLV::createFromBuffer(0x10, (unsigned char*) "\x02\x00", 0x2));
        
        x = 0;
        p = 0;
        qmilog("Writing second control packet\n");
        this->write(request);
        while(x <= 0) {
            p++;
            memset(buffer, '\0', sizeof(buffer));
            x = USBQMIInterface_readUSBController(mInterface1, buffer, sizeof(buffer));
            if(x == 0) {
                continue;
                
            } else if(x > 0) {
                qmilog("Baseband responded!!!\n");
                hexdump(buffer, x);
                break;
                
            } else if(x == -1) {
                qmilog("INFO: Engaging USBQMIInterface\n");
                engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
                this->write(request);
                qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);
            }
            
        }
         */
        return 0;
    }
    
    int LibController::loadAtCommandStudio() {
        mATCommandStudio = dlopen(ATCommandStudioPath, RTLD_NOW);
        if(mATCommandStudio != NULL) {
            dlset(QMux_QMux, mATCommandStudio, "_ZN4QMuxC1EP13ATCSIPCDriverPvRKSsP16ATCSResetInvokerbb");
            dlset(USBQMIInterface_engage, mATCommandStudio, "_ZN15USBQMIInterface6engageEv");
            dlset(USBQMIInterface_getInterface, mATCommandStudio, "_ZN15USBQMIInterface12getInterfaceEPv");
            dlset(USBQMIInterface_create, mATCommandStudio, "_ZN15USBQMIInterface6createEPNS_18USBInterfaceStructE");
            dlset(USBQMIInterface_readUSBController, mATCommandStudio, "_ZN15USBQMIInterface17readUSBControllerEiPhi");
            dlset(USBQMIInterface_writeUSBController, mATCommandStudio, "_ZN15USBQMIInterface18writeUSBControllerEiPhi");
            dlset(USBQMIInterface_createUSBController, mATCommandStudio, "_ZN15USBQMIInterface19createUSBControllerEPKc");
            //dlset(USBQMIInterface_getFatalErrorReason, mATCommandStudio, "_ZN15USBQMIInterface19getFatalErrorReasonEv");
            //dlset(USBQMIInterface_hasFatalErrorOccured, mATCommandStudio, "_ZN15USBQMIInterface21hasFatalErrorOccurredEv");
            dlset(QMIClient_requestClient, mATCommandStudio, "_ZN9QMIClient13requestClientERKSsN3qmi11ServiceTypeERK4QMuxP17QMIClientCallbackbb");
            
        } else return -1;
        
        return 0;
    }
    
    int LibController::closeAtCommandStudio() {
        if(mATCommandStudio != NULL) {
            dlclose(mATCommandStudio);
            mATCommandStudio = NULL;
        }
        return 0;
    }


/*
#ifdef LOAD_IOKIT
    int LibController::loadIOKit() {
        mIOKit = dlopen(IOKitPath, RTLD_NOW);
        if(mIOKit != NULL) {
            dlset(IOKit_ServiceGetMatchingService, mIOKit, "IOServiceGetMatchingService");
            dlset(IOKit_ServiceMatching, mIOKit, "IOServiceMatching");
            dlset(IOKit_ServiceOpen, mIOKit, "IOServiceOpen");
            dlset(IOKit_ServiceClose, mIOKit, "IOServiceClose");
            dlset(IOKit_ConnectCallScalarMethod, mIOKit, "IOConnectCallScalarMethod");
            
        } else return -1;
        return 0;
    }

    int LibController::closeIOKit() {
        if(mIOKit) {
            dlclose(mIOKit);
            mIOKit = NULL;
        }
        return 0;
    }
#endif
*/
    
    int LibController::close() {
        closeAtCommandStudio();
/*
#ifdef LOAD_IOKIT
        closeIOKit();
#endif
 */
        
        return 0;
    }
    
    unsigned int LibController::createControllerWithQMux(const char* name, USBQMIControl** outcontrol, QMux** outmux) {
        USBQMIControl* control = (USBQMIControl*)USBQMIInterface_createUSBController((USBQMIInterface*)mQMIInterface, name);
        qmilog("DEBUG: USBQMIInterface::createUSBController(this:0x%08X, name:\"%s\"):0x%08X\n", mQMIInterface, name, control);
        if (control == NULL) {
            //unsigned char fFatalError = (unsigned char)USBQMIInterface_hasFatalErrorOccured();
            //qmilog("DEBUG: USBQMIInterface::fFatalError:%d\n", fFatalError);
            //if (fFatalError != 0) {
                //std::string fFatalErrorReason = ((std::string(*)())USBQMIInterface_getFatalErrorReason)();
                //qmilog("DEBUG: USBQMIInterface::fFatalErrorReason:\"%s\"\n", fFatalErrorReason.c_str());
            //}
            
        }
        unsigned interface = USBQMIInterface_getInterface((USBQMIInterface*)mQMIInterface, control); // qmux1~3 3,5,7,9, loc 11
        // simple take out member+8 in USBQMIControl
        qmilog("DEBUG: USBQMIInterface::getInterface(this:0x%08X, control:0x%08X):0x%08X\n", mQMIInterface, control, interface);
        QMux* newmem8 = (QMux*)new char[8]; // delete in release control
        QMux* qmux = (QMux*)QMux_QMux(newmem8, NULL, NULL, name, NULL, true, false);
        qmilog("DEBUG: QMux::QMux(this:0x%08X, driver:0x%08X, cookie:0x%08X, name:\"%s\", resetInvoker:0x%08X, canFlush:%d, logIO:%d):0x%08X //0x00635468\n", newmem8, NULL, NULL, name, NULL, true, false, qmux);
        *outcontrol = control;
        *outmux = qmux;
        return interface;
    }

    
    
    void LibController::write(Packet* packet) {
        packet->setSequence(mSequence);
        unsigned char* data = packet->serialize();
        Packet::Header* header = (Packet::Header*) data;
        if(packet->getService() == Service::CTL) {
            ControlPacket::Header* ctlpkt = (ControlPacket::Header*) data;
            ctlpkt->sequence = (unsigned char) mSequence;
        } else {
            header->sequence = (unsigned short) mSequence;
        }
        
        //qmilog("Writing USB packet\n");
        int x = USBQMIInterface_writeUSBController(mInterface1, packet->serialize(), header->size+1);
        //qmilog("Wrote %d btyes\n", x);
        if(x == 0) {
            mSequence++;
            
        } else if(x == -1) {
            unsigned int engaged = 0;
            qmilog("INFO: Engaging USBQMIInterface\n");
            engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
            qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);
            
            if(engaged) {
                qmilog("INFO: Interface successfully engaged\n");
            } else {
                qmilog("ERROR: Unable to engage our interface\n");
            }
        }
        if(data) delete data;
    }
    
    Packet* LibController::read() {
        /*
        // Engaging interface for the first time

        */
        uint32_t size = 0x2000;
        Packet* packet = NULL;
        unsigned char* data = new unsigned char[size];
        qmilog("Reading USB packet\n");
        int y = USBQMIInterface_readUSBController(mInterface1, data, sizeof(data)-1);
        if(y > 0) {
            qmilog("Read %d bytes\n", y);
            Packet::Header* header = (Packet::Header*) data;
            if(header->service == Service::CTL) {
                packet = new ControlPacket(data);
            } else {
                packet = new Packet(data);
            }
        } else if(y == -1) {
            unsigned int engaged = 0;
            qmilog("INFO: Engaging USBQMIInterface\n");
            engaged = (unsigned int) USBQMIInterface_engage((USBQMIInterface*)mQMIInterface);
            qmilog("DEBUG: USBQMIInterface::engage(this:0x%08X):0x%08X\n", mQMIInterface, engaged);
            
            if(engaged) {
                qmilog("INFO: Interface successfully engaged\n");
            } else {
                qmilog("ERROR: Unable to engage our interface\n");
            }
        }
        if(data) delete data;
        return packet;
    }
}
