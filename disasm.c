#include <stdio.h>
#include <stdint.h>

#ifdef F_CPU
	#include <avr/io.h>
#endif

#define OP_CONST "A"
// xxxx xxxx xxxx xxxx <>
#define OP_D4_R4 "B"
// xxxx xxxx dddd rrrr <dr>
#define OP_D3_R3 "C"
// xxxx xxxx xddd xrrr <dr>
#define OP_RD_D4_R4 "D"
// xxxx xxrd dddd rrrr <dr>
#define OP_K8_R4 "E"
// xxxx kkkk rrrr kkkk <kr>
#define OP_R5 "F"
// xxxx xxxr rrrr xxxx <r>
#define OP_Q_R5 "G"
// xxqx qqxr rrrr yqqq <qr>
#define OP_K6_R2 "H"
// xxxx xxxx kkrr kkkk <kr>
#define OP_IO_B "I"
// xxxx xxxx aaaa abbb <ab>
#define OP_IO_R5 "J"
// xxxx xaar rrrr aaaa <ar>
#define OP_K12 "K"
// xxxx kkkk kkkk kkkk <k>
#define OP_R5_B "L"
// xxxx xxxr rrrr xbbb <rb>
#define OP_K7 "M"
// xxxx xxkk kkkk kxxx <k>
#define OP_R5_K16 "N"
// xxxx xxxr rrrr xxxx [k16] <r>
#define OP_R5_Y_P "O"
// xxxx xxxr rrrr yypp <rp>
#define OP_K4 "P"
// xxxx xxxx kkkk xxxx <k>
#define OP_K22 "Q"
// xxxx xxxk kkkk xxxk [k16] <k>
#define OP_ANY "R"
// what ever what ever <>

#define OP_CONST_CHR 'A'
#define OP_D4_R4_CHR 'B'
#define OP_D3_R3_CHR 'C'
#define OP_RD_D4_R4_CHR 'D'
#define OP_K8_R4_CHR 'E'
#define OP_R5_CHR 'F'
#define OP_Q_R5_CHR 'G'
#define OP_K6_R2_CHR 'H'
#define OP_IO_B_CHR 'I'
#define OP_IO_R5_CHR 'J'
#define OP_K12_CHR 'K'
#define OP_R5_B_CHR 'L'
#define OP_K7_CHR 'M'
#define OP_R5_K16_CHR 'N'
#define OP_R5_Y_P_CHR 'O'
#define OP_K4_CHR 'P'
#define OP_K22_CHR 'Q'
#define OP_ANY_CHR 'R'

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
	U16(0000, 0000, 0000, 0000),
};

// Skipping the following opcodes, as they are overriden by their
// more specific versions at all times:
// bclr, brbc, brbs, brlo, brsh, bset, cbr, clr (eor rx, rx is clear enough),
// lds/sts (16-bit) - it seems to overlap with ldd rD, Z+q
// lsl, rol, sbr, ser, tst
//
// Also, we mostly use alphabetical order. We make an exception for
// ld/ldd and st/std opcodes due to their complexity.
static char op_names[]=
	OP_RD_D4_R4 "adc"
	OP_RD_D4_R4 "add"
	OP_K6_R2    "adiw"
	OP_RD_D4_R4 "and"
	OP_K8_R4    "andi"
	OP_R5       "asr"
	OP_R5_B     "bld"
	OP_K7       "brcc"
	OP_K7       "brcs"
	OP_CONST    "break"
	OP_K7       "breq"
	OP_K7       "brge"
	OP_K7       "brhc"
	OP_K7       "brhs"
	OP_K7       "brid"
	OP_K7       "brie"
	OP_K7       "brlt"
	OP_K7       "brmi"
	OP_K7       "brne"
	OP_K7       "brpl"
	OP_K7       "brtc"
	OP_K7       "brts"
	OP_K7       "brvc"
	OP_K7       "brvs"
	OP_R5_B     "bst"
	OP_K22      "call"
	OP_IO_B     "cbi"
	OP_CONST    "clc"
	OP_CONST    "clh"
	OP_CONST    "cli"
	OP_CONST    "cln"
	OP_CONST    "cls"
	OP_CONST    "clt"
	OP_CONST    "clv"
	OP_CONST    "clz"
	OP_R5       "com"
	OP_RD_D4_R4 "cp"
	OP_RD_D4_R4 "cpc"
	OP_K8_R4    "cpi"
	OP_RD_D4_R4 "cpse"
	OP_R5       "dec"
	OP_K4       "des"
	OP_CONST    "eicall"
	OP_CONST    "eijmp"
	OP_CONST    "elpm"
	OP_R5       "elpm"
	OP_R5       "elpm"
	OP_RD_D4_R4 "eor"
	OP_D3_R3    "fmul"
	OP_D3_R3    "fmuls"
	OP_D3_R3    "fmulsu"
	OP_CONST    "icall"
	OP_CONST    "ijmp"
	OP_IO_R5    "in"
	OP_R5       "inc"
	OP_K22      "jmp"
	OP_R5       "lac Z,"
	OP_R5       "las Z,"
	OP_R5       "lat Z,"
	OP_K8_R4    "ldi"
	OP_R5_K16   "lds"
	OP_CONST    "lpm"
	OP_R5       "lpm"
	OP_R5       "lpm"
	OP_R5       "lsr"
	OP_RD_D4_R4 "mov"
	OP_D4_R4    "movw"
	OP_RD_D4_R4 "mul"
	OP_D4_R4    "muls"
	OP_D3_R3    "mulsu"
	OP_R5       "neg"
	OP_CONST    "nop"
	OP_RD_D4_R4 "or"
	OP_K8_R4    "ori"
	OP_IO_R5    "out"
	OP_R5       "pop"
	OP_R5       "push"
	OP_K12      "rcall"
	OP_CONST    "ret"
	OP_CONST    "reti"
	OP_K12      "rjmp"
	OP_R5       "ror"
	OP_RD_D4_R4 "sbc"
	OP_K8_R4    "sbci"
	OP_IO_B     "sbi"
	OP_IO_B     "sbic"
	OP_IO_B     "sbis"
	OP_K6_R2    "sbiw"
	OP_R5_B     "sbrc"
	OP_R5_B     "sbrs"
	OP_CONST    "sec"
	OP_CONST    "seh"
	OP_CONST    "sei"
	OP_CONST    "sen"
	OP_CONST    "ses"
	OP_CONST    "set"
	OP_CONST    "sev"
	OP_CONST    "sez"
	OP_CONST    "sleep"
	OP_CONST    "spm"
	OP_CONST    "spm Z+"
	OP_R5_K16   "sts"
	OP_RD_D4_R4 "sub"
	OP_K8_R4    "subi"
	OP_R5       "swap"
	OP_CONST    "wdr"
	OP_R5       "xch Z,"

	OP_Q_R5     "ld"
	OP_Q_R5     "st"
	OP_R5_Y_P   "ld"
	OP_R5_Y_P   "st"

	OP_ANY "[reserved]"
	OP_ANY // Sort of null terminator.
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

	U16(0000, 0000, 0000, 0000), // [reserved]
};

#define IS_METADATA(c) ((c)>='A' && (c)<=OP_ANY_CHR)

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

static char buffer[64];
static char* buf;

static void reset(){
	for(int i=0; i<64; i++){
		buffer[i]=0;
	}
	buf=buffer;
}

static void append(char c){
	*buf++=c;
}

static uint16_t divu10(uint16_t n){
	for(uint16_t i=0; ; i++){
		if(i*10u>n){ return i-1u; }
	}
}

// Up to 9999.
static void append_decnum(uint16_t num){
	static uint8_t digs[4];
	uint8_t* dig=digs;
	while(num){
		uint16_t divided=divu10(num);
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

static uint16_t next;
static uint8_t arguments[16];

static void append_arguments(){
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
				append('0');
				append('x');
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

static void decode(uint16_t op){
	reset();
	char* ptr=op_names;
	uint8_t* args=arguments+sizeof(arguments);
	while(arguments!=args){
		*args--=ARG_EOF;
	}
	*args=ARG_RESERVED;
	for(uint8_t i=0; ; i++){
		while( !IS_METADATA(*ptr) ){
			ptr++;
		}
		uint8_t op_type=ptr[0]-'A';
		ptr++;
		if( (op & type_masks[op_type]) == op_bits[i] ){
			// Found match. Let's print the name first.
			while( !IS_METADATA(*ptr)){
				append(*ptr);
				ptr++;
			}
			uint8_t dreg=(op>>4)&0x1f;
			uint8_t st=!!(op&0x0200u);
			uint8_t reg=(op&0xf)|((op&0x200)>>5);
			switch(op_type+'A'){
				case OP_R5_Y_P_CHR:
				{
					char xyz=(op>>2u)&3u;
					uint8_t p=(op&3u);

					// This complicated condition is check for validity.
					if(xyz!=1u && p!=3u && (p!=0u || xyz==3u)){
						// Possible formats:
						//             p st
						// st N+, rD | 1 1
						// st -N, rD | 2 1
						// st X, rD  | 0 1
						// ld rD, N+ | 1 0
						// ld rD, -N | 2 0
						// ld rD, X  | 0 0
						if(st){ // st
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
				case OP_K12_CHR:
				{
					int16_t k=op&0xfff;
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
					if(st){ // st
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
				{
					*args++=ARG_REG;
					*args++=dreg;
					*args++=ARG_REG;
					*args++=reg;
				} break;
				case OP_D3_R3_CHR:
				{
					*args++=ARG_REG;
					*args++=(dreg&7)+16;
					*args++=ARG_REG;
					*args++=(reg&7)+16;
				} break;
				case OP_D4_R4_CHR:
				{
					reg&=0xf;
					dreg&=0xf;
					if(op&0x0100u){ // movw
						*args++=ARG_REG;
						*args++=dreg*2;
						*args++=ARG_REG;
						*args++=reg*2;
					}
					else{ // muls
						*args++=ARG_REG;
						*args++=dreg+16;
						*args++=ARG_REG;
						*args++=reg+16;
					}
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
				case OP_K4_CHR:
				{
					*args++=ARG_DECBYTE;
					*args++=(op>>4)&0xf;
				} break;
				case OP_K22_CHR:
				{
					*args++=ARG_HEX3B;
					*args++=(op&1)|((op>>3)&0x3e);
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
				case OP_ANY_CHR:
				default:
				{
					*args=ARG_EOF;
				} break;
			}
			break;
		}
	}
	append_arguments();
}

int main(){
#ifndef F_CPU
	for(int i=0; i<(1<<16); i++){
		decode(i); printf("%s\n", buffer);
	}

#else
	for(int i=0; i<10; i++){
		decode(i);
		DDRB=*buf;
		DDRB=buf[1];
	}
#endif
}
