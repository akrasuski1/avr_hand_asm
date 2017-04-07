#define OP_CONST_STR "\x00"
#define OP_D4_R4_STR "\x01"
#define OP_D3_R3_STR "\x02"
#define OP_RD_D4_R4_STR "\x03"
#define OP_K8_R4_STR "\x04"
#define OP_R5_STR "\x05"
#define OP_Q_R5_STR "\x06"
#define OP_K6_R2_STR "\x07"
#define OP_IO_B_STR "\x08"
#define OP_IO_R5_STR "\x09"
#define OP_K12_STR "\x0a"
#define OP_R5_B_STR "\x0b"
#define OP_K7_STR "\x0c"
#define OP_R5_K16_STR "\x0d"
#define OP_R5_Y_P_STR "\x0e"
#define OP_K22_STR "\x0f"
#define OP_K4_STR "\x10"

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
// bclr, brbc, brbs, brlo, brsh, bset, cbr, lsl, rol, sbr, ser, tst
// Also these three opcodes are different in avr-objdump:
// lds/sts (16-bit) - it seems to overlap with ldd rD, Z+q
// clr (eor rx, rx is clear enough)
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

