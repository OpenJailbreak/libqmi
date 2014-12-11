//
//  util.h
//  usblogger
//
//  Created by Joshua Hill on 1/29/13.
//
//

#ifndef usblogger_util_h
#define usblogger_util_h

#include <sys/types.h>


typedef u_int8_t boolean;
#ifndef TRUE
#define TRUE ((u_int8_t) 1)
#endif
#ifndef FALSE
#define FALSE ((u_int8_t) 0)
#endif

#define DIAG_CONTROL_CHAR 0x7E
#define DIAG_TRAILER_LEN  3

uint16_t flip_endian16(uint16_t value);
uint32_t flip_endian32(uint32_t value);

unsigned int random_int();
unsigned char random_char();
unsigned short random_short();
unsigned int random_string(unsigned char* buffer, unsigned int size);

void hexdump(unsigned char *data, unsigned int amount);
void qmilog(const char* msg, ...);

#endif
