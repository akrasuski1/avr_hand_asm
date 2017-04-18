#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

#define OP_CONST_CHR 0
// xxxx xxxx xxxx xxxx <>
#define OP_D4_R4_CHR 1
// xxxx xxxx dddd rrrr <dr>
#define OP_D3_R3_CHR 2
// xxxx xxxx xddd xrrr <dr>
#define OP_RD_D4_R4_CHR 3
// xxxx xxrd dddd rrrr <dr>
#define OP_K8_R4_CHR 4
// xxxx kkkk rrrr kkkk <kr>
#define OP_R5_CHR 5
// xxxx xxxr rrrr xxxx <r>
#define OP_Q_R5_CHR 6
// xxqx qqxr rrrr yqqq <qr>
#define OP_K6_R2_CHR 7
// xxxx xxxx kkrr kkkk <kr>
#define OP_IO_B_CHR 8
// xxxx xxxx aaaa abbb <ab>
#define OP_IO_R5_CHR 9
// xxxx xaar rrrr aaaa <ar>
#define OP_K12_CHR 10
// xxxx kkkk kkkk kkkk <k>
#define OP_R5_B_CHR 11
// xxxx xxxr rrrr xbbb <rb>
#define OP_K7_CHR 12
// xxxx xxkk kkkk kxxx <k>
#define OP_R5_K16_CHR 13
// xxxx xxxr rrrr xxxx [k16] <r>
#define OP_R5_Y_P_CHR 14
// xxxx xxxr rrrr yypp <rp>
#define OP_K22_CHR 15
// xxxx xxxk kkkk xxxk [k16] <k>
#define OP_K4_CHR 16
// xxxx xxxx kkkk xxxx <k>

#define OP_FIRST OP_CONST_CHR
#define OP_LAST OP_K4_CHR

#define MAGIC_LEN_EOF 10
#define MAGIC_LEN_K4 9
#define MAGIC_LEN_7B_NULL_TERMINATED 8
#define MAGIC_LEN_NULL_TERMINATED 7

#define SHORT_SPACE_Z_COMMA_CHR '{' 
#define SHORT_SPACE_Z_PLUS_CHR  '|' 
#define SHORT_SPACE             '}' 

#define SHORT_SPACE_Z_COMMA_STR "{"
#define SHORT_SPACE_Z_PLUS_STR  "|"
#define SHORT_SPACE_STR         "}"

#define U16(a, b, c, d) (0b ## a ## b ## c ## d)

#define CATEGORY_ARITHMETIC 0
#define CATEGORY_LOAD_STORE 1
#define CATEGORY_CONTROL_FLOW 2
#define CATEGORY_MISC 3

#define OP_NAMES_NUM 111

extern const char* short_strings[];
extern uint16_t type_masks[];

enum {
	STRING_RESERVED,
	STRING_ADC,
	STRING_ADD,
	STRING_ADIW,
	STRING_AND,
	STRING_ANDI,
	STRING_ARE_YOU_SURE,
	STRING_ASR,
	STRING_BLD,
	STRING_BRCC,
	STRING_BRCS,
	STRING_BREAK,
	STRING_BREQ,
	STRING_BRGE,
	STRING_BRHC,
	STRING_BRHS,
	STRING_BRID,
	STRING_BRIE,
	STRING_BRLT,
	STRING_BRMI,
	STRING_BRNE,
	STRING_BRPL,
	STRING_BRTC,
	STRING_BRTS,
	STRING_BRVC,
	STRING_BRVS,
	STRING_BST,
	STRING_CALL,
	STRING_CARD,
	STRING_CBI,
	STRING_CLC,
	STRING_CLH,
	STRING_CLI,
	STRING_CLN,
	STRING_CLS,
	STRING_CLT,
	STRING_CLV,
	STRING_CLZ,
	STRING_COM,
	STRING_CONFIG,
	STRING_CONTINUE,
	STRING_COOL,
	STRING_CP,
	STRING_CPC,
	STRING_CPI,
	STRING_CPSE,
	STRING_DEC,
	STRING_DES,
	STRING_DEVICE,
	STRING_EDIT,
	STRING_EEPROM,
	STRING_EICALL,
	STRING_EIJMP,
	STRING_ELPM,
	STRING_EOR,
	STRING_FLASH,
	STRING_FMUL,
	STRING_FMULS,
	STRING_FMULSU,
	STRING_ICALL,
	STRING_IJMP,
	STRING_IN,
	STRING_INC,
	STRING_JMP,
	STRING_LAC_Z_COMMA,
	STRING_LAS_Z_COMMA,
	STRING_LAT_Z_COMMA,
	STRING_LD,
	STRING_LDI,
	STRING_LDS,
	STRING_LOAD,
	STRING_LOCAL,
	STRING_LOCATION,
	STRING_LPM,
	STRING_LSR,
	STRING_M32,
	STRING_M8,
	STRING_MAIN_MENU,
	STRING_MOV,
	STRING_MOVE,
	STRING_MOVW,
	STRING_MUL,
	STRING_MULS,
	STRING_MULSU,
	STRING_NEG,
	STRING_NO,
	STRING_NOP,
	STRING_OPERATION,
	STRING_OR,
	STRING_ORI,
	STRING_OTHER,
	STRING_OUT,
	STRING_PAGE_SIZE,
	STRING_POP,
	STRING_PUSH,
	STRING_RCALL,
	STRING_READ_OFF,
	STRING_REMOTE,
	STRING_RET,
	STRING_RETI,
	STRING_RJMP,
	STRING_ROR,
	STRING_RUN,
	STRING_SBC,
	STRING_SBCI,
	STRING_SBI,
	STRING_SBIC,
	STRING_SBIS,
	STRING_SBIW,
	STRING_SBRC,
	STRING_SBRS,
	STRING_SD,
	STRING_SEC,
	STRING_SEH,
	STRING_SEI,
	STRING_SEN,
	STRING_SES,
	STRING_SET,
	STRING_SEV,
	STRING_SEZ,
	STRING_SLEEP,
	STRING_SPM_Z_PLUS,
	STRING_SPM,
	STRING_ST,
	STRING_STORE,
	STRING_STS,
	STRING_SUB,
	STRING_SUBI,
	STRING_SWAP,
	STRING_T13,
	STRING_TYPE,
	STRING_WDR,
	STRING_WRITE_OFF,
	STRING_XCH_Z_COMMA,
	STRING_YES,
};

#endif
