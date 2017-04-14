#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H

#include <stdint.h>
#include "buffer_utils.h"
#include "common.h"

typedef struct bit_state {
	uint8_t* ptr;
	uint8_t curbit;
} bit_state;

uint8_t get_bit(bit_state* bs);
uint8_t get_bits(uint8_t cnt, bit_state* bs);
uint8_t next_string(uint8_t* op_type, bit_state* bs);
void load_string(uint8_t id);

#endif // DECOMPRESSION_H
