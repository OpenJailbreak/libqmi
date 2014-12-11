//
//  main.c
//  dbgcomm
//
//  Created by Joshua Hill on 2/2/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <errno.h>

#include "etl.h"
#include "util.h"
#include "diag.h"

void diag_strerror(unsigned char response) {
    if(response == 0x13) {
        qmilog("Error: Invalid command\n");
    } else if(response == 0x14) {
        qmilog("Error: Invalid parameter\n");
    } else if(response == 0x15) {
        qmilog("Error: Invalid packet length\n");
    } else if(response == 0x16) {
        qmilog("Error: Not accepted by the device\n");
    } else if(response == 0x17) {
        qmilog( "Error: Not allowed in this mode\n");
    } else if(response == 0x42) {
        qmilog("Error: Invalid NV read/write because SP is locked\n");
    } else if(response == 0x47) {
        qmilog("Error: Operation not allowed in this security state\n");
    } else {
        qmilog("Error: Unknown error occurred 0x%hhx\n", response);
    }
}

int diag_open() {
    int speed = 115200;
    struct termios term;
    unsigned char cc[] = { 0x04, 0xff, 0xff, 0x7f, 0x17, 0x15, 0x12, 0xff, 0x03, 0x1c, 0x1a, 0x19, 0x11, 0x13, 0x16, 0x0f, 0x01, 0x00, 0x14, 0xff };
    
    int fd = open("/dev/cu.debug", 0x2);
    if(fd == -1) {
        return -1;
    }
    
    tcgetattr(fd, &term);
    term.c_iflag = 0x1;
    term.c_oflag = 0x0;
    term.c_cflag = 0xcb00;
    term.c_lflag = 0x0;
    term.c_ispeed = 115200;
    term.c_ospeed = 115200;
    memcpy(term.c_cc, cc, sizeof(term.c_cc));
    tcsetattr(fd, 0, &term);
    
    return fd;
}

void diag_close(int fd) {
    close(fd);
}

int diag_read(int fd, unsigned char** data, unsigned int* size) {
    unsigned int outsize = 0;
    unsigned char* output = NULL;
    unsigned char buffer[0x400];
    memset(buffer, '\0', sizeof(buffer));
    int x = read(fd, buffer, sizeof(buffer));
    if(x > 0) {
        //etl_response(buffer, x, &output, &outsize);
        //*data = output;
        //*size = outsize;
        *data = (unsigned char*) malloc(x);
        memcpy(*data, buffer, x);
        *size = x;
        return x;
    }
    
    *size = 0;
    *data = NULL;
    return -1;
}

int diag_write(int fd, unsigned char* data, unsigned int size) {
    unsigned int outsize = 0;
    unsigned char* output = NULL;
    etl_request(data, size, &output, &outsize);
    int x = write(fd, output, outsize);
    if(x <= 0) {
        return -1;
    }
    return x;
}

void diag_fuzz() {
    int fd = diag_open();
    unsigned int len = 0;
    unsigned char buffer[0x200];
    while(1) {
        memset(buffer, '\0', sizeof(buffer));
        len = random_int() % 0x100;
        random_string(buffer, len);
        qmilog("\rCommand: 0x%02hhx, Length: 0x%04x, ", buffer[0], len);
        fflush(stdout);
        int x = diag_write(fd, buffer, len);
        if(x > 0) {
            unsigned int size = 0;
            unsigned char* data = NULL;
            int y = diag_read(fd, &data, &size);
            if(y > 0) {
                free(data);
            }
            
            //qmilog("Returned: 0x%04x", y);
            //if(buffer[0] == data[0]) {
                //qmilog("\n");
                //hexdump(data, size);
            //}
            //fflush(stdout);
        }
    }
    diag_close(fd);
}

int DiagTool (int argc, const char * argv[]) {
    int fd = diag_open();
    if(fd == -1) {
        qmilog("%i(%s)\n", errno, strerror(errno));
        return -1;
    }
    
    if(argc > 1) {
        int i = 0;
        unsigned int size = 0;
        unsigned int byte = 0;
        const char* in = argv[1];
        unsigned char data[0x200];
        size = strlen(in) / 2;
        for(i = 0; i < size; i++) {
            sscanf(in, "%02x", &byte);
            data[i] = byte;
            in += 2;
        }
        
        unsigned int outsize = 0;
        unsigned char* output = NULL;
        etl_request(data, i, &output, &outsize);
        
        unsigned char buf[0x800];
        int x = write(fd, output, outsize);
        if(x != 0) {
            int y = read(fd, buf, 0x800);
            if(y != 0) {
                if(buf[0] == data[0]) {
                    qmilog("Send:\n");
                    hexdump(output, outsize);
                    qmilog("Recv:\n");
                    hexdump(buf, y);
                    qmilog("\n");
                }
                else {
                    diag_strerror(buf[0]);
                    hexdump(buf, y);
                }
            }
        }
        free(output);
        
    } else {
        
        
        unsigned char req1 = 0;
        unsigned char req[] = { 0x0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30 };
                while(req1 < 0xFF) {
                    //req[0] = req1;
                    //req[1] = req2;
                    //req[2] = req3;
                    //qmilog("0x%x 0x%x 0x%x\n", req[0], req[1], req[2]);
                    if(req1 == 0x2a) {
                        req1++;
                        continue;
                    }
                    unsigned int outsize = 0;
                    unsigned char* output = NULL;
                    req[0] = req1;
                    etl_request(req, sizeof(req), &output, &outsize);
    
                    unsigned char buf[0x800];
                    memset(buf, '\0', sizeof(buf));
                    int x = write(fd, output, outsize);
                    if(x != 0) {
                        int y = read(fd, buf, 0x800);
                        if(y != 0) {

                            if(buf[0] == req1) {
                                qmilog("Send:\n");
                                hexdump(output, outsize);
                                qmilog("Recv:\n");
                                hexdump(buf, y);
                                qmilog("\n");
                            }
                            else {
                                qmilog("On request 0x%x\n", req1);
                                diag_strerror(buf[0]);
                            }
                        }
                    }
                    req1++;
                    if(output) free(output);
                }

    }
    
	return 0;
}

