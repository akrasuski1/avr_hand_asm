#ifndef OP_TREE_H
#define OP_TREE_H

#include "progmem_utils.h"

#include <stdint.h>

typedef struct op_node{
	uint16_t switchmask;
	uintptr_t next[];
} op_node;

// On AVR, 0x8000. Code assumes this is after all flash.
#define LEAFFLAG (1ull<<(sizeof(uintptr_t)*8-1))
#define LEAF(name, category, op_type) ((name) | (category<<8) | (op_type<<10) | LEAFFLAG)
#define NODE(node) ((intptr_t)&node)
#define NAME_FROM_IP(ip) (ip&0xff)
#define CATEGORY_FROM_IP(ip) ((ip>>8)&0x3)
#define OP_TYPE_FROM_IP(ip) ((ip>>10)&0x1f)
#define IS_LEAF(ip) (ip&LEAFFLAG)

extern const PROGMEM op_node op_node_root;

#define CATEGORY_ARITH 0
#define CATEGORY_JUMP  1
#define CATEGORY_MOVE  2
#define CATEGORY_MISC  3

#endif // OP_TREE_H
