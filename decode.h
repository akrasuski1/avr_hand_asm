#ifndef DECODE_H_
#define DECODE_H_

#include "decompression.h"

#include <stdint.h>

uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs);
void decode(uint16_t op, uint16_t next);
void append_arguments(uint8_t* arguments, uint16_t next);

#endif
