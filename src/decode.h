#ifndef DECODE_H_
#define DECODE_H_

#include "decompression.h"

#include <stdint.h>

typedef struct compressed_ops_names {
	bit_state compressed_op_bs, compressed_op_names;
} compressed_ops_names;

void init_ops_names(compressed_ops_names* bs);
uint16_t get_op_mask(bit_state* bs, uint16_t mask);
uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs);
void decode(uint16_t op, uint16_t next);
void append_arguments(uint8_t* arguments, uint16_t next);

#endif
