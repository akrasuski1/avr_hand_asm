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

#define MAGIC_LEN_EOF 0
#define MAGIC_LEN_K4 7

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

#define SHORT_SPACE_Z_COMMA_CHR '{' 
#define SHORT_SPACE_Z_PLUS_CHR  '`' 
