#include "op_tree.h"

#include "common.h"

// These eight nodes are disambiguating nop from 255 surrounding reserved ops.
const PROGMEM op_node op_node_000w={0x0001u, {
	LEAF(STRING_NOP,      CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000z={0x0002u, {
	NODE(op_node_000w),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000y={0x0004u, {
	NODE(op_node_000z),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_000x={0x0008u, {
	NODE(op_node_000y),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00ww={0x0010u, {
	NODE(op_node_000x),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00zz={0x0020u, {
	NODE(op_node_00ww),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00yy={0x0040u, {
	NODE(op_node_00zz),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_00xx={0x0080u, {
	NODE(op_node_00yy),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_03xx={0x0088u, {
	LEAF(STRING_MULSU,  CATEGORY_ARITH, OP_D3_R3_CHR), 
	LEAF(STRING_FMUL,   CATEGORY_ARITH, OP_D3_R3_CHR), 
	LEAF(STRING_FMULS,  CATEGORY_ARITH, OP_D3_R3_CHR), 
	LEAF(STRING_FMULSU, CATEGORY_ARITH, OP_D3_R3_CHR), 
}};

const PROGMEM op_node op_node_0yyy={0x0300u, {
	NODE(op_node_00xx), 
	LEAF(STRING_MOVW, CATEGORY_MOVE, OP_D4_R4_CHR),
	LEAF(STRING_MULS, CATEGORY_ARITH, OP_D4_R4_CHR),
	NODE(op_node_03xx), 
}};

const PROGMEM op_node op_node_0xxx={0x0c00u, {
	NODE(op_node_0yyy), 
	LEAF(STRING_CPC, CATEGORY_JUMP,  OP_RD_D4_R4_CHR),
	LEAF(STRING_SBC, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_ADD, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_1xxx={0x0c00u, {
	LEAF(STRING_CPSE, CATEGORY_JUMP, OP_RD_D4_R4_CHR),
	LEAF(STRING_CP,   CATEGORY_JUMP, OP_RD_D4_R4_CHR),
	LEAF(STRING_SUB,  CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_ADC,  CATEGORY_ARITH, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_2xxx={0x0c00u, {
	LEAF(STRING_AND, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_EOR, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_OR,  CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_MOV, CATEGORY_MOVE,  OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_8a={0x0200u, {
	LEAF(STRING_LD, CATEGORY_MOVE, OP_Q_R5_CHR),
	LEAF(STRING_ST, CATEGORY_MOVE, OP_Q_R5_CHR),
}};

const PROGMEM op_node op_node_bxxx={0x0800u, {
	LEAF(STRING_IN,  CATEGORY_MISC, OP_IO_R5_CHR),
	LEAF(STRING_OUT, CATEGORY_MISC, OP_IO_R5_CHR),
}};

const PROGMEM op_node op_node_f0={0x0407u, {
	LEAF(STRING_BRCS, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BREQ, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRMI, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRVS, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRLT, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRHS, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRTS, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRIE, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRCC, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRNE, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRPL, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRVC, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRGE, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRHC, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRTC, CATEGORY_JUMP, OP_K7_CHR),
	LEAF(STRING_BRID, CATEGORY_JUMP, OP_K7_CHR),
}};

const PROGMEM op_node op_node_f8_real={0x0600u, {
	LEAF(STRING_BLD,  CATEGORY_MOVE, OP_R5_B_CHR),
	LEAF(STRING_BST,  CATEGORY_MOVE, OP_R5_B_CHR),
	LEAF(STRING_SBRC, CATEGORY_JUMP, OP_R5_B_CHR),
	LEAF(STRING_SBRS, CATEGORY_JUMP, OP_R5_B_CHR),
}};

const PROGMEM op_node op_node_f8={0x0008u, {
	NODE(op_node_f8_real),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_fxxx={0x0800u, {
	NODE(op_node_f0),
	NODE(op_node_f8),
}};

const PROGMEM op_node op_node_misc1={0x000f, {
	LEAF(STRING_LDS,  CATEGORY_MOVE, OP_R5_K16_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_LPM,  CATEGORY_MOVE, OP_R5_LPM_CHR),
	LEAF(STRING_LPM,  CATEGORY_MOVE, OP_R5_LPM_CHR),
	LEAF(STRING_ELPM, CATEGORY_MOVE, OP_R5_LPM_CHR),
	LEAF(STRING_ELPM, CATEGORY_MOVE, OP_R5_LPM_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_LD,   CATEGORY_MOVE, OP_R5_Y_P_CHR),
	LEAF(STRING_POP,  CATEGORY_MOVE, OP_R5_CHR),
}};

const PROGMEM op_node op_node_misc2={0x000f, {
	LEAF(STRING_STS,  CATEGORY_MOVE, OP_R5_K16_STS_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_XCH_, CATEGORY_MOVE, OP_R5_CHR),
	LEAF(STRING_LAS_, CATEGORY_MOVE, OP_R5_CHR),
	LEAF(STRING_LAC_, CATEGORY_MOVE, OP_R5_CHR),
	LEAF(STRING_LAT_, CATEGORY_MOVE, OP_R5_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_ST,   CATEGORY_MOVE, OP_R5_Y_P_ST_CHR),
	LEAF(STRING_PUSH, CATEGORY_MOVE, OP_R5_CHR),
}};

const PROGMEM op_node op_node_misc_9x8x={0x01f0, {
	LEAF(STRING_SEC,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SEZ,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SEN,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SEV,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SES,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SEH,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SET,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SEI,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLC,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLZ,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLN,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLV,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLS,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLH,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLT,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_CLI,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RET,  CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RETI, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_SLEEP, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_BREAK, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_WDR,   CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_LPM,  CATEGORY_MOVE, OP_CONST_CHR),
	LEAF(STRING_ELPM, CATEGORY_MOVE, OP_CONST_CHR),
	LEAF(STRING_SPM,  CATEGORY_MOVE, OP_CONST_CHR),
	LEAF(STRING_SPM_, CATEGORY_MOVE, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_c={0x0110, {
	LEAF(STRING_IJMP,   CATEGORY_JUMP, OP_CONST_CHR),
	LEAF(STRING_EIJMP,  CATEGORY_JUMP, OP_CONST_CHR),
	LEAF(STRING_ICALL,  CATEGORY_JUMP, OP_CONST_CHR),
	LEAF(STRING_EICALL, CATEGORY_JUMP, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_b={0x0020, {
	NODE(op_node_misc_9x9x_c),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x_a={0x0040, {
	NODE(op_node_misc_9x9x_b),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9x9x={0x0080, {
	NODE(op_node_misc_9x9x_a),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc_9xbx={0x0100, {
	LEAF(STRING_DES,      CATEGORY_MISC, OP_K4_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
}};

const PROGMEM op_node op_node_misc3={0x000f, {
	LEAF(STRING_COM,  CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_NEG,  CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_SWAP, CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_INC,  CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_RESERVED, CATEGORY_MISC, OP_CONST_CHR),
	LEAF(STRING_ASR,  CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_LSR,  CATEGORY_ARITH, OP_R5_CHR),
	LEAF(STRING_ROR,  CATEGORY_ARITH, OP_R5_CHR),
	NODE(op_node_misc_9x8x),
	NODE(op_node_misc_9x9x),
	LEAF(STRING_DEC,  CATEGORY_ARITH, OP_R5_CHR),
	NODE(op_node_misc_9xbx),
	LEAF(STRING_JMP,  CATEGORY_JUMP, OP_K22_CHR),
	LEAF(STRING_JMP,  CATEGORY_JUMP, OP_K22_CHR),
	LEAF(STRING_CALL, CATEGORY_JUMP, OP_K22_CHR),
	LEAF(STRING_CALL, CATEGORY_JUMP, OP_K22_CHR),
}};

const PROGMEM op_node op_node_adiw_sbiw={0x0100, {
	LEAF(STRING_ADIW, CATEGORY_ARITH, OP_K6_R2_CHR),
	LEAF(STRING_SBIW, CATEGORY_ARITH, OP_K6_R2_CHR),
}};

const PROGMEM op_node op_node_cbi_sbic={0x0100, {
	LEAF(STRING_CBI,  CATEGORY_MISC, OP_IO_B_CHR),
	LEAF(STRING_SBIC, CATEGORY_JUMP, OP_IO_B_CHR),
}};

const PROGMEM op_node op_node_sbi_sbis={0x0100, {
	LEAF(STRING_SBI,  CATEGORY_MISC, OP_IO_B_CHR),
	LEAF(STRING_SBIS, CATEGORY_JUMP, OP_IO_B_CHR),
}};

const PROGMEM op_node op_node_9xxx={0x0e00u, {
	NODE(op_node_misc1),
	NODE(op_node_misc2),
	NODE(op_node_misc3),
	NODE(op_node_adiw_sbiw),
	NODE(op_node_cbi_sbic),
	NODE(op_node_sbi_sbis),
	LEAF(STRING_MUL, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
	LEAF(STRING_MUL, CATEGORY_ARITH, OP_RD_D4_R4_CHR),
}};

const PROGMEM op_node op_node_root={0xf000u, { 
	NODE(op_node_0xxx),
	NODE(op_node_1xxx),
	NODE(op_node_2xxx),
	LEAF(STRING_CPI,  CATEGORY_JUMP,  OP_K8_R4_CHR), 
	LEAF(STRING_SBCI, CATEGORY_ARITH, OP_K8_R4_CHR), 
	LEAF(STRING_SUBI, CATEGORY_ARITH, OP_K8_R4_CHR), 
	LEAF(STRING_ORI,  CATEGORY_ARITH, OP_K8_R4_CHR), 
	LEAF(STRING_ANDI, CATEGORY_ARITH, OP_K8_R4_CHR), 
	NODE(op_node_8a),
	NODE(op_node_9xxx),
	NODE(op_node_8a),
	NODE(op_node_bxxx),
	LEAF(STRING_RJMP,  CATEGORY_JUMP, OP_K12_CHR), 
	LEAF(STRING_RCALL, CATEGORY_JUMP, OP_K12_CHR), 
	LEAF(STRING_LDI,   CATEGORY_MOVE, OP_K8_R4_CHR), 
	NODE(op_node_fxxx),
}};
