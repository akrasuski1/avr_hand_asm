#include "decode.h"

#include "gen/comp_op_bits.h"
#include "gen/comp_name_bits.h"

enum {
	ARG_EOF,
	ARG_REG,
	ARG_HEXBYTE,
	ARG_HEXWORD,
	ARG_HEX3B,
	ARG_DECBYTE,
	ARG_OFFSET,
	ARG_RESERVED,
	ARG_MXP,
	ARG_YPQ,
};

uint16_t get_op_mask(bit_state* bs, uint16_t mask){
	uint16_t ret=0;
	for(uint8_t bit=0; bit<16; bit++){
		ret>>=1;
		if(mask&1u){
			if(get_bit(bs)){
				ret|=0x8000u;
			}
		}
		mask>>=1;
	}
	return ret;
}

uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs){
	uint16_t mask=type_masks[op_type];
	uint16_t op_mask=get_op_mask(bs, mask);
	return (mask&op)==op_mask;
}

void init_ops_names(compressed_ops_names* bs){
	bs->compressed_op_bs.ptr=compressed_op_bits;
	bs->compressed_op_bs.curbit=0;

	bs->compressed_op_names.ptr=compressed_name_bits;
	bs->compressed_op_names.curbit=0;
}

typedef struct op_node{
	uint16_t switchmask;
	uintptr_t next[];
} op_node;

char* op_name_table[]={
	"adc"   ,"add"  ,"adiw"  ,"and"  ,"andi","asr" ,"bld"  ,"brcc" ,
	"brcs"  ,"break","breq"  ,"brge" ,"brhc","brhs","brid" ,"brie" ,
	"brlt"  ,"brmi" ,"brne"  ,"brpl" ,"brtc","brts","brvc" ,"brvs" ,
	"bst"   ,"call" ,"cbi"   ,"clc"  ,"clh" ,"cli" ,"cln"  ,"cls"  ,
	"clt"   ,"clv"  ,"clz"   ,"com"  ,"cp"  ,"cpc" ,"cpi"  ,"cpse" ,
	"dec"   ,"des"  ,"eicall","eijmp","elpm","eor" ,"fmul" ,"fmuls",
	"fmulsu","icall","ijmp"  ,"in"   ,"inc" ,"jmp" ,
	"lac" SHORT_SPACE_Z_COMMA_STR,
	"las" SHORT_SPACE_Z_COMMA_STR,
	"lat" SHORT_SPACE_Z_COMMA_STR,
	"ld"    ,"ldi"  ,"lds"   ,"lpm"  ,"lsr" ,"mov" ,"movw" ,"mul"  ,
	"muls"  ,"mulsu","neg"   ,"nop"  ,"or"  ,"ori" ,"out"  ,"pop"  ,
	"push"  ,"rcall","ret"   ,"reti" ,"rjmp","ror" ,"sbc"  ,"sbci" ,
	"sbi"   ,"sbic" ,"sbis"  ,"sbiw" ,"sbrc","sbrs","sec"  ,"seh"  ,
	"sei"   ,"sen"  ,"ses"   ,"set"  ,"sev" ,"sez" ,"sleep","spm"  ,
	"spm" SHORT_SPACE_Z_PLUS_STR,
	"st"    ,"sts"  ,"sub"   ,"subi" ,"swap","wdr" ,
	"xch" SHORT_SPACE_Z_COMMA_STR,
	"[reserved]",
};

enum {
	STRING_ADC   ,STRING_ADD  ,STRING_ADIW  ,STRING_AND  ,STRING_ANDI,STRING_ASR ,STRING_BLD  ,STRING_BRCC ,
	STRING_BRCS  ,STRING_BREAK,STRING_BREQ  ,STRING_BRGE ,STRING_BRHC,STRING_BRHS,STRING_BRID ,STRING_BRIE ,
	STRING_BRLT  ,STRING_BRMI ,STRING_BRNE  ,STRING_BRPL ,STRING_BRTC,STRING_BRTS,STRING_BRVC ,STRING_BRVS ,
	STRING_BST   ,STRING_CALL ,STRING_CBI   ,STRING_CLC  ,STRING_CLH ,STRING_CLI ,STRING_CLN  ,STRING_CLS  ,
	STRING_CLT   ,STRING_CLV  ,STRING_CLZ   ,STRING_COM  ,STRING_CP  ,STRING_CPC ,STRING_CPI  ,STRING_CPSE ,
	STRING_DEC   ,STRING_DES  ,STRING_EICALL,STRING_EIJMP,STRING_ELPM,STRING_EOR ,STRING_FMUL ,STRING_FMULS,
	STRING_FMULSU,STRING_ICALL,STRING_IJMP  ,STRING_IN   ,STRING_INC ,STRING_JMP ,
	STRING_LAC_Z_COMMA,
	STRING_LAS_Z_COMMA,
	STRING_LAT_Z_COMMA,
	STRING_LD    ,STRING_LDI  ,STRING_LDS   ,STRING_LPM  ,STRING_LSR ,STRING_MOV ,STRING_MOVW ,STRING_MUL  ,
	STRING_MULS  ,STRING_MULSU,STRING_NEG   ,STRING_NOP  ,STRING_OR  ,STRING_ORI ,STRING_OUT  ,STRING_POP  ,
	STRING_PUSH  ,STRING_RCALL,STRING_RET   ,STRING_RETI ,STRING_RJMP,STRING_ROR ,STRING_SBC  ,STRING_SBCI ,
	STRING_SBI   ,STRING_SBIC ,STRING_SBIS  ,STRING_SBIW ,STRING_SBRC,STRING_SBRS,STRING_SEC  ,STRING_SEH  ,
	STRING_SEI   ,STRING_SEN  ,STRING_SES   ,STRING_SET  ,STRING_SEV ,STRING_SEZ ,STRING_SLEEP,STRING_SPM  ,
	STRING_SPM_Z_PLUS,
	STRING_ST    ,STRING_STS  ,STRING_SUB   ,STRING_SUBI ,STRING_SWAP,STRING_WDR ,
	STRING_XCH_Z_COMMA,
	STRING_RESERVED_2,
};

#if __AVR__
#define LEAFFLAG 0x8000u
#else
#define LEAFFLAG (1ull<<(sizeof(uintptr_t)*8-1))
#endif

#define LEAF(name, op_type) ((name) | (op_type<<10) | LEAFFLAG)
#define NODE(node) ((intptr_t)&node)
#define NAME_FROM_IP(ip) (ip&0xff)
#define OP_TYPE_FROM_IP(ip) ((ip>>10)&0x1f)
#define IS_LEAF(ip) (ip&LEAFFLAG)

#define STRING_NONE 0xff

// These eight nodes are disambiguating nop from 255 surrounding reserved ops.
op_node op_node_000w={0x0001u, {
	LEAF(STRING_NOP,      OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_000z={0x0002u, {
	NODE(op_node_000w),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_000y={0x0004u, {
	NODE(op_node_000z),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_000x={0x0008u, {
	NODE(op_node_000y),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_00ww={0x0010u, {
	NODE(op_node_000x),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_00zz={0x0020u, {
	NODE(op_node_00ww),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_00yy={0x0040u, {
	NODE(op_node_00zz),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};
op_node op_node_00xx={0x0080u, {
	NODE(op_node_00yy),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_03xx={0x0088u, {
	LEAF(STRING_MULSU,  OP_D3_R3_CHR), 
	LEAF(STRING_FMUL,   OP_D3_R3_CHR), 
	LEAF(STRING_FMULS,  OP_D3_R3_CHR), 
	LEAF(STRING_FMULSU, OP_D3_R3_CHR), 
}};

op_node op_node_0yyy={0x0300u, {
	NODE(op_node_00xx), 
	LEAF(STRING_MOVW, OP_D4_R4_CHR),
	LEAF(STRING_MULS, OP_D4_R4_CHR),
	NODE(op_node_03xx), 
}};

op_node op_node_0xxx={0x0c00u, {
	NODE(op_node_0yyy), 
	LEAF(STRING_CPC, OP_RD_D4_R4_CHR),
	LEAF(STRING_SBC, OP_RD_D4_R4_CHR),
	LEAF(STRING_ADD, OP_RD_D4_R4_CHR),
}};

op_node op_node_1xxx={0x0c00u, {
	LEAF(STRING_CPSE, OP_RD_D4_R4_CHR),
	LEAF(STRING_CP,   OP_RD_D4_R4_CHR),
	LEAF(STRING_SUB,  OP_RD_D4_R4_CHR),
	LEAF(STRING_ADC,  OP_RD_D4_R4_CHR),
}};

op_node op_node_2xxx={0x0c00u, {
	LEAF(STRING_AND, OP_RD_D4_R4_CHR),
	LEAF(STRING_EOR, OP_RD_D4_R4_CHR),
	LEAF(STRING_OR,  OP_RD_D4_R4_CHR),
	LEAF(STRING_MOV, OP_RD_D4_R4_CHR),
}};

op_node op_node_8a={0x0200u, {
	LEAF(STRING_LD, OP_Q_R5_CHR),
	LEAF(STRING_ST, OP_Q_R5_CHR),
}};

op_node op_node_bxxx={0x0800u, {
	LEAF(STRING_IN,  OP_IO_R5_CHR),
	LEAF(STRING_OUT, OP_IO_R5_CHR),
}};

op_node op_node_f0={0x0c07u, {
	LEAF(STRING_BRCS, OP_K7_CHR),
	LEAF(STRING_BREQ, OP_K7_CHR),
	LEAF(STRING_BRMI, OP_K7_CHR),
	LEAF(STRING_BRVS, OP_K7_CHR),
	LEAF(STRING_BRLT, OP_K7_CHR),
	LEAF(STRING_BRHS, OP_K7_CHR),
	LEAF(STRING_BRTS, OP_K7_CHR),
	LEAF(STRING_BRIE, OP_K7_CHR),
	LEAF(STRING_BRCC, OP_K7_CHR),
	LEAF(STRING_BRNE, OP_K7_CHR),
	LEAF(STRING_BRPL, OP_K7_CHR),
	LEAF(STRING_BRVC, OP_K7_CHR),
	LEAF(STRING_BRGE, OP_K7_CHR),
	LEAF(STRING_BRHC, OP_K7_CHR),
	LEAF(STRING_BRTC, OP_K7_CHR),
	LEAF(STRING_BRID, OP_K7_CHR),
}};

op_node op_node_f8_real={0x0600u, {
	LEAF(STRING_BLD,  OP_R5_B_CHR),
	LEAF(STRING_BST,  OP_R5_B_CHR),
	LEAF(STRING_SBRC, OP_R5_B_CHR),
	LEAF(STRING_SBRS, OP_R5_B_CHR),
}};

op_node op_node_f8={0x0008u, {
	NODE(op_node_f8_real),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_fxxx={0x0800u, {
	NODE(op_node_f0),
	NODE(op_node_f8),
}};

op_node op_node_misc1={0x000f, {
	LEAF(STRING_LDS,  OP_R5_K16_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_LPM,  OP_R5_CHR),
	LEAF(STRING_LPM,  OP_R5_CHR),
	LEAF(STRING_ELPM, OP_R5_CHR),
	LEAF(STRING_ELPM, OP_R5_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_POP,  OP_R5_CHR),
}};

op_node op_node_misc2x={0x000f, {
	LEAF(STRING_STS,  OP_R5_K16_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_CHR),
	LEAF(STRING_PUSH, OP_R5_CHR),
}};

op_node op_node_misc2={0x0000, {
	LEAF(STRING_NONE, OP_CONST_CHR)
}};

op_node op_node_misc_9x8x={0x01f0, {
	LEAF(STRING_SEC,  OP_CONST_CHR),
	LEAF(STRING_SEZ,  OP_CONST_CHR),
	LEAF(STRING_SEN,  OP_CONST_CHR),
	LEAF(STRING_SEV,  OP_CONST_CHR),
	LEAF(STRING_SES,  OP_CONST_CHR),
	LEAF(STRING_SEH,  OP_CONST_CHR),
	LEAF(STRING_SET,  OP_CONST_CHR),
	LEAF(STRING_SEI,  OP_CONST_CHR),
	LEAF(STRING_CLC,  OP_CONST_CHR),
	LEAF(STRING_CLZ,  OP_CONST_CHR),
	LEAF(STRING_CLN,  OP_CONST_CHR),
	LEAF(STRING_CLV,  OP_CONST_CHR),
	LEAF(STRING_CLS,  OP_CONST_CHR),
	LEAF(STRING_CLH,  OP_CONST_CHR),
	LEAF(STRING_CLT,  OP_CONST_CHR),
	LEAF(STRING_CLI,  OP_CONST_CHR),
	LEAF(STRING_RET,  OP_CONST_CHR),
	LEAF(STRING_RETI, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_SLEEP, OP_CONST_CHR),
	LEAF(STRING_BREAK, OP_CONST_CHR),
	LEAF(STRING_WDR,   OP_CONST_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_LPM,  OP_CONST_CHR),
	LEAF(STRING_ELPM, OP_CONST_CHR),
	LEAF(STRING_SPM,  OP_CONST_CHR),
	LEAF(STRING_SPM_Z_PLUS, OP_CONST_CHR),
}};

op_node op_node_misc_9x9x_c={0x0110, {
	LEAF(STRING_IJMP, OP_CONST_CHR),
	LEAF(STRING_EIJMP, OP_CONST_CHR),
	LEAF(STRING_ICALL, OP_CONST_CHR),
	LEAF(STRING_EICALL, OP_CONST_CHR),
}};

op_node op_node_misc_9x9x_b={0x0020, {
	NODE(op_node_misc_9x9x_c),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_misc_9x9x_a={0x0040, {
	NODE(op_node_misc_9x9x_b),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_misc_9x9x={0x0080, {
	NODE(op_node_misc_9x9x_a),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_misc_9xbx={0x0100, {
	LEAF(STRING_DES, OP_K4_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
}};

op_node op_node_misc3={0x000f, {
	LEAF(STRING_COM,  OP_R5_CHR),
	LEAF(STRING_NEG,  OP_R5_CHR),
	LEAF(STRING_SWAP, OP_R5_CHR),
	LEAF(STRING_INC,  OP_R5_CHR),
	LEAF(STRING_RESERVED_2, OP_CONST_CHR),
	LEAF(STRING_ASR,  OP_R5_CHR),
	LEAF(STRING_LSR,  OP_R5_CHR),
	LEAF(STRING_ROR,  OP_R5_CHR),
	NODE(op_node_misc_9x8x),
	NODE(op_node_misc_9x9x),
	LEAF(STRING_DEC,  OP_R5_CHR),
	NODE(op_node_misc_9xbx),
	LEAF(STRING_JMP,  OP_K22_CHR),
	LEAF(STRING_JMP,  OP_K22_CHR),
	LEAF(STRING_CALL, OP_K22_CHR),
	LEAF(STRING_CALL, OP_K22_CHR),
}};

op_node op_node_adiw_sbiw={0x0100, {
	LEAF(STRING_ADIW, OP_K6_R2_CHR),
	LEAF(STRING_SBIW, OP_K6_R2_CHR),
}};

op_node op_node_cbi_sbic={0x0100, {
	LEAF(STRING_CBI,  OP_IO_B_CHR),
	LEAF(STRING_SBIC, OP_IO_B_CHR),
}};

op_node op_node_sbi_sbis={0x0100, {
	LEAF(STRING_SBI,  OP_IO_B_CHR),
	LEAF(STRING_SBIS, OP_IO_B_CHR),
}};

op_node op_node_9xxx={0x0e00u, {
	NODE(op_node_misc1),
	NODE(op_node_misc2),
	NODE(op_node_misc3),
	NODE(op_node_adiw_sbiw),
	NODE(op_node_cbi_sbic),
	NODE(op_node_sbi_sbis),
	LEAF(STRING_MUL, OP_RD_D4_R4_CHR),
	LEAF(STRING_MUL, OP_RD_D4_R4_CHR),
}};

op_node op_node_root={0xf000u, { 
	NODE(op_node_0xxx),
	NODE(op_node_1xxx),
	NODE(op_node_2xxx),
	LEAF(STRING_CPI,  OP_K8_R4_CHR), 
	LEAF(STRING_SBCI, OP_K8_R4_CHR), 
	LEAF(STRING_SUBI, OP_K8_R4_CHR), 
	LEAF(STRING_ORI,  OP_K8_R4_CHR), 
	LEAF(STRING_ANDI, OP_K8_R4_CHR), 
	NODE(op_node_8a),
	NODE(op_node_9xxx),
	NODE(op_node_8a),
	NODE(op_node_bxxx),
	LEAF(STRING_RJMP, OP_K12_CHR), 
	LEAF(STRING_RCALL,OP_K12_CHR), 
	LEAF(STRING_LDI,  OP_K8_R4_CHR), 
	NODE(op_node_fxxx),
}};
#include <stdio.h>

void decode(uint16_t op, uint16_t next){
	uint8_t arguments[16];
	uint8_t* args=arguments+sizeof(arguments);
	while(arguments!=args){
		*--args=ARG_EOF;
	}
	*args=ARG_RESERVED;

	uint8_t op_type;

	compressed_ops_names bs;
	init_ops_names(&bs);

	reset();
	op_node* node=&op_node_root;
	while(1){
		uint8_t bits=0;
		uint16_t mask=node->switchmask;
		uint16_t op_tmp=op;
		while(mask){
			if(mask&0x8000u){
				bits<<=1;
				bits|=op_tmp>>15;
			}
			mask<<=1;
			op_tmp<<=1;
		}
		intptr_t what=node->next[bits];
		if(IS_LEAF(what)){
			uint8_t str_index=NAME_FROM_IP(what);
			if(str_index==STRING_NONE){ /*printf("");*/ break; } //TODO
			char* name=op_name_table[str_index];
			while(*name){
				append(*name++);
			}
			op_type=OP_TYPE_FROM_IP(what);
			goto found;
		}
		else{
			node=(op_node*)what;
		}
	}

	while(next_string(&op_type, &bs.compressed_op_names)){
		if(check_opcode_match(op_type, op, &bs.compressed_op_bs)){
			uint8_t dreg;
found:
			// Found match. Let's print the name first.
			dreg=(op>>4)&0x1f;
			uint8_t reg=(op&0xf)|((op&0x0200u)>>5);

			*args=ARG_REG;
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
							*++args=dreg;
							*++args=ARG_MXP;
							*++args=p;
							*++args=xyz;
						}
					}
					else{
						*args++=ARG_RESERVED;
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
						*++args=dreg;
						*++args=ARG_HEXWORD;
						*++args=next>>8;
						*++args=next;
					}
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
						*++args=dreg;
						*++args=ARG_YPQ;
						*++args=yz;
						*++args=q;
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
					*++args=dreg;
					*++args=ARG_REG;
					*++args=reg;
				} break;
				case OP_K6_R2_CHR:
				{
					*++args=(dreg&3)*2+24;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>2)&0x30);
				} break;
				case OP_K8_R4_CHR:
				{
					*++args=(dreg&0xf)+16;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>4)&0xf0);
				} break;
				case OP_R5_CHR:
				{
					*++args=dreg;
					if((op&0xfe0cu)==0x9004u){ // lpm/elpm Z(+)
						*++args=ARG_MXP;
						*++args=op&1;
						*++args=0;
					}
				} break;
				case OP_R5_B_CHR:
				{
					*++args=dreg;
					*++args=ARG_DECBYTE;
					*++args=op&7;
				} break;
				case OP_K12_CHR:
				{
					uint16_t k=op&0xfff;
					*args++=ARG_OFFSET;
					*args++=12;
					*args++=k>>8;
					*args++=k;
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
					if(op&0x0800u){ // out
						*args++=ARG_HEXBYTE;
						*args++=a;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // in
						*++args=dreg;
						*++args=ARG_HEXBYTE;
						*++args=a;
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
				default: __builtin_unreachable();
			}
			break;
		}
	}
	append_arguments(arguments, next);
}

void append_arguments(uint8_t* arguments, uint16_t next){
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
					append_hexbyte(num);
				}
			} break;
			case ARG_HEX3B:
			{
				uint8_t bytes[3];
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
				load_string(STRING_RESERVED);
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
			default: __builtin_unreachable();
		}
	}
}
