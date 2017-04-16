#include "integer.h"
#include <avr/io.h>
void init_spi (void){}
void dly_100us (void){}
BYTE rcv_spi (void){return 0;}
void xmit_spi (BYTE x){DDRB=x;}

