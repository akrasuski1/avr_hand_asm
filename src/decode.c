#include "decode.h"

#include "buffer_utils.h"
#include "op_tree.h"

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
