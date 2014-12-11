//
//  diag.h
//  qmifuzzer
//
//  Created by Joshua Hill on 4/24/13.
//
//

#ifndef qmifuzzer_diag_h
#define qmifuzzer_diag_h

int diag_open();
void diag_close(int fd);
int diag_write(int fd, unsigned char* data, unsigned int size);
int diag_read(int fd, unsigned char** data, unsigned int* size);
void diag_strerror(unsigned char response);

void diag_fuzz();

#endif
