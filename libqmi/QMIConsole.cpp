//
//  QMIConsole.cpp
//  qmifuzzer
//
//  Created by Joshua Hill on 4/21/13.
//
//

#include "QMIClient.h"
#include "QMIService.h"
#include "QMIConsole.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "util.h"

using namespace QMI;

/*

 RMS     = 0xE1,
 OMA     = 0xE2
 } Type;*/

unsigned char getServiceFromName(const char* service) {
    if(strcmp(service, "CTL") == 0 || strcmp(service, "ctl") == 0) {
        return Service::CTL;
    } else if(strcmp(service, "WDS") == 0 || strcmp(service, "wds") == 0) {
        return Service::WDS;
    } else if(strcmp(service, "DMS") == 0 || strcmp(service, "dms") == 0) {
        return Service::DMS;
    } else if(strcmp(service, "NAS") == 0 || strcmp(service, "nas") == 0) {
        return Service::NAS;
    } else if(strcmp(service, "QOS") == 0 || strcmp(service, "qos") == 0) {
        return Service::QOS;
    } else if(strcmp(service, "WMS") == 0 || strcmp(service, "wms") == 0) {
        return Service::WMS;
    } else if(strcmp(service, "PDS") == 0 || strcmp(service, "pds") == 0) {
        return Service::PDS;
    } else if(strcmp(service, "AUTH") == 0 || strcmp(service, "auth") == 0) {
        return Service::AUTH;
    } else if(strcmp(service, "AT") == 0 || strcmp(service, "at") == 0) {
        return Service::AT;
    } else if(strcmp(service, "VOICE") == 0 || strcmp(service, "voice") == 0) {
        return Service::VOICE;
    } else if(strcmp(service, "CAT2") == 0 || strcmp(service, "cat2") == 0) {
        return Service::CAT2;
    } else if(strcmp(service, "UIM") == 0 || strcmp(service, "uim") == 0) {
        return Service::UIM;
    } else if(strcmp(service, "PBM") == 0 || strcmp(service, "pbm") == 0) {
        return Service::PBM;
    } else if(strcmp(service, "RMTFS") == 0 || strcmp(service, "rmtfs") == 0) {
        return Service::RMTFS;
    } else if(strcmp(service, "LOC") == 0 || strcmp(service, "loc") == 0) {
        return Service::LOC;
    } else if(strcmp(service, "SAR") == 0 || strcmp(service, "sar") == 0) {
        return Service::SAR;
    } else if(strcmp(service, "CSD") == 0 || strcmp(service, "csd") == 0) {
        return Service::CSD;
    } else if(strcmp(service, "EFS") == 0 || strcmp(service, "efs") == 0) {
        return Service::EFS;
    } else if(strcmp(service, "TS") == 0 || strcmp(service, "ts") == 0) {
        return Service::TS;
    } else if(strcmp(service, "TMD") == 0 || strcmp(service, "tmd") == 0) {
        return Service::TMD;
    } else if(strcmp(service, "CAT") == 0 || strcmp(service, "cat") == 0) {
        return Service::CAT;
    } else if(strcmp(service, "RMS") == 0 || strcmp(service, "rms") == 0) {
        return Service::RMS;
    } else if(strcmp(service, "OMA") == 0 || strcmp(service, "oma") == 0) {
        return Service::OMA;
    }
    return -1;
}

void getNameFromService(unsigned char service) {
    switch(service) {
        case Service::CTL:
            printf("CTL");
            break;
            
        case Service::WDS:
            printf("WDS");
            break;
            
        case Service::DMS:
            printf("DMS");
            break;
            
        case Service::NAS:
            printf("NAS");
            break;
            
        case Service::QOS:
            printf("QOS");
            break;
            
        case Service::WMS:
            printf("WMS");
            break;
            
        case Service::PDS:
            printf("PDS");
            break;
            
        case Service::AUTH:
            printf("AUTH");
            break;
            
        case Service::AT:
            printf("AT");
            break;
            
        case Service::VOICE:
            printf("VOICE");
            break;
            
        case Service::CAT2:
            printf("CAT2");
            break;
            
        case Service::UIM:
            printf("UIM");
            break;
            
        case Service::PBM:
            printf("PBM");
            break;
            
        case Service::RMTFS:
            printf("RMTFS");
            break;
            
        case Service::LOC:
            printf("LOC");
            break;
            
        case Service::SAR:
            printf("SAR");
            break;
            
        case Service::CSD:
            printf("CSD");
            break;
            
        case Service::EFS:
            printf("EFS");
            break;
            
        case Service::TS:
            printf("TS");
            break;
            
        case Service::TMD:
            printf("TMD");
            break;
            
        case Service::CAT:
            printf("CAT");
            break;
            
        case Service::RMS:
            printf("RMS");
            break;
            
        case Service::OMA:
            printf("OMA");
            break;
            
        default:
            break;
    }
}

void lineToArgs(char* line, char*** argv, int* argc) {
    int count = 0;
    char* input = strdup(line);
    char** args = (char**)malloc(sizeof(char*) * 0x100);
    
    char* next = input;
    char* prev = input;
    while(count < 0x100) {
        next = strchr((const char*) next, ' ');
        if(next) {
            next[0] = '\0';
            next++;
            args[count] = prev;
            prev = next;
            
            //printf("%s\n", args[count]);
        } else {
            next = prev;
            next = strchr((const char*) next, '\n');
            if(next) {
                next[0] = '\0';
                args[count] = prev;
                
                //printf("%s\n", args[count]);
                count++;
                break;
            }
        }
        count++;
    }
    
    //int i = 0;
    //for(i = 0; i < count; i++) {
    //    printf("%s\n", args[i]);
    //}
    *argv = args;
    *argc = count;
}

//unsigned short atouh(char* word)

int QMIConsole(int argc, char* argv[]) {
    char input[0x200];
    unsigned char svc = -1;
    Service* service = NULL;
    Client* client = Client::create();
    
    if(client) {
        while(1) {
            if(service != NULL) {
                getNameFromService(svc);
            }
            printf("> ");
            memset(input, '\0', sizeof(input));
            fgets(input, sizeof(input), stdin);
            
            int count = 0;
            char** args = NULL;
            lineToArgs(input, &args, &count);
            
            if(strcmp(args[0], "help") == 0) {
                printf("  service <svc>                     switch to specified service\n");
                printf("  command <cmd> <type> <value>      send command with TLV\n");
                printf("  fuzz <cmd>                        fuzz a specific command\n");
                printf("  nvread <item>                     read value from nvitem\n");
                printf("  nvwrite <item> <value>            write value to nvitem\n");
                printf("  exit                              exit from this console\n");
            } else
            
            if(strcmp(args[0], "exit") == 0) {
                break;
            } else
            
            if(strcmp(args[0], "service") == 0 || strcmp(args[0], "svc") == 0) {
                char* name = args[1];
                svc = getServiceFromName(name);
                if(svc != -1) {
                    if(service != NULL) {
                        service->close();
                        service = NULL;
                    }
                    service = client->createService(svc);
                }
            } else
                
            if(strcmp(args[0], "command") == 0 || strcmp(args[0], "cmd") == 0) {
                if(service == NULL) {
                    printf("You must select a service first\n");
                } else {
                    if(strcmp(args[1], "find") == 0) {
                        int i = 0;
                        Packet* request = new Packet;
                        for(i = 0; i < 0xFFFF; i++) {
                            request->setCommand((unsigned short)i);
                            Packet* response = service->send(request);
                            if(response) {
                                TLV* result = response->getTLV(0x2);
                                if(result) {
                                    unsigned int value = *(unsigned int*) result->getValue();
                                    //01 00 47 00
                                    if(value != 0x470001) {
                                        printf("Command 0x%x returned 0x%x\n", i, value);
                                    }
                                }
                                delete response;
                            }
                        }
                        delete request;
                    } else {
                        unsigned short cmd = (unsigned short) strtoul(args[1], NULL, 0);
                        printf("Sending command 0x%hx\n", cmd);
                        Packet* request = new Packet();
                        request->setCommand(cmd);
                        int arg = 2;
                        while(arg < count) {
                            unsigned char type = (unsigned char) strtoul(args[arg++], NULL, 0);
                        
                            int i = 0;
                            unsigned int sz = 0;
                            unsigned int byte = 0;
                            const char* in = args[arg++];
                            unsigned char data[0x2000];
                            sz = strlen(in) / 2;
                            for(i = 0; i < sz; i++) {
                                sscanf(in, "%02x", &byte);
                                data[i] = byte;
                                in += 2;
                            }
                            request->addTLV(TLV::createFromBuffer(type, data, sz));
                        }
                    
                        Packet* response = service->send(request);
                        unsigned char* resp = response->serialize();
                        Packet::Header* header = (Packet::Header*) resp;
                        hexdump(resp, header->size+1);
                        delete response;
                        delete request;
                        delete resp;
                    }
                }
            } else
                
            if(strcmp(args[0], "fuzz") == 0) {
                if(service == NULL) {
                    printf("You must select a service first\n");
                } else {
                    
                    int i = 0;
                    char buffer[0x200];
                    unsigned char types[0x30];
                    unsigned short lengths[0x30];
                    memset(buffer, '\0', sizeof(buffer));
                    unsigned short cmd = (unsigned short) strtoul(args[1], NULL, 0);
                    printf("Preparing to fuzz command 0x%hx for service 0x%hhx\n", cmd, svc);
                    
                    printf("Mandatory TLVs\n");
                    while(strcmp(buffer, "\n") != 0) {
                        printf("Type: ");
                        memset(buffer, '\0', sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);
                        if(strcmp(buffer, "\n") == 0) break;
                        types[i] = (unsigned char)strtoul(buffer, NULL, 0);
                        printf("Length: ");
                        memset(buffer, '\0', sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);
                        if(strcmp(buffer, "\n") == 0) break;
                        lengths[i] = (unsigned short) strtoul(buffer, NULL, 0);
                        i++;
                    }
                    
                    memset(buffer, '\0', sizeof(buffer));
                    printf("Optional TLVs\n");
                    while(strcmp(buffer, "\n") != 0) {
                        printf("Type: ");
                        memset(buffer, '\0', sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);
                        if(strcmp(buffer, "\n") == 0) break;
                        types[i] = (unsigned char)strtoul(buffer, NULL, 0);
                        printf("Length: ");
                        memset(buffer, '\0', sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);
                        if(strcmp(buffer, "\n") == 0) break;
                        lengths[i] = (unsigned short) strtoul(buffer, NULL, 0);
                        i++;
                    }
                    
                    Packet* request = new Packet;
                    request->setCommand(cmd);
                    printf("Fuzzing command 0x%hx with %d TLVs of type and length...\n", cmd, i);
                    unsigned char buf[0x200];
                    unsigned int loop = 0;
                    memset(buffer, '\0', sizeof(buffer));
                    while(1) {
                        int j = 0;
                        for(j = 0; j < i; j++) {
                            memset(buffer, '\0', sizeof(buffer));
                            random_string((unsigned char*) buf, (unsigned int)lengths[j]);
                            TLV* tlv = TLV::createFromBuffer(types[j], (unsigned char*) buf, lengths[j]);
                            request->addTLV(tlv);
                        }
                        printf("\rService: 0x%hhx, Command: 0x%hx, Loop: 0x%08x", svc, cmd, loop);
                        fflush(stdout);
                        Packet* response = service->send(request);
                        request->clearTLV();
                        delete response;
                        loop++;
                    }
                    delete request;
                }
            } else
            
            if(strcmp(args[0], "nvread") == 0) {
                if(svc != Service::DMS) {
                    printf("You must be using DMS service to use this command\n");
                } else {
                    if(strcmp(args[1], "all") == 0) {
                        unsigned int i = 0;
                        Packet* request = new Packet;
                        request->setCommand(0x5557);
                        for(i = 0xFFFF; i < 0xFFFFFFFF; i++) {
                            request->clearTLV();
                            unsigned int item = i;
                            request->addTLV(TLV::createFromInteger(0x01, item));
                            Packet* response = service->send(request);
                            if(response) {
                                TLV* result = response->getTLV(0x2);
                                if(result) {
                                    unsigned int value = *(unsigned int*) result->getValue();
                                    if(value == 0) {
                                        //unsigned char* resp = response->serialize();
                                        //Packet::Header* header = (Packet::Header*) resp;
                                        //hexdump(resp, header->size+1);
                                        //delete resp;
                                        printf("NVItem 0x%x (%d) returned QMI_ERR_SUCCESS\n", item, item);
                                        
                                    } else if(value == 0x30001) {
                                        //printf("NVItem 0x%x (%d) returned QMI_ERR_INVALID_ARG\n", item, item);
                                        
                                    } else if(value == 0x520001) {
                                        printf("NVItem 0x%x (%d) returned QMI_ERR_ACCESS_DENIED\n", item, item);
                                        
                                    } else {
                                        printf("Unable to get item returned 0x%x\n", value);
                                    }
                                }
                                delete response;
                            }
                        }
                        
                    } else {
                        unsigned int item = strtoul(args[1], NULL, 0);
                        Packet* request = new Packet;
                        request->setCommand(0x5557);
                        request->addTLV(TLV::createFromInteger(0x01, item));
                        Packet* response = service->send(request);
                        if(response) {
                            TLV* result = response->getTLV(0x2);
                            if(result) {
                                unsigned int value = *(unsigned int*) result->getValue();
                                if(value == 0) {
                                    unsigned char* resp = response->serialize();
                                    Packet::Header* header = (Packet::Header*) resp;
                                    hexdump(resp, header->size+1);
                                    delete resp;
                                } else {
                                    printf("Unable to get item returned 0x%x\n", value);
                                }
                            }
                            delete response;
                        }

                        delete request;
                    }
                }
            } else
                
            if(strcmp(args[0], "nvwrite") == 0) {
                if(svc != Service::DMS) {
                    printf("You must select DMS service first\n");
                } else {
                    unsigned char buf[0x84];
                    memset(buf, '\0', sizeof(buf));
                    unsigned int item = strtoul(args[1], NULL, 0);
                    *(unsigned int*) &buf[0] = item;
                    Packet* request = new Packet;
                    request->setCommand(0x5556);
                    
                    int i = 0;
                    unsigned int sz = 0;
                    unsigned int byte = 0;
                    const char* in = args[2];
                    unsigned char* data = &buf[4];
                    sz = strlen(in) / 2;
                    for(i = 0; i < sz; i++) {
                        sscanf(in, "%02x", &byte);
                        data[i] = byte;
                        in += 2;
                    }
                    
                    request->addTLV(TLV::createFromBuffer(0x1, (unsigned char*)buf, sizeof(buf)));
                    unsigned char* req = request->serialize();
                    Packet::Header* hdr = (Packet::Header*) req;
                    hexdump(req, hdr->size+1);
                    delete req;
                    Packet* response = service->send(request);
                    unsigned char* resp = response->serialize();
                    Packet::Header* header = (Packet::Header*) resp;
                    hexdump(resp, header->size+1);
                    delete resp;
                    delete response;
                    delete request;
                    
                }
            }
            
            
        }
    }
    
    if(service) service->close();
    if(client) delete client;
    return 0;
}