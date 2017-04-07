#ifdef F_CPU
	#include <avr/io.h>
#else
	#include <stdio.h>
	#include <stdint.h>
#endif

#define OP_CONST_STR "\x00"
// xxxx xxxx xxxx xxxx <>
#define OP_D4_R4_STR "\x01"
// xxxx xxxx dddd rrrr <dr>
#define OP_D3_R3_STR "\x02"
// xxxx xxxx xddd xrrr <dr>
#define OP_RD_D4_R4_STR "\x03"
// xxxx xxrd dddd rrrr <dr>
#define OP_K8_R4_STR "\x04"
// xxxx kkkk rrrr kkkk <kr>
#define OP_R5_STR "\x05"
// xxxx xxxr rrrr xxxx <r>
#define OP_Q_R5_STR "\x06"
// xxqx qqxr rrrr yqqq <qr>
#define OP_K6_R2_STR "\x07"
// xxxx xxxx kkrr kkkk <kr>
#define OP_IO_B_STR "\x08"
// xxxx xxxx aaaa abbb <ab>
#define OP_IO_R5_STR "\x09"
// xxxx xaar rrrr aaaa <ar>
#define OP_K12_STR "\x0a"
// xxxx kkkk kkkk kkkk <k>
#define OP_R5_B_STR "\x0b"
// xxxx xxxr rrrr xbbb <rb>
#define OP_K7_STR "\x0c"
// xxxx xxkk kkkk kxxx <k>
#define OP_R5_K16_STR "\x0d"
// xxxx xxxr rrrr xxxx [k16] <r>
#define OP_R5_Y_P_STR "\x0e"
// xxxx xxxr rrrr yypp <rp>
#define OP_K22_STR "\x0f"
// xxxx xxxk kkkk xxxk [k16] <k>
#define OP_K4_STR "\x10"
// xxxx xxxx kkkk xxxx <k>

#define OP_CONST_CHR 0
#define OP_D4_R4_CHR 1
#define OP_D3_R3_CHR 2
#define OP_RD_D4_R4_CHR 3
#define OP_K8_R4_CHR 4
#define OP_R5_CHR 5
#define OP_Q_R5_CHR 6
#define OP_K6_R2_CHR 7
#define OP_IO_B_CHR 8
#define OP_IO_R5_CHR 9
#define OP_K12_CHR 10
#define OP_R5_B_CHR 11
#define OP_K7_CHR 12
#define OP_R5_K16_CHR 13
#define OP_R5_Y_P_CHR 14
#define OP_K22_CHR 15
#define OP_K4_CHR 16

#define OP_LAST OP_K4_CHR

#define U16(a, b, c, d) (0b ## a ## b ## c ## d)

static uint16_t type_masks[]={
	U16(1111, 1111, 1111, 1111),
	U16(1111, 1111, 0000, 0000),
	U16(1111, 1111, 1000, 1000),
	U16(1111, 1100, 0000, 0000),
	U16(1111, 0000, 0000, 0000),
	U16(1111, 1110, 0000, 1111),
	U16(1101, 0010, 0000, 0000),
	U16(1111, 1111, 0000, 0000),
	U16(1111, 1111, 0000, 0000),
	U16(1111, 1000, 0000, 0000),
	U16(1111, 0000, 0000, 0000),
	U16(1111, 1110, 0000, 1000),
	U16(1111, 1100, 0000, 0111),
	U16(1111, 1110, 0000, 1111),
	U16(1111, 1110, 0000, 0000),
	U16(1111, 1110, 0000, 1110),
	U16(1111, 1111, 0000, 1111),
};

// Skipping the following opcodes, as they are overriden by their
// more specific versions at all times:
// bclr, brbc, brbs, brlo, brsh, bset, cbr, clr (eor rx, rx is clear enough),
// lds/sts (16-bit) - it seems to overlap with ldd rD, Z+q
// lsl, rol, sbr, ser, tst
//
// Also, we mostly use alphabetical order. We make an exception for
// ld/ldd and st/std opcodes due to their complexity.
#define OP_NAMES_NUM 111

#define SHORT_SPACE_Z_COMMA_STR "{" // Chosen because it's just after 'z'
#define SHORT_SPACE_Z_COMMA_CHR '{' 
#define SHORT_SPACE_Z_PLUS_STR  "|" // Chosen because it's just after 'z'
#define SHORT_SPACE_Z_PLUS_CHR  '|' 
static uint8_t op_names[]=
	OP_RD_D4_R4_STR "adc"
	OP_RD_D4_R4_STR "add"
	OP_K6_R2_STR "adiw"
	OP_RD_D4_R4_STR "and"
	OP_K8_R4_STR "andi"
	OP_R5_STR "asr"
	OP_R5_B_STR "bld"
	OP_K7_STR "brcc"
	OP_K7_STR "brcs"
	OP_CONST_STR "break"
	OP_K7_STR "breq"
	OP_K7_STR "brge"
	OP_K7_STR "brhc"
	OP_K7_STR "brhs"
	OP_K7_STR "brid"
	OP_K7_STR "brie"
	OP_K7_STR "brlt"
	OP_K7_STR "brmi"
	OP_K7_STR "brne"
	OP_K7_STR "brpl"
	OP_K7_STR "brtc"
	OP_K7_STR "brts"
	OP_K7_STR "brvc"
	OP_K7_STR "brvs"
	OP_R5_B_STR "bst"
	OP_K22_STR "call"
	OP_IO_B_STR "cbi"
	OP_CONST_STR "clc"
	OP_CONST_STR "clh"
	OP_CONST_STR "cli"
	OP_CONST_STR "cln"
	OP_CONST_STR "cls"
	OP_CONST_STR "clt"
	OP_CONST_STR "clv"
	OP_CONST_STR "clz"
	OP_R5_STR "com"
	OP_RD_D4_R4_STR "cp"
	OP_RD_D4_R4_STR "cpc"
	OP_K8_R4_STR "cpi"
	OP_RD_D4_R4_STR "cpse"
	OP_R5_STR "dec"
	OP_K4_STR "des"
	OP_CONST_STR "eicall"
	OP_CONST_STR "eijmp"
	OP_CONST_STR "elpm"
	OP_R5_STR "elpm"
	OP_R5_STR "elpm"
	OP_RD_D4_R4_STR "eor"
	OP_D3_R3_STR "fmul"
	OP_D3_R3_STR "fmuls"
	OP_D3_R3_STR "fmulsu"
	OP_CONST_STR "icall"
	OP_CONST_STR "ijmp"
	OP_IO_R5_STR "in"
	OP_R5_STR "inc"
	OP_K22_STR "jmp"
	OP_R5_STR "lac" SHORT_SPACE_Z_COMMA_STR
	OP_R5_STR "las" SHORT_SPACE_Z_COMMA_STR
	OP_R5_STR "lat" SHORT_SPACE_Z_COMMA_STR
	OP_K8_R4_STR "ldi"
	OP_R5_K16_STR "lds"
	OP_CONST_STR "lpm"
	OP_R5_STR "lpm"
	OP_R5_STR "lpm"
	OP_R5_STR "lsr"
	OP_RD_D4_R4_STR "mov"
	OP_D4_R4_STR "movw"
	OP_RD_D4_R4_STR "mul"
	OP_D4_R4_STR "muls"
	OP_D3_R3_STR "mulsu"
	OP_R5_STR "neg"
	OP_CONST_STR "nop"
	OP_RD_D4_R4_STR "or"
	OP_K8_R4_STR "ori"
	OP_IO_R5_STR "out"
	OP_R5_STR "pop"
	OP_R5_STR "push"
	OP_K12_STR "rcall"
	OP_CONST_STR "ret"
	OP_CONST_STR "reti"
	OP_K12_STR "rjmp"
	OP_R5_STR "ror"
	OP_RD_D4_R4_STR "sbc"
	OP_K8_R4_STR "sbci"
	OP_IO_B_STR "sbi"
	OP_IO_B_STR "sbic"
	OP_IO_B_STR "sbis"
	OP_K6_R2_STR "sbiw"
	OP_R5_B_STR "sbrc"
	OP_R5_B_STR "sbrs"
	OP_CONST_STR "sec"
	OP_CONST_STR "seh"
	OP_CONST_STR "sei"
	OP_CONST_STR "sen"
	OP_CONST_STR "ses"
	OP_CONST_STR "set"
	OP_CONST_STR "sev"
	OP_CONST_STR "sez"
	OP_CONST_STR "sleep"
	OP_CONST_STR "spm"
	OP_CONST_STR "spm" SHORT_SPACE_Z_PLUS_STR
	OP_R5_K16_STR "sts"
	OP_RD_D4_R4_STR "sub"
	OP_K8_R4_STR "subi"
	OP_R5_STR "swap"
	OP_CONST_STR "wdr"
	OP_R5_STR "xch" SHORT_SPACE_Z_COMMA_STR

	OP_Q_R5_STR "ld"
	OP_Q_R5_STR "st"
	OP_R5_Y_P_STR "ld"
	OP_R5_Y_P_STR "st"

	OP_CONST_STR // Sort of null terminator.
;

/*
uint8_t compressed_op_bits[]=
	"\xc7\x60\x89\x5c\x29\xf1\xd1\x83\xc7\xac\x0c\xcf\xbe\x7d\xf9\xf7"
	"\xe7\xc9\x8b\x4f\xaf\xde\xbd\xf9\xf5\xe3\xf5\xaf\xc4\x44\xa4\xc4"
	"\xa6\xc4\xa7\x44\xa5\x44\xa6\x44\xa7\xc4\xa5\xc4\xa4\x04\x65\x11"
	"\x0c\xa1\xca\xa5\xcc\xa8\xcc\xa0\xc4\xae\x34\xe4\x21\x93\x06\x1c"
	"\x78\x20\xa1\x32\x81\xd2\x8e\xd2\x4a\x4b\x56\xf2\x92\x0e\x48\xe4"
	"\x4a\x42\x16\xd2\x94\x4b\xc0\x29\xc0\x40\x28\x01\x00\x14\xbb\x8f"
	"\xfc\x64\x23\x54\x62\x54\xf2\x29\x05\xd2\xcc\xdc\xbc\xe4\xf7\x47"
	"\xa0\xc4\xa2\xc4\xa3\x44\xa1\x44\xa2\x44\xa3\xc4\xa1\xc4\xa0\x44"
	"\xac\x44\xaf\xc4\xaf\x84\xa4\x51\xa2\x44\xad\xa4\x24\x26\x32\x09";
*/
#include "compress.inc"

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
