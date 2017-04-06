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
#define OP_K4_STR "\x0f"
// xxxx xxxx kkkk xxxx <k>
#define OP_K22_STR "\x10"
// xxxx xxxk kkkk xxxk [k16] <k>

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
#define OP_K4_CHR 15
#define OP_K22_CHR 16

#define OP_FIRST OP_CONST_CHR
#define OP_LAST OP_K22_CHR

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
	U16(1111, 1111, 0000, 1111),
	U16(1111, 1110, 0000, 1110),
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

static uint16_t op_bits[]={
	U16(0001, 1100, 0000, 0000), // adc
	U16(0000, 1100, 0000, 0000), // add
	U16(1001, 0110, 0000, 0000), // adiw
	U16(0010, 0000, 0000, 0000), // and
	U16(0111, 0000, 0000, 0000), // andi
	U16(1001, 0100, 0000, 0101), // asr
	U16(1111, 1000, 0000, 0000), // bld
	U16(1111, 0100, 0000, 0000), // brcc
	U16(1111, 0000, 0000, 0000), // brcs
	U16(1001, 0101, 1001, 1000), // break
	U16(1111, 0000, 0000, 0001), // breq
	U16(1111, 0100, 0000, 0100), // brge
	U16(1111, 0100, 0000, 0101), // brhc
	U16(1111, 0000, 0000, 0101), // brhs
	U16(1111, 0100, 0000, 0111), // brid
	U16(1111, 0000, 0000, 0111), // brie
	U16(1111, 0000, 0000, 0100), // brlt
	U16(1111, 0000, 0000, 0010), // brmi
	U16(1111, 0100, 0000, 0001), // brne
	U16(1111, 0100, 0000, 0010), // brpl
	U16(1111, 0100, 0000, 0110), // brtc
	U16(1111, 0000, 0000, 0110), // brts
	U16(1111, 0100, 0000, 0011), // brvc
	U16(1111, 0000, 0000, 0011), // brvs
	U16(1111, 1010, 0000, 0000), // bst
	U16(1001, 0100, 0000, 1110), // call
	U16(1001, 1000, 0000, 0000), // cbi
	U16(1001, 0100, 1000, 1000), // clc
	U16(1001, 0100, 1101, 1000), // clh
	U16(1001, 0100, 1111, 1000), // cli
	U16(1001, 0100, 1010, 1000), // cln
	U16(1001, 0100, 1100, 1000), // cls
	U16(1001, 0100, 1110, 1000), // clt
	U16(1001, 0100, 1011, 1000), // clv
	U16(1001, 0100, 1001, 1000), // clz
	U16(1001, 0100, 0000, 0000), // com
	U16(0001, 0100, 0000, 0000), // cp
	U16(0000, 0100, 0000, 0000), // cpc
	U16(0011, 0000, 0000, 0000), // cpi
	U16(0001, 0000, 0000, 0000), // cpse
	U16(1001, 0100, 0000, 1010), // dec
	U16(1001, 0100, 0000, 1011), // des
	U16(1001, 0101, 0001, 1001), // eicall
	U16(1001, 0100, 0001, 1001), // eijmp
	U16(1001, 0101, 1101, 1000), // elpm
	U16(1001, 0000, 0000, 0110), // elpm
	U16(1001, 0000, 0000, 0111), // elpm
	U16(0010, 0100, 0000, 0000), // eor
	U16(0000, 0011, 0000, 1000), // fmul
	U16(0000, 0011, 1000, 0000), // fmuls
	U16(0000, 0011, 1000, 1000), // fmulsu
	U16(1001, 0101, 0000, 1001), // icall
	U16(1001, 0100, 0000, 1001), // ijmp
	U16(1011, 0000, 0000, 0000), // in
	U16(1001, 0100, 0000, 0011), // inc
	U16(1001, 0100, 0000, 1100), // jmp
	U16(1001, 0010, 0000, 0110), // lac
	U16(1001, 0010, 0000, 0101), // las
	U16(1001, 0010, 0000, 0111), // lat
	U16(1110, 0000, 0000, 0000), // ldi
	U16(1001, 0000, 0000, 0000), // lds
	U16(1001, 0101, 1100, 1000), // lpm
	U16(1001, 0000, 0000, 0100), // lpm
	U16(1001, 0000, 0000, 0101), // lpm
	U16(1001, 0100, 0000, 0110), // lsr
	U16(0010, 1100, 0000, 0000), // mov
	U16(0000, 0001, 0000, 0000), // movw
	U16(1001, 1100, 0000, 0000), // mul
	U16(0000, 0010, 0000, 0000), // muls
	U16(0000, 0011, 0000, 0000), // mulsu
	U16(1001, 0100, 0000, 0001), // neg
	U16(0000, 0000, 0000, 0000), // nop
	U16(0010, 1000, 0000, 0000), // or
	U16(0110, 0000, 0000, 0000), // ori
	U16(1011, 1000, 0000, 0000), // out
	U16(1001, 0000, 0000, 1111), // pop
	U16(1001, 0010, 0000, 1111), // push
	U16(1101, 0000, 0000, 0000), // rcall
	U16(1001, 0101, 0000, 1000), // ret
	U16(1001, 0101, 0001, 1000), // reti
	U16(1100, 0000, 0000, 0000), // rjmp
	U16(1001, 0100, 0000, 0111), // ror
	U16(0000, 1000, 0000, 0000), // sbc
	U16(0100, 0000, 0000, 0000), // sbci
	U16(1001, 1010, 0000, 0000), // sbi
	U16(1001, 1001, 0000, 0000), // sbic
	U16(1001, 1011, 0000, 0000), // sbis
	U16(1001, 0111, 0000, 0000), // sbiw
	U16(1111, 1100, 0000, 0000), // sbrc
	U16(1111, 1110, 0000, 0000), // sbrs
	U16(1001, 0100, 0000, 1000), // sec
	U16(1001, 0100, 0101, 1000), // seh
	U16(1001, 0100, 0111, 1000), // sei
	U16(1001, 0100, 0010, 1000), // sen
	U16(1001, 0100, 0100, 1000), // ses
	U16(1001, 0100, 0110, 1000), // set
	U16(1001, 0100, 0011, 1000), // sev
	U16(1001, 0100, 0001, 1000), // sez
	U16(1001, 0101, 1000, 1000), // sleep
	U16(1001, 0101, 1110, 1000), // spm
	U16(1001, 0101, 1111, 1000), // spm
	U16(1001, 0010, 0000, 0000), // sts
	U16(0001, 1000, 0000, 0000), // sub
	U16(0101, 0000, 0000, 0000), // subi
	U16(1001, 0100, 0000, 0010), // swap
	U16(1001, 0101, 1010, 1000), // wdr
	U16(1001, 0010, 0000, 0100), // xch

	U16(1000, 0000, 0000, 0000), // ld
	U16(1000, 0010, 0000, 0000), // st
	U16(1001, 0000, 0000, 0000), // ld
	U16(1001, 0010, 0000, 0000), // st
};

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

static uint8_t popcnt16(uint16_t v){
	return __builtin_popcount(v);
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
	return ret;
}

static uint8_t get_bits(uint8_t** ptr, uint8_t* curbit, uint8_t cnt){
	uint8_t ret=0;
	while(cnt--){
		ret*=2;
		ret|=get_bit(ptr, curbit);
	}
	return ret;
}

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
	printf("%zu bits\n", curbit-all_bits);
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
