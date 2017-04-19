#include "hd44780.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

// Machine specific.
#define lcd_init_ports() { DDRC  =  0xff;  }
#define lcd_e_strobe()   { PORTC|=  1<<5;  \
	                       PORTC&=~(1<<5); }
#define lcd_rs_set()     { PORTC|=  1<<4;  }
#define lcd_rs_clr()     { PORTC&=~(1<<4); }
#define lcd_put_dat(dat) { PORTC = (PORTC&0xf0)|(dat); }

// Note:
// This library is optimized for size, not speed. Some delay numbers are weird
// to get rid of nops and other padding.

void lcd_byte(uint8_t byte){
	for(uint8_t i=2; i--; ){
		lcd_put_dat(byte>>4);
		lcd_e_strobe();
		// I'd use "byte<<=4" here, but this compiles to just swap instruction.
		byte = ((uint8_t)(byte<<4)) | ((uint8_t)(byte>>4));
		_delay_us(80-2); 
	}
	// Normally 40us of delay at the end of function would be plenty, but to 
	// save space, same function is used elsewhere when timing is longer.
}

void lcd_cmd(uint8_t cmd){
	lcd_rs_clr();
	lcd_byte(cmd);
	_delay_us(2000-3);
}

void lcd_init(){
	lcd_init_ports();
	_delay_us(40000-3);
	lcd_retry();
}

void lcd_retry(){
	// Normally we would write:
	//           3 [5ms] 3 [160us]           3 [160us] 2 - to set 4-bit mode.
	// We write:
	// 3 [160us] 3 [5ms] 3 [160us] 3 [ 5ms ] 3 [160us] 2 - with similar effect.
	// (It is only important that there is are at least 3 repetitions with at
	// least one 5ms break somewhere near beginning).
	// Further five bytes are real commands.
	// 0x28 - 2 line, 4 bit, 5x7 font.
	// 0x08 - Display off.
	// 0x01 - Clear.
	// 0x06 - Cursor move right, no shift.
	// 0x0c - Display on, cursor off, no blinking.
	static PROGMEM const uint8_t data[]={0x33, 0x33, 0x32, 0x28, 0x08, 0x01, 0x06, 0x0c};
	for(uint8_t i=0; i<sizeof(data); i++){
		lcd_cmd(pgm_read_byte(data+i));
	}
}

void lcd_xy(uint8_t row, uint8_t col){
	lcd_cmd((row?0xc0u:0x80u)|col);
}

void lcd_putc(uint8_t chr){
	lcd_rs_set();
	lcd_byte(chr);
}
