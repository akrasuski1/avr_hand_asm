#include "buffer_utils.h"

uint8_t buffer[20];
uint8_t* buf;

void reset(){
	buf=buffer;
}

void append(uint8_t c){
	*buf++=c;
}

void skip(uint8_t many){
	buf+=many;
}

// Up to 9999.
void append_decnum(uint16_t num){
	static uint8_t digs[4];
	uint8_t* dig=digs;
	do{
		int16_t remainder=num;
		num=0;
		while(1){
			remainder-=10;
			if(remainder<0){ break; }
			num++;
		}
		*dig++=remainder+10;
	} while(num);
	do{
		append(*--dig +'0');
	} while(dig!=digs);
}

void append_hexnibble(uint8_t num){
	num&=0xfu;
	if(num<10){
		append('0'+num);
	}
	else{
		append(('a'-10)+num);
	}
}
