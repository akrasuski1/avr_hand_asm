#include <stdio.h>
#include <stdint.h>

#include "common.h"
#include "full_tables.h"

#define IS_METADATA(c) ((c)<=OP_LAST)

uint8_t all_bits[10000];
uint8_t* curbit=all_bits;
int main(){
	uint8_t* ptr=op_names;
	for(uint8_t i=0; i<OP_NAMES_NUM; i++){
		while( !IS_METADATA(*ptr) ){
			ptr++;
		}
		uint8_t op_type=*ptr++;
		uint16_t mask=type_masks[op_type];
		uint16_t opcode=op_bits[i];
		for(int b=0; b<16; b++){
			if((1<<b) & mask){
				*curbit++=!!( (1<<b) & opcode );
			}
		}
	}
	printf("// %zu bits\n", curbit-all_bits);
	printf("uint8_t compressed_op_bits[]=\n\t\"");
	for(int i=0; i<(curbit-all_bits); i+=8){
		if(i && i%128==0){
			printf("\"\n\t\"");
		}
		uint8_t a=0;
		for(int j=0; j<8; j++){
			a|=all_bits[i+j]<<j;
		}
		printf("\\x%02x", a);
	}
	printf("\";\n");
}
