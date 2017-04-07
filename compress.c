#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "common.h"
#include "full_tables.h"

#define IS_METADATA(c) ((c)<=OP_LAST)

void print_all(uint8_t* from, uint8_t* to){
	for(int i=0; i<(to-from); i+=8){
		if(i && i%128==0){
			printf("\"\n\t\"");
		}
		uint8_t a=0;
		for(int j=0; j<8; j++){
			a|=from[i+j]<<j;
		}
		printf("\\x%02x", a);
	}
	printf("\";\n");
}

void store(uint8_t** where, uint8_t what, uint8_t bits){
	while(bits--){
		**where=(what>>bits)&1;
		(*where)++;
	}
}

int main(){
	uint8_t all_bits[10000];
	uint8_t* curbit=all_bits;

	uint8_t name_bits[10000];
	uint8_t* namebit=name_bits;

	uint8_t curop[100];
	uint8_t prevop[100]={};

	uint8_t* ptr=op_names;
	for(uint8_t i=0; i<OP_NAMES_NUM; i++){
		uint8_t op_type=*ptr++;
		uint16_t mask=type_masks[op_type];
		uint16_t opcode=op_bits[i];
		for(int b=0; b<16; b++){
			if((1<<b) & mask){
				*curbit++=!!( (1<<b) & opcode );
			}
		}
		uint8_t len=0;
		while( !IS_METADATA(*ptr) ){
			curop[len++]=*ptr++;
		}
		curop[len]=0;

		uint8_t real_len=len;
		// This is op_type 16, not enough place in 4 bits.
		if(op_type==OP_K4_CHR){ 
			len=MAGIC_LEN_K4; 
			op_type=0; 
			// Length is implied 3, for "des"
		}
		uint8_t compress=1;
		uint8_t special=0;
		if(!strcmp("des", (char*)curop)){
		   	compress=1; 
			special=1;
		}
		for(int j=0; j<real_len; ){
			if(compress && curop[j] && curop[j+1] && curop[j]==prevop[j] && curop[j+1]==prevop[j+1]){
				j+=3;
				if(!special){
					len--;
				}
			}
			else{
				j++;
			}
		}
		store(&namebit, len, 3);
		store(&namebit, op_type, 4);
		for(uint8_t j=0; j<real_len; ){
			if(compress && curop[j] && curop[j+1] && curop[j]==prevop[j] && curop[j+1]==prevop[j+1]){
				uint8_t c=curop[j+2]-0x60;
				store(&namebit, 28u|(c>>3), 5);
				if(curop[j+2]){
					store(&namebit, c&7, 3);
				}
				j+=3;
			}
			else{
				store(&namebit, curop[j]-0x60, 5);
				j++;
			}
		}

		for(uint8_t j=0; j<=real_len; j++){
			prevop[j]=curop[j];
		}
	}
	printf("// %zu bits\n", curbit-all_bits);
	printf("uint8_t compressed_op_bits[]=\n\t\"");
	print_all(all_bits, curbit);

	printf("\n");
	printf("// %zu bits\n", namebit-name_bits);
	printf("uint8_t compressed_name_bits[]=\n\t\"");
	print_all(name_bits, namebit);
}
