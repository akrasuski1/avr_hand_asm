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

// These are equivalent to above, but used for optimizing switches.
// We have 32 values to use - plenty to spare.
#define OP_R5_Y_P_ST_CHR  17
#define OP_R5_K16_STS_CHR 18
#define OP_R5_LPM_CHR     19

#define MAGIC_LEN_EOF 10
#define MAGIC_LEN_RESERVED 9
#define MAGIC_LEN_7B_NULL_TERMINATED 8
#define MAGIC_LEN_NULL_TERMINATED 7

#define SHORT_SPACE_Z_COMMA_CHR '{' 
#define SHORT_SPACE_Z_PLUS_CHR  '|' 
#define SHORT_SPACE             '}' 

#define SHORT_SPACE_Z_COMMA_STR "{"
#define SHORT_SPACE_Z_PLUS_STR  "|"
#define SHORT_SPACE_STR         "}"

#define OP_NAMES_NUM 111

extern const char* short_strings[];

#include "gen/strings.h"

#define STRING_RESERVED STRING__RESERVED_

#endif
