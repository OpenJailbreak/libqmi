//
//  etl.h
//  qmifuzzer
//
//  Created by Joshua Hill on 4/24/13.
//
//

#ifndef __qmifuzzer__etl__
#define __qmifuzzer__etl__

#include <iostream>

u_int16_t etl_crc16(uint8_t* buffer, uint32_t len);
int etl_request(uint8_t* input, uint32_t insize, uint8_t** output, uint32_t* outsize);
int etl_response(uint8_t* input, uint32_t insize, uint8_t** output, uint32_t* outsize);
int etl_escape(uint8_t* input, uint32_t insize, uint8_t** output, uint32_t* outsize);
int etl_unescape(uint8_t* input, uint32_t insize, uint8_t** output, uint32_t* outsize);

#endif /* defined(__qmifuzzer__etl__) */
