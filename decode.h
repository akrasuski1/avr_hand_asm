#ifndef DECODE_H_
#define DECODE_H_

#include "decompression.h"

#include <stdint.h>

#define ARG_EOF 0
#define ARG_REG 1
#define ARG_HEXBYTE 2
#define ARG_HEXWORD 3
#define ARG_HEX3B 4
#define ARG_DECBYTE 5
#define ARG_OFFSET 6
#define ARG_RESERVED 7
#define ARG_MXP 8
#define ARG_YPQ 9

uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs);
void decode(uint16_t op, uint16_t next);
void append_arguments(uint8_t* arguments, uint16_t next);

#endif
