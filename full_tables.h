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

#define SHORT_SPACE_Z_COMMA_STR "{"
#define SHORT_SPACE_Z_PLUS_STR  "`"

// Skipping the following opcodes, as they are overriden by their
// more specific versions at all times:
// bclr, brbc, brbs, brlo, brsh, bset, cbr, lsl, rol, sbr, ser, tst
// Also these three opcodes are different in avr-objdump:
// lds/sts (16-bit) - it seems to overlap with ldd rD, Z+q
// clr (eor rx, rx is clear enough)
//
// Also, we mostly use alphabetical order - both because of aesthetics,
// but also to compress better. We make an exception for
// ld/ldd and st/std opcodes due to their complexity.
#define OP_NAMES_NUM 111

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
	U16(1001, 0000, 0000, 0000), // ld
	U16(1000, 0010, 0000, 0000), // st
	U16(1001, 0010, 0000, 0000), // st
};

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
	OP_R5_Y_P_STR "ld"
	OP_Q_R5_STR "st"
	OP_R5_Y_P_STR "st"
;

