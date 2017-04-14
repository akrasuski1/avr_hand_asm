#include "decode.h"

#include "compressed.h"

#define ARG_EOF 0
#define ARG_REG 1
#define ARG_HEXBYTE 2
#define ARG_HEXWORD 3
#define ARG_HEX3B 4
#define ARG_DECBYTE 5
#define ARG_OFFSET 6
#define ARG_RESERVED 7
#define ARG_MXP 8
#define ARG_YPQ 9

uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs){
	uint8_t fail=0;
	uint16_t mask=type_masks[op_type];
	uint16_t op_tmp=op;
	for(uint8_t bit=0; bit<16; bit++){
		if(mask&1u){
			fail |= ( (op_tmp&1) ^ 
					get_bit(bs));
		}
		mask>>=1;
		op_tmp>>=1;
	}
	return !fail;
}

void decode(uint16_t op, uint16_t next){
	uint8_t arguments[16];
	uint8_t* args=arguments+sizeof(arguments);
	while(arguments!=args){
		*--args=ARG_EOF;
	}
	*args=ARG_RESERVED;

	uint8_t op_type;

	bit_state compressed_op_bs, compressed_op_names;

	compressed_op_bs.ptr=compressed_op_bits;
	compressed_op_bs.curbit=0;

	compressed_op_names.ptr=compressed_name_bits;
	compressed_op_names.curbit=0;

	while(next_string(&op_type, &compressed_op_names)){
		if(check_opcode_match(op_type, op, &compressed_op_bs)){
			// Found match. Let's print the name first.
			uint8_t dreg=(op>>4)&0x1f;
			uint8_t reg=(op&0xf)|((op&0x0200u)>>5);

			*args=ARG_REG;
			switch(op_type){
				case OP_R5_Y_P_CHR:
				{
					uint8_t xyz=(op>>2u)&3u;
					uint8_t p=(op&3u);

					// This complicated condition is check for validity.
					if((p!=0u || xyz==3u) && xyz!=1u && p!=3u){
						// Possible formats:
						//             p st
						// st N+, rD | 1 1
						// st -N, rD | 2 1
						// st X, rD  | 0 1
						// ld rD, N+ | 1 0
						// ld rD, -N | 2 0
						// ld rD, X  | 0 0
						if(op&0x0200u){ // st
							*args++=ARG_MXP;
							*args++=p;
							*args++=xyz;
							*args++=ARG_REG;
							*args++=dreg;
						}
						else{ // ld
							*++args=dreg;
							*++args=ARG_MXP;
							*++args=p;
							*++args=xyz;
						}
					}
					else{
						*args++=ARG_RESERVED;
					}
				} break;
				case OP_R5_K16_CHR:
				{
					if(op&0x0200){ // sts
						*args++=ARG_HEXWORD;
						*args++=next>>8;
						*args++=next;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // lds
						*++args=dreg;
						*++args=ARG_HEXWORD;
						*++args=next>>8;
						*++args=next;
					}
				} break;
				case OP_Q_R5_CHR:
				{
					uint8_t q=(op&7)|((op&0x0c00u)>>7)|((op&0x2000u)>>8);
					char yz=((op>>3)&1);

					if(q){
						append('d');
					}
					if(op&0x0200u){ // st
						*args++=ARG_YPQ;
						*args++=yz;
						*args++=q;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // ld
						*++args=dreg;
						*++args=ARG_YPQ;
						*++args=yz;
						*++args=q;
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
					*++args=dreg;
					*++args=ARG_REG;
					*++args=reg;
				} break;
				case OP_K6_R2_CHR:
				{
					*++args=(dreg&3)*2+24;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>2)&0x30);
				} break;
				case OP_K8_R4_CHR:
				{
					*++args=(dreg&0xf)+16;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>4)&0xf0);
				} break;
				case OP_R5_CHR:
				{
					*++args=dreg;
					if((op&0xfe0cu)==0x9004u){ // lpm/elpm Z(+)
						*++args=ARG_MXP;
						*++args=op&1;
						*++args=0;
					}
				} break;
				case OP_R5_B_CHR:
				{
					*++args=dreg;
					*++args=ARG_DECBYTE;
					*++args=op&7;
				} break;
				case OP_K12_CHR:
				{
					uint16_t k=op&0xfff;
					*args++=ARG_OFFSET;
					*args++=12;
					*args++=k>>8;
					*args++=k;
				} break;
				case OP_K7_CHR:
				{
					*args++=ARG_OFFSET;
					*args++=7;
					*args++=0;
					*args++=(op>>3)&0x7f;
				} break;
				case OP_K22_CHR:
				{
					*args++=ARG_HEX3B;
					*args++=(op&1)|((op>>3)&0x3e);
				} break;
				case OP_IO_B_CHR:
				{
					*args++=ARG_HEXBYTE;
					*args++=(op>>3)&0x1f;
					*args++=ARG_DECBYTE;
					*args++=op&7;
				} break;
				case OP_IO_R5_CHR:
				{
					uint8_t a=(op&0xf)|((op>>5)&0x30);
					if(op&0x0800u){ // out
						*args++=ARG_HEXBYTE;
						*args++=a;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // in
						*++args=dreg;
						*++args=ARG_HEXBYTE;
						*++args=a;
					}
				} break;
				case OP_CONST_CHR:
				{
					*args++=ARG_EOF;
				} break;
				case OP_K4_CHR:
				{
					*args++=ARG_DECBYTE;
					*args++=(op>>4)&0xfu;
				} break;
				default: __builtin_unreachable();
			}
			break;
		}
	}
	append_arguments(arguments, next);
}

void append_arguments(uint8_t* arguments, uint16_t next){
	uint8_t* args=arguments;
	while(1){
		uint8_t byte=*args++;
		if(byte==0){ return; }
		if(args!=arguments+1){
			append(',');
		}
		append(' ');
		switch(byte){
			case ARG_HEXBYTE:
			case ARG_HEXWORD:
			{
				append('0');
				append('x');
				uint8_t bytes=byte-(ARG_HEXBYTE-1);
				while(bytes--){
					uint8_t num=*args++;
					append_hexnibble(num>>4);
					append_hexnibble(num);
				}
			} break;
			case ARG_HEX3B:
			{
				uint8_t bytes[3];
				bytes[0]=((*args++)<<1)|(next>>15);
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
			} break;
			case ARG_REG:
				append('r');
			case ARG_DECBYTE: // Fallthrough.
				append_decnum(*args++);
				break;
			case ARG_OFFSET:
			{
				append('.');
				uint8_t bits=*args++;
				uint16_t k=*args++;
				k=(k<<8)|*args++;
				if(k&(1u<<(bits-1))){
					append('-');
					k=(1u<<bits)-k;
				}
				else{
					append('+');
				}
				append_decnum(k*2);
			} break;
			case ARG_RESERVED:
			{
				reset();
				for(const char* c="[reserved]"; *c; c++){
					append(*c);
				}
			} break;
			case ARG_MXP:
			{
				uint8_t p=*args++;
				uint8_t xyz=*args++;
				xyz=('X'+3)-xyz-!xyz;
				if(p==2){ append('-'); }
				append(xyz);
				if(p==1){ append('+'); }
			} break;
			case ARG_YPQ:
			{
				uint8_t yz='Z'-*args++;
				uint8_t q=*args++;
				append(yz);
				if(q){
					append('+');
					append_decnum(q);
				}
			} break;
			default: __builtin_unreachable();
		}
	}
}
