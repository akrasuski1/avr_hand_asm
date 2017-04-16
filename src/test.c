#include "decode.h"

#include <stdio.h>

int main(){
	for(int i=0; i<(1<<16); i++){
		decode(i, 0); 
		for(uint8_t* b=buffer; b!=buf; b++){
			if(*b==SHORT_SPACE_Z_PLUS_CHR){
				printf(" Z+");
			}
			else if(*b==SHORT_SPACE_Z_COMMA_CHR){
				printf(" Z,");
			}
			else{
				printf("%c", *b);
			}
		}
		printf("\n");
	}
	return 0;
}
