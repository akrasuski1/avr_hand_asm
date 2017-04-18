#ifndef OP_TREE_H
#define OP_TREE_H

#include "progmem_utils.h"

#include <stdint.h>

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

extern const PROGMEM op_node op_node_root;

#endif // OP_TREE_H
