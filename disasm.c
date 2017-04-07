#ifdef F_CPU
	#include <avr/io.h>
#else
	#include <stdio.h>
	#include <stdint.h>
#endif

#include "common.h"
#include "compressed.h"
									

#define IS_METADATA(c) ((c)<=OP_LAST)

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

static char buffer[20];
static char* buf;

static void reset(){
	buf=buffer+sizeof(buffer);
	while(buf != buffer){
		*--buf=0;
	}
}

static void append(char c){
	*buf++=c;
}

static uint16_t div10(int16_t n){
	for(uint16_t i=0; ; i++){
		n-=10;
		if(n<0){ return i; }
	}
}

// Up to 9999.
static void append_decnum(uint16_t num){
	static uint8_t digs[4];
	uint8_t* dig=digs;
	while(num){
		uint16_t divided=div10(num);
		*dig++=num-divided*10u;
		num=divided;
	}
	if(dig==digs){
		append('0');
	}
	while(dig--!=digs){
		append(*dig +'0');
	}
}

static void append_hexnibble(uint8_t num){
	num&=0xfu;
	if(num<10){
		append('0'+num);
	}
	else{
		append(('a'-10)+num);
	}
}

static uint8_t get_bit(uint8_t** ptr, uint8_t* curbit){
	uint8_t ret=(**ptr)&(1<<*curbit);
	*curbit=(*curbit+1)&7;
	if(*curbit==0){
		(*ptr)++;
	}
	return !!ret;
}

static uint8_t get_bits(uint8_t** ptr, uint8_t* curbit, uint8_t cnt){
	uint8_t ret=0;
	for(uint8_t i=0; i<cnt; i++){
		ret|=1u<<get_bit(ptr, curbit);
	}
	return ret;
}

static uint8_t* append_str(uint8_t* str){
	while( !IS_METADATA(*str)){
		append(*str++);
	}
	return str;
}

static uint16_t next;

static void append_arguments();
static void decode(uint16_t op){
	reset();
	uint8_t* ptr=op_names;
	uint8_t arguments[16];
	uint8_t* args=arguments+sizeof(arguments);

	uint8_t* compressed_op_bits_ptr=compressed_op_bits;
	uint8_t compressed_op=0;

	while(arguments!=args){
		*--args=ARG_EOF;
	}
	*args=ARG_RESERVED;
	for(uint8_t i=0; i<OP_NAMES_NUM; i++){
		while( !IS_METADATA(*ptr) ){
			ptr++;
		}
		uint8_t op_type=*ptr++;
		uint8_t ok=1;
		uint16_t mask=type_masks[op_type];
		for(uint8_t bit=0; bit<16; bit++){
			if(mask&(1u<<bit)){
				uint8_t this_bit=!!((1u<<bit) & op);
				if(this_bit != get_bit(&compressed_op_bits_ptr, &compressed_op)){
					ok=0;
				}
			}
		}
		if(ok){
			// Found match. Let's print the name first.
			ptr=append_str(ptr);
			uint8_t dreg=(op>>4)&0x1f;
			uint8_t reg=(op&0xf)|((op&0x200)>>5);

			switch(op_type){
				case OP_R5_Y_P_CHR:
				{
					uint8_t xyz=(op>>2u)&3u;
					uint8_t p=(op&3u);

					// This complicated condition is check for validity.
					if((p!=0u || xyz==3u) && xyz!=1u && p!=3u){
						// Possible formats:
						//             p st
						// st N+, rD | 1 1
						// st -N, rD | 2 1
						// st X, rD  | 0 1
						// ld rD, N+ | 1 0
						// ld rD, -N | 2 0
						// ld rD, X  | 0 0
						if(op&0x0200u){ // st
							*args++=ARG_MXP;
							*args++=p;
							*args++=xyz;
							*args++=ARG_REG;
							*args++=dreg;
						}
						else{ // ld
							*args++=ARG_REG;
							*args++=dreg;
							*args++=ARG_MXP;
							*args++=p;
							*args++=xyz;
						}
					}
				} break;
				case OP_R5_K16_CHR:
				{
					if(op&0x0200){ // sts
						*args++=ARG_HEXWORD;
						*args++=next>>8;
						*args++=next;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // lds
						*args++=ARG_REG;
						*args++=dreg;
						*args++=ARG_HEXWORD;
						*args++=next>>8;
						*args++=next;
					}
				} break;
				case OP_K12_CHR:
				{
					uint16_t k=op&0xfff;
					*args++=ARG_OFFSET;
					*args++=12;
					*args++=k>>8;
					*args++=k;
				} break;
				case OP_Q_R5_CHR:
				{
					uint8_t q=(op&7)|((op&0x0c00u)>>7)|((op&0x2000u)>>8);
					char yz=((op>>3)&1);

					if(q){
						append('d');
					}
					if(op&0x0200u){ // st
						*args++=ARG_YPQ;
						*args++=yz;
						*args++=q;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // ld
						*args++=ARG_REG;
						*args++=dreg;
						*args++=ARG_YPQ;
						*args++=yz;
						*args++=q;
					}
				} break;
				case OP_RD_D4_R4_CHR:
				case OP_D3_R3_CHR:
				case OP_D4_R4_CHR:
				{
					if(op_type==OP_D3_R3_CHR){
						dreg=(dreg&7)+16;
						reg = (reg&7)+16;
					}
					else if(op_type==OP_D4_R4_CHR){
						if(op&0x0100u){ // movw
							dreg=(dreg&0xf)*2;
							reg = (reg&0xf)*2;
						}
						else{ // muls
							dreg=(dreg&0xf)+16;
							reg = (reg&0xf)+16;
						}
					}
					*args++=ARG_REG;
					*args++=dreg;
					*args++=ARG_REG;
					*args++=reg;
				} break;
				case OP_K6_R2_CHR:
				{
					*args++=ARG_REG;
					*args++=(dreg&3)*2+24;
					*args++=ARG_HEXBYTE;
					*args++=(op&0xf)|((op>>2)&0x30);
				} break;
				case OP_K8_R4_CHR:
				{
					*args++=ARG_REG;
					*args++=(dreg&0xf)+16;
					*args++=ARG_HEXBYTE;
					*args++=(op&0xf)|((op>>4)&0xf0);
				} break;
				case OP_R5_CHR:
				{
					*args++=ARG_REG;
					*args++=dreg;
					if((op&0xfe0cu)==0x9004){ // lpm/elpm Z(+)
						*args++=ARG_MXP;
						*args++=op&1;
						*args++=0;
					}
				} break;
				case OP_R5_B_CHR:
				{
					*args++=ARG_REG;
					*args++=dreg;
					*args++=ARG_DECBYTE;
					*args++=op&7;
				} break;
				case OP_K7_CHR:
				{
					*args++=ARG_OFFSET;
					*args++=7;
					*args++=0;
					*args++=(op>>3)&0x7f;
				} break;
				case OP_K22_CHR:
				{
					*args++=ARG_HEX3B;
					*args++=(op&1)|((op>>3)&0x3e);
				} break;
				case OP_IO_B_CHR:
				{
					*args++=ARG_HEXBYTE;
					*args++=(op>>3)&0x1f;
					*args++=ARG_DECBYTE;
					*args++=op&7;
				} break;
				case OP_IO_R5_CHR:
				{
					uint8_t a=(op&0xf)|((op>>5)&0x30);
					if(op&0x0800){ // out
						*args++=ARG_HEXBYTE;
						*args++=a;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // in
						*args++=ARG_REG;
						*args++=dreg;
						*args++=ARG_HEXBYTE;
						*args++=a;
					}
				} break;
				case OP_CONST_CHR:
				{
					*args++=ARG_EOF;
				} break;
				case OP_K4_CHR:
				{
					*args++=ARG_DECBYTE;
					*args++=(op>>4)&0xfu;
				} break;
			}
			break;
		}
	}
	append_arguments(arguments);
}

static void append_arguments(uint8_t* arguments){
	uint8_t* args=arguments;
	while(1){
		uint8_t byte=*args++;
		if(byte==0){ return; }
		if(args!=arguments+1){
			append(',');
		}
		append(' ');
		switch(byte){
			case ARG_HEXBYTE:
			case ARG_HEXWORD:
			{
				append_str((uint8_t*)"0x");
				uint8_t bytes=byte-(ARG_HEXBYTE-1);
				while(bytes--){
					uint8_t num=*args++;
					append_hexnibble(num>>4);
					append_hexnibble(num);
				}
			} break;
			case ARG_HEX3B:
			{
				static uint8_t bytes[3];
				bytes[0]=((*args++)<<1)|(next>>15);
				bytes[1]=(next&0x07f80u)>>7;
				bytes[2]=(next&0x7f)<<1;
				append('0');
				if(bytes[0] || bytes[1] || bytes[2]){
					append('x');
					uint8_t any=0;
					for(uint8_t nib=0; nib<6; nib++){
						uint8_t x=bytes[nib/2];
						if((nib&1) == 0){ x>>=4; }
						any|=x;
						if(any){
							append_hexnibble(x);
						}
					}
				}
			} break;
			case ARG_REG:
				append('r');
			case ARG_DECBYTE: // Fallthrough.
				append_decnum(*args++);
				break;
			case ARG_OFFSET:
			{
				append('.');
				uint8_t bits=*args++;
				uint16_t k=*args++;
				k=(k<<8)|*args++;
				if(k&(1u<<(bits-1))){
					append('-');
					k=(1u<<bits)-k;
				}
				else{
					append('+');
				}
				append_decnum(k*2);
			} break;
			case ARG_RESERVED:
			{
				reset();
				for(const char* c="[reserved]"; *c; c++){
					append(*c);
				}
			} break;
			case ARG_MXP:
			{
				uint8_t p=*args++;
				uint8_t xyz=*args++;
				xyz=('X'+3)-xyz-!xyz;
				if(p==2){ append('-'); }
				append(xyz);
				if(p==1){ append('+'); }
			} break;
			case ARG_YPQ:
			{
				uint8_t yz='Z'-*args++;
				uint8_t q=*args++;
				append(yz);
				if(q){
					append('+');
					append_decnum(q);
				}
			} break;
		}
	}
}

int main(){
#ifndef F_CPU
	for(int i=0; i<(1<<16); i++){
		decode(i); 
		for(char* b=buffer; *b; b++){
			if(*b==SHORT_SPACE_Z_PLUS_CHR){
				printf(" Z+");
			}
			else if(*b==SHORT_SPACE_Z_COMMA_CHR){
				printf(" Z,");
			}
			else{
				printf("%c", *b);
			}
		}
		printf("\n");
	}

#else
	for(int i=0; i<10; i++){
		next=i;
		decode(i);
		DDRB=*buf;
		DDRB=buf[1];
	}
#endif
}
