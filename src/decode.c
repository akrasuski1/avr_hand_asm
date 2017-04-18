#include "decode.h"

typedef struct op_node{
	uint16_t switchmask;
	uintptr_t next[];
} op_node;

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

// These eight nodes are disambiguating nop from 255 surrounding reserved ops.
const PROGMEM op_node op_node_000w={0x0001u, {
	LEAF(STRING_NOP,      OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000z={0x0002u, {
	NODE(op_node_000w),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000y={0x0004u, {
	NODE(op_node_000z),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000x={0x0008u, {
	NODE(op_node_000y),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00ww={0x0010u, {
	NODE(op_node_000x),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00zz={0x0020u, {
	NODE(op_node_00ww),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00yy={0x0040u, {
	NODE(op_node_00zz),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00xx={0x0080u, {
	NODE(op_node_00yy),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_03xx={0x0088u, {
	LEAF(STRING_MULSU,  OP_D3_R3_CHR), 
	LEAF(STRING_FMUL,   OP_D3_R3_CHR), 
	LEAF(STRING_FMULS,  OP_D3_R3_CHR), 
	LEAF(STRING_FMULSU, OP_D3_R3_CHR), 
}};

const PROGMEM op_node op_node_0yyy={0x0300u, {
	NODE(op_node_00xx), 
	LEAF(STRING_MOVW, OP_D4_R4_CHR),
	LEAF(STRING_MULS, OP_D4_R4_CHR),
	NODE(op_node_03xx), 
}};

const PROGMEM op_node op_node_0xxx={0x0c00u, {
	NODE(op_node_0yyy), 
	LEAF(STRING_CPC, OP_RD_D4_R4_CHR),
	LEAF(STRING_SBC, OP_RD_D4_R4_CHR),
	LEAF(STRING_ADD, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_1xxx={0x0c00u, {
	LEAF(STRING_CPSE, OP_RD_D4_R4_CHR),
	LEAF(STRING_CP,   OP_RD_D4_R4_CHR),
	LEAF(STRING_SUB,  OP_RD_D4_R4_CHR),
	LEAF(STRING_ADC,  OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_2xxx={0x0c00u, {
	LEAF(STRING_AND, OP_RD_D4_R4_CHR),
	LEAF(STRING_EOR, OP_RD_D4_R4_CHR),
	LEAF(STRING_OR,  OP_RD_D4_R4_CHR),
	LEAF(STRING_MOV, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_8a={0x0200u, {
	LEAF(STRING_LD, OP_Q_R5_CHR),
	LEAF(STRING_ST, OP_Q_R5_CHR),
}};

const PROGMEM op_node op_node_bxxx={0x0800u, {
	LEAF(STRING_IN,  OP_IO_R5_CHR),
	LEAF(STRING_OUT, OP_IO_R5_CHR),
}};

const PROGMEM op_node op_node_f0={0x0c07u, {
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

const PROGMEM op_node op_node_f8_real={0x0600u, {
	LEAF(STRING_BLD,  OP_R5_B_CHR),
	LEAF(STRING_BST,  OP_R5_B_CHR),
	LEAF(STRING_SBRC, OP_R5_B_CHR),
	LEAF(STRING_SBRS, OP_R5_B_CHR),
}};

const PROGMEM op_node op_node_f8={0x0008u, {
	NODE(op_node_f8_real),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_fxxx={0x0800u, {
	NODE(op_node_f0),
	NODE(op_node_f8),
}};

const PROGMEM op_node op_node_misc1={0x000f, {
	LEAF(STRING_LDS,  OP_R5_K16_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_LPM,  OP_R5_LPM_CHR),
	LEAF(STRING_LPM,  OP_R5_LPM_CHR),
	LEAF(STRING_ELPM, OP_R5_LPM_CHR),
	LEAF(STRING_ELPM, OP_R5_LPM_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   OP_R5_Y_P_CHR),
	LEAF(STRING_POP,  OP_R5_CHR),
}};

const PROGMEM op_node op_node_misc2={0x000f, {
	LEAF(STRING_STS,  OP_R5_K16_STS_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_XCH_Z_COMMA, OP_R5_CHR),
	LEAF(STRING_LAS_Z_COMMA, OP_R5_CHR),
	LEAF(STRING_LAC_Z_COMMA, OP_R5_CHR),
	LEAF(STRING_LAT_Z_COMMA, OP_R5_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   OP_R5_Y_P_ST_CHR),
	LEAF(STRING_PUSH, OP_R5_CHR),
}};

const PROGMEM op_node op_node_misc_9x8x={0x01f0, {
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
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_SLEEP, OP_CONST_CHR),
	LEAF(STRING_BREAK, OP_CONST_CHR),
	LEAF(STRING_WDR,   OP_CONST_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
	LEAF(STRING_LPM,  OP_CONST_CHR),
	LEAF(STRING_ELPM, OP_CONST_CHR),
	LEAF(STRING_SPM,  OP_CONST_CHR),
	LEAF(STRING_SPM_Z_PLUS, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_c={0x0110, {
	LEAF(STRING_IJMP, OP_CONST_CHR),
	LEAF(STRING_EIJMP, OP_CONST_CHR),
	LEAF(STRING_ICALL, OP_CONST_CHR),
	LEAF(STRING_EICALL, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_b={0x0020, {
	NODE(op_node_misc_9x9x_c),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_a={0x0040, {
	NODE(op_node_misc_9x9x_b),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x={0x0080, {
	NODE(op_node_misc_9x9x_a),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9xbx={0x0100, {
	LEAF(STRING_DES, OP_K4_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc3={0x000f, {
	LEAF(STRING_COM,  OP_R5_CHR),
	LEAF(STRING_NEG,  OP_R5_CHR),
	LEAF(STRING_SWAP, OP_R5_CHR),
	LEAF(STRING_INC,  OP_R5_CHR),
	LEAF(STRING_RESERVED, OP_CONST_CHR),
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

const PROGMEM op_node op_node_adiw_sbiw={0x0100, {
	LEAF(STRING_ADIW, OP_K6_R2_CHR),
	LEAF(STRING_SBIW, OP_K6_R2_CHR),
}};

const PROGMEM op_node op_node_cbi_sbic={0x0100, {
	LEAF(STRING_CBI,  OP_IO_B_CHR),
	LEAF(STRING_SBIC, OP_IO_B_CHR),
}};

const PROGMEM op_node op_node_sbi_sbis={0x0100, {
	LEAF(STRING_SBI,  OP_IO_B_CHR),
	LEAF(STRING_SBIS, OP_IO_B_CHR),
}};

const PROGMEM op_node op_node_9xxx={0x0e00u, {
	NODE(op_node_misc1),
	NODE(op_node_misc2),
	NODE(op_node_misc3),
	NODE(op_node_adiw_sbiw),
	NODE(op_node_cbi_sbic),
	NODE(op_node_sbi_sbis),
	LEAF(STRING_MUL, OP_RD_D4_R4_CHR),
	LEAF(STRING_MUL, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_root={0xf000u, { 
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

void append_hex8(uint8_t byte){
	append('0');
	append('x');
	append_hexbyte(byte);
}

void append_hex16(uint16_t word){
	append('0');
	append('x');
	append_hexbyte(word>>8);
	append_hexbyte(word);
}

void append_register(uint8_t reg){
	append('r');
	append_decnum(reg);
}

void append_hex3b(uint8_t a, uint16_t next){
	uint8_t bytes[3];
	bytes[0]=(a<<1)|(next>>15);
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
}

void append_mxp(uint8_t p, uint8_t xyz){
	xyz=('X'+3)-xyz-!xyz;
	if(p==2){ append('-'); }
	append(xyz);
	if(p==1){ append('+'); }
}

void append_ypq(uint8_t a, uint8_t q){
	uint8_t yz='Z'-a;
	append(yz);
	if(q){
		append('+');
		append_decnum(q);
	}
}

void append_offset(uint8_t bits, uint16_t k){
	append('.');
	if(k&(1u<<(bits-1))){
		append('-');
		k=(1u<<bits)-k;
	}
	else{
		append('+');
	}
	append_decnum(k*2);
}

void append_separator(){
	append(',');
	append(' ');
}

void decode(uint16_t op, uint16_t next){
	const op_node* node=&op_node_root;
	uint8_t op_type;
	while(1){
		uint8_t bits=0;
		uint16_t mask=pgm_word(&node->switchmask);
		uint16_t op_tmp=op;
		while(mask){
			if(mask&0x8000u){
				bits<<=1;
				bits|=op_tmp>>15;
			}
			mask<<=1;
			op_tmp<<=1;
		}
		intptr_t what=pgm_word(&node->next[bits]);
		if(IS_LEAF(what)){
			uint8_t str_index=NAME_FROM_IP(what);
			load_string(str_index);
			op_type=OP_TYPE_FROM_IP(what);
			break;
		}
		else{
			node=(op_node*)what;
		}
	}
	if(op_type!=OP_CONST_CHR){
		append(' ');
	}

	uint8_t dreg=(op>>4)&0x1f;
	uint8_t reg=(op&0xf)|((op&0x0200u)>>5);
	uint8_t xyz=(op>>2u)&3u;
	uint8_t p=(op&3u);
	uint8_t q=(op&7)|((op&0x0c00u)>>7)|((op&0x2000u)>>8);
	uint8_t yz=((op>>3)&1);
	uint8_t io=(op&0xf)|((op>>5)&0x30);
	uint16_t k=op&0xfff;

	switch(op_type){
		case OP_R5_Y_P_CHR:
		{
			append_register(dreg);
			append_separator();
			append_mxp(p, xyz);
		} break;
		case OP_R5_Y_P_ST_CHR:
		{
			append_mxp(p, xyz);
			append_separator();
			append_register(dreg);
		} break;
		case OP_R5_K16_CHR:
		{
			append_register(dreg);
			append_separator();
			append_hex16(next);
		} break;
		case OP_R5_K16_STS_CHR:
		{
			append_hex16(next);
			append_separator();
			append_register(dreg);
		} break;
		case OP_Q_R5_CHR:
		{
			if(q){
				buf[-1]='d';
				append(' ');
			}
			if(op&0x0200u){ // st
				append_ypq(yz, q);
				append_separator();
				append_register(dreg);
			}
			else{ // ld
				append_register(dreg);
				append_separator();
				append_ypq(yz, q);
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
			append_register(dreg);
			append_separator();
			append_register(reg);
		} break;
		case OP_K6_R2_CHR:
		{
			append_register((dreg&3)*2+24);
			append_separator();
			append_hex8((op&0xf)|((op>>2)&0x30));
		} break;
		case OP_K8_R4_CHR:
		{
			append_register((dreg&0xf)+16);
			append_separator();
			append_hex8((op&0xf)|((op>>4)&0xf0));
		} break;
		case OP_R5_CHR:
		{
			append_register(dreg);
		} break;
		case OP_R5_LPM_CHR:
		{
			append_register(dreg);
			append_separator();
			append_mxp(op&1, 0);
		} break;
		case OP_R5_B_CHR:
		{
			append_register(dreg);
			append_separator();
			append_decnum(op&7);
		} break;
		case OP_K12_CHR:
		{
			append_offset(12, k);
		} break;
		case OP_K7_CHR:
		{
			append_offset(7, (op>>3)&0x7f);
		} break;
		case OP_K22_CHR:
		{
			append_hex3b((op&1)|((op>>3)&0x3e), next);
		} break;
		case OP_IO_B_CHR:
		{
			append_hex8((op>>3)&0x1f);
			append_separator();
			append_decnum(op&7);
		} break;
		case OP_IO_R5_CHR:
		{
			if(op&0x0800u){ // out
				append_hex8(io);
				append_separator();
				append_register(dreg);
			}
			else{ // in
				append_register(dreg);
				append_separator();
				append_hex8(io);
			}
		} break;
		case OP_CONST_CHR:
		{
		} break;
		case OP_K4_CHR:
		{
			append_decnum((op>>4)&0xfu);
		} break;
		default: __builtin_unreachable();
	}
}
