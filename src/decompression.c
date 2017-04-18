#include "decompression.h"

#include "gen/comp_str_bits.h"
#include "buffer_utils.h"
#include "progmem_utils.h"

typedef struct bit_state {
	const uint8_t* PROGMEM ptr;
	uint8_t curbit;
} bit_state;

uint8_t get_bit(bit_state* bs){
	uint8_t ret=( pgm_byte(bs->ptr) >> (bs->curbit) )&1;
	bs->curbit=(bs->curbit+1)&7;
	if(bs->curbit==0){
		(bs->ptr)++;
	}
	return ret;
}

uint8_t get_bits(uint8_t cnt, bit_state* bs){
	uint8_t ret=0;
	while(cnt--){
		ret*=2;
		ret|=get_bit(bs);
	}
	return ret;
}

uint8_t next_string(bit_state* bs){
	reset();
	uint8_t skipped=get_bits(2, bs);
	skip(skipped);
	uint8_t len=get_bits(3, bs);
	uint8_t ls=len+skipped;
	if(ls==MAGIC_LEN_EOF){ return 0; }
	else if(ls==MAGIC_LEN_RESERVED){
		len=1;
	}
	else if(ls==MAGIC_LEN_NULL_TERMINATED){
		len=255; // Wait until magic null terminator.
	}
	else if(ls==MAGIC_LEN_7B_NULL_TERMINATED){
		reset(); // Skip is meaningless.
		while(1){
			uint8_t bits=get_bits(7, bs);
			if(!bits){ return 1; }
			append(bits);
		}
	}
	while(len--){
		uint8_t bits=get_bits(5, bs);
		if(!bits){ break; }
		append(bits|0x60);
	}
	return 1;
}

void load_string(uint8_t id){
	const uint8_t* skip=compressed_string_bits_skiptable;
	bit_state bs;
	bs.ptr=compressed_string_bits;
	while(id>=PARTITION_SIZE){
		id-=PARTITION_SIZE;
		bs.ptr+=pgm_byte(skip);
		skip++;
	}
	bs.curbit=0;
	do {
		next_string(&bs);
	} while(id--);
}
