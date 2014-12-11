//
//  util.h
//  dloadtool
//
//  Created by Joshua Hill on 1/31/13.
//
//

#ifndef __dloadtool__util__
#define __dloadtool__util__


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>

#include <mach/mach.h>
#include <IOKit/IOKitLib.h>

#include "util.h"

uint16_t flip_endian16(uint16_t value) {
    uint16_t result = 0;
    result |= (value & 0xFF) << 8;
    result |= (value & 0xFF00) >> 8;
    return result;
}

uint32_t flip_endian32(uint32_t value) {
    uint32_t result = 0;
    result |= (value & 0xFF) << 24;
    result |= (value & 0xFF00) << 8;
    result |= (value & 0xFF0000) >> 8;
    result |= (value & 0xFF000000) >> 24;
    return result;
}

unsigned int random_string(unsigned char* buffer, unsigned int size) {
	unsigned int i = 0;
	for (i = 0; i < size; i++) {
		buffer[i] = rand() & 0xFF;
	}
	return size;
}

unsigned int random_int() {
	unsigned int i = 0;
    unsigned int v = 0;
    unsigned char buffer[4];
	for (i = 0; i < 4; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned int*) buffer;
	return v;
}

unsigned short random_short() {
	unsigned int i = 0;
    unsigned short v = 0;
    unsigned char buffer[2];
	for (i = 0; i < 2; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned short*) buffer;
	return v;
}

unsigned char random_char() {
	unsigned int i = 0;
    unsigned char v = 0;
    unsigned char buffer[1];
	for (i = 0; i < 1; i++) {
		buffer[i] = rand() & 0xFF;
	}
    v = *(unsigned char*) buffer;
	return v;
}

void hexdump (unsigned char *data, unsigned int amount) {
    unsigned int    dp, p;  /* data pointer */
    const char      trans[] =
    "................................ !\"#$%&'()*+,-./0123456789"
    ":;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklm"
    "nopqrstuvwxyz{|}~...................................."
    "....................................................."
    "........................................";
    
    
    for (dp = 1; dp <= amount; dp++) {
        qmilog("%02x ", data[dp-1]);
        if ((dp % 8) == 0)
            qmilog(" ");
        if ((dp % 16) == 0) {
            qmilog("| ");
            p = dp;
            for (dp -= 16; dp < p; dp++)
                qmilog("%c", trans[data[dp]]);
            fflush (stderr);
            qmilog("\n");
        }
        fflush (stderr);
    }
    // tail
    if ((amount % 16) != 0) {
        p = dp = 16 - (amount % 16);
        for (dp = p; dp > 0; dp--) {
            qmilog("   ");
            if (((dp % 8) == 0) && (p != 8))
                qmilog(" ");
            fflush (stderr);
        }
        qmilog(" | ");
        for (dp = (amount - (16 - p)); dp < amount; dp++)
            qmilog("%c", trans[data[dp]]);
        fflush (stderr);
    }
    qmilog("\n");
    
    return;
}

void qmilog(const char* fmt, ...) {
    FILE* fd = fopen("/private/var/mobile/Media/qmi.log", "a+");
    if(fd) {
        va_list args;
        va_start(args, fmt);
        vfprintf(fd, fmt, args);
        //vprintf(fmt, args);
        va_end(args);
        //fflush(stdout);
        fflush(fd);
        fclose(fd);
    }
}

#endif /* defined(__dloadtool__util__) */
