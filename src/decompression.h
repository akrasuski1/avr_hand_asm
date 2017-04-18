#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H

#include <stdint.h>
#include "buffer_utils.h"
#include "common.h"
#include "progmem_utils.h"

typedef struct bit_state {
	const uint8_t* PROGMEM ptr;
	uint8_t curbit;
} bit_state;

uint8_t get_bit(bit_state* bs);
uint8_t get_bits(uint8_t cnt, bit_state* bs);
uint8_t next_string(bit_state* bs);
void load_string(uint8_t id);

#endif // DECOMPRESSION_H
