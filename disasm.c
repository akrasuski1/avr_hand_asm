#include <stdio.h>
#include <stdint.h>

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

#define FURTHER_INPUT_STR "W"
#define FURTHER_INPUT_CHR 'W'

#define U16(a, b, c, d) (0b ## a ## b ## c ## d)

uint16_t type_masks[]={
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
char op_names[]=
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
	OP_R5       "elpm" FURTHER_INPUT_STR ", Z"
	OP_R5       "elpm" FURTHER_INPUT_STR ", Z+"
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
	OP_R5       "lpm" FURTHER_INPUT_STR ", Z"
	OP_R5       "lpm" FURTHER_INPUT_STR ", Z+"
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

uint16_t op_bits[]={
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

uint16_t masked_int(uint16_t val, uint16_t mask){
	uint16_t res=0;
	uint8_t resi=0;
	for(uint8_t i=0; i<16; i++){
		if(mask & ( ((uint8_t)1u)<<i ) ){
			if(val & ( ((uint8_t)1u)<<i ) ){
				res |= ((uint16_t)1u)<<resi;
			}
			resi++;
		}
	}
	return res;
}

#define IS_METADATA(c) ((c)>='A' && (c)<='W')
#define IS_OPDATA(c) ((c)>='A' && (c)<=OP_ANY_CHR)

char buffer[64];
char* buf;

void reset(){
	for(int i=0; i<64; i++){
		buffer[i]=0;
	}
	buf=buffer;
}

void append(char c){
	*buf++=c;
}

void append_decnum(uint16_t num){
	char digs[6]={};
	uint8_t i=0;
	while(num){
		digs[i++]=num%10;
		num/=10;
	}
	if(i==0){
		append('0');
	}
	while(i--){
		append(digs[i]+'0');
	}
}

void append_hexnibble(uint8_t num){
	if(num<10){
		append('0'+num);
	}
	else{
		append(('a'-10)+num);
	}
}

void append_hex32(uint32_t val){
	append('0');
	if(val){
		append('x');
		uint8_t nib=(uint8_t)28u;
		while( !((0xful<<nib) & val) ){
			nib-=(uint8_t)4u;
		}
		for(; nib!=(uint8_t)0xfcu; nib-=(uint8_t)4u){
			append_hexnibble( (val>>nib) & 0xfu );
		}
	}
}

void append_hex16(uint16_t val){
	append('0');
	append('x');
	for(uint8_t nib=(uint8_t)0xcu; nib!=(uint8_t)0xfcu; nib-=(uint8_t)4u){
		append_hexnibble( (val>>nib) & 0xfu );
	}
}

void append_hexbyte(uint8_t num){
	append('0');
	append('x');
	append_hexnibble(num>>4);
	append_hexnibble(num&0xF);
}

void append_reg(uint8_t num){
	append('r');
	append_decnum(num);
}

void append_regspace(uint8_t num){
	append_reg(num);
	append(',');
	append(' ');
}

void decode(uint16_t op, uint16_t next){
	reset();
	char* ptr=op_names;
	for(uint8_t i=0; ; i++){
		while( !IS_OPDATA(*ptr) ){
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
			switch(op_type+'A'){
				case OP_R5_Y_P_CHR:
				{
					uint8_t r=masked_int(op, U16(0000, 0001, 1111, 0000));
					char xyz=(op>>2u)&3u;
					uint8_t st=!!(op&0x0200u);
					uint8_t p=(op&3u);

					if(xyz==1u || p==3u || (p==0u && xyz!=3u)){
						reset();
						for(const char* c="[reserved]"; *c; c++){
							*buf++=*c;
						}
					}
					else{
						append(' ');
						xyz=('X'+3)-xyz-!xyz;
						// Possible formats:
						//             p st
						// st N+, rD | 1 1
						// st -N, rD | 2 1
						// st X, rD  | 0 1
						// ld rD, N+ | 1 0
						// ld rD, -N | 2 0
						// ld rD, X  | 0 0
						if(st){ // st
							switch(p){
								case 1:
									append(xyz);
									append('+');
									break;
								case 2:
									append('-');
								case 0: // fallthrough
									append(xyz);
									break;
							}
						}
						else{
							append_reg(r);
						}
						append(',');
						append(' ');
						if(st){ // st
							append_reg(r);
						}
						else{
							switch(p){
								case 1:
									append(xyz);
									append('+');
									break;
								case 2:
									append('-');
								case 0: // fallthrough
									append(xyz);
									break;
							}
						}

					}
				} break;
				case OP_K12_CHR:
				{
					append(' ');
					append('.');
					int16_t k=masked_int(op, U16(0000, 1111, 1111, 1111));
					if(k&0x800){
						append('-');
						k=0x1000-k;
					}
					else{
						append('+');
					}
					append_decnum(k*2);
				} break;
				case OP_Q_R5_CHR:
				{
					uint8_t q=masked_int(op, U16(0010, 1100, 0000, 0111));
					uint8_t r=masked_int(op, U16(0000, 0001, 1111, 0000));
					char yz='Z'-((op>>3)&1);
					uint8_t st=!!(op&0x0200u);

					if(q){
						append('d');
					}
					append(' ');
					if(st){ // st
						append(yz);
						if(q){
							append('+');
							append_decnum(q);
						}
					}
					else{ // ld
						append_reg(r);
					}
					append(',');
					append(' ');
					if(st){ // st
						append_reg(r);
					}
					else{ // ld
						append(yz);
						if(q){
							append('+');
							append_decnum(q);
						}
					}
				} break;
				case OP_RD_D4_R4_CHR:
				{
					uint8_t r=masked_int(op, U16(0000, 0010, 0000, 1111));
					uint8_t d=masked_int(op, U16(0000, 0001, 1111, 0000));
					append(' ');
					append_regspace(d);
					append_reg(r);
				} break;
				case OP_D3_R3_CHR:
				{
					uint8_t r=masked_int(op, U16(0000, 0000, 0000, 0111));
					uint8_t d=masked_int(op, U16(0000, 0000, 0111, 0000));
					append(' ');
					append_regspace(d+16);
					append_reg(r+16);
				} break;
				case OP_D4_R4_CHR:
				{
					uint8_t r=masked_int(op, U16(0000, 0000, 0000, 1111));
					uint8_t d=masked_int(op, U16(0000, 0000, 1111, 0000));
					append(' ');
					if(op&0x0100u){ // movw
						append_regspace(d*2);
						append_reg(r*2);
					}
					else{ // muls
						append_regspace(d+16);
						append_reg(r+16);
					}
				} break;
				case OP_K6_R2_CHR:
				{
					uint8_t d=masked_int(op, U16(0000, 0000, 0011, 0000));
					uint8_t k=masked_int(op, U16(0000, 0000, 1100, 1111));
					append(' ');
					append_regspace(d*2+24);
					append_hexbyte(k);
				} break;
				case OP_K8_R4_CHR:
				{
					uint8_t d=masked_int(op, U16(0000, 0000, 1111, 0000));
					uint8_t k=masked_int(op, U16(0000, 1111, 0000, 1111));
					append(' ');
					append_regspace(d+16);
					append_hexbyte(k);
				} break;
				case OP_R5_CHR:
				{
					uint8_t d=masked_int(op, U16(0000, 0001, 1111, 0000));
					append(' ');
					append_reg(d);
				} break;
				case OP_R5_B_CHR:
				{
					uint8_t d=masked_int(op, U16(0000, 0001, 1111, 0000));
					uint8_t b=masked_int(op, U16(0000, 0000, 0000, 0111));
					append(' ');
					append_regspace(d);
					append_decnum(b);
				} break;
				case OP_K7_CHR:
				{
					uint8_t k=masked_int(op, U16(0000, 0011, 1111, 1000));
					append(' ');
					append('.');
					if(k&64){
						append('-');
						k=128-k;
					}
					else{
						append('+');
					}
					append_decnum(k*2);
				} break;
				case OP_K4_CHR:
				{
					uint8_t k=masked_int(op, U16(0000, 0000, 1111, 0000));
					append(' ');
					append_decnum(k);
				} break;
				case OP_CONST_CHR:
				case OP_ANY_CHR:
				{
				} break;
				case OP_K22_CHR:
				{
					uint32_t k=masked_int(op, U16(0000, 0001, 1111, 0001));
					k<<=16;
					k|=next;
					append(' ');
					append_hex32(k*2);
				} break;
				case OP_R5_K16_CHR:
				{
					uint32_t d=masked_int(op, U16(0000, 0001, 1111, 0000));
					append(' ');
					if(op&0x0200){
						append_hex16(next);
						append(',');
						append(' ');
						append_reg(d);
					}
					else{
						append_regspace(d);
						append_hex16(next);
					}
				} break;
				case OP_IO_B_CHR:
				{
					uint8_t a=masked_int(op, U16(0000, 0000, 1111, 1000));
					uint8_t b=masked_int(op, U16(0000, 0000, 0000, 0111));
					append(' ');
					append_hexbyte(a);
					append(',');
					append(' ');
					append_decnum(b);
				} break;
				case OP_IO_R5_CHR:
				{
					uint8_t a=masked_int(op, U16(0000, 0110, 0000, 1111));
					uint8_t d=masked_int(op, U16(0000, 0001, 1111, 0000));
					append(' ');
					if(op&0x0800){ // out
						append_hexbyte(a);
						append(',');
						append(' ');
						append_reg(d);
					}
					else{ // in
						append_regspace(d);
						append_hexbyte(a);
					}
				} break;
				default:
				{
					printf("Ehh...\n");
				} break;
			}
			if(*ptr==FURTHER_INPUT_CHR){
				ptr++;
				do {
					append(*ptr);
					ptr++;
				} while( !IS_METADATA(*ptr));
			}
			return;
		}
	}
}

int main(){
	int a=sizeof(op_names);
	int b=sizeof(op_bits);
	int c=sizeof(type_masks);
	//printf("Approximate data size:\n");
	//printf("  op_names: %d\n", a);
	//printf("   op_bits: %d\n", b);
	//printf("type_masks: %d\n", c);
	//printf("==============\n");
	//printf("     total: %d\n", a+b+c);
	for(int i=0; i<(1<<16); i++){
		decode(i, 0); printf("%s\n", buffer);
	}
}
