#ifdef __AVR__
	#include <avr/io.h>
#else
	#include <stdio.h>
#endif

#include "common.h"
#include "interface.h"
#include "buffer_utils.h"
#include "decode.h"
#include "menu.h"

#include <stdint.h>

void do_edit(){
	
}

void run(){
	// jmp to reset vector.
}

void store_local_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('S');
	put_character('L');
	put_character('E');
	select_display_line(write_offset);
}
void load_local_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('L');
	put_character('L');
	put_character('E');
	select_display_line(write_offset);
}
void store_local_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('S');
	put_character('L');
	put_character('F');
	select_display_line(write_offset);
}
void load_local_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('L');
	put_character('L');
	put_character('F');
	select_display_line(write_offset);
}
void store_remote_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('S');
	put_character('R');
	put_character('E');
	select_display_line(write_offset);
}
void load_remote_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('L');
	put_character('R');
	put_character('E');
	select_display_line(write_offset);
}
void store_remote_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('S');
	put_character('R');
	put_character('F');
	select_display_line(write_offset);
}
void load_remote_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('L');
	put_character('R');
	put_character('F');
	select_display_line(write_offset);
}
void store_sd(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('S');
	put_character('S');
	select_display_line(write_offset);
}
void load_sd(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset);
	put_character('L');
	put_character('S');
	select_display_line(write_offset);
}

void menu_move(){
	uint8_t load_store=show_menu(MENU_LOAD_STORE);
	uint8_t location=show_menu(MENU_LOCATION);
	uint8_t flash_eeprom=0;
	if(location!=2){ // Local or remote.
		flash_eeprom=show_menu(MENU_FLASH_EEPROM);
	}
	uint16_t ro=menu_ask16(STRING_READ_OFF);
	uint16_t wo=menu_ask16(STRING_WRITE_OFF);
	uint8_t sure=show_menu(MENU_ARE_YOU_SURE);
	if(sure){
		switch(location){
		case 0: // Local.
		{
			if(flash_eeprom){
				if(load_store){ store_local_eeprom(ro, wo); }
				else{ load_local_eeprom(ro, wo); }
			}
			else{
				if(load_store){ store_local_flash(ro, wo); }
				else{ load_local_flash(ro, wo); }
			}
		} break;
		case 1: // Remote.
		{
			if(flash_eeprom){
				if(load_store){ store_remote_eeprom(ro, wo); }
				else{ load_remote_eeprom(ro, wo); }
			}
			else{
				if(load_store){ store_remote_flash(ro, wo); }
				else{ load_remote_flash(ro, wo); }
			}
		} break;
		case 2: // SD.
		{
			if(load_store){ store_sd(ro, wo); }
			else{ load_sd(ro, wo); }
		} break;
		default: __builtin_unreachable();
		}
	}
}

uint16_t remote_flash_size=128;
void menu_config(){
	uint8_t choice=show_menu(MENU_DEVICE);
	switch(choice){
	case 0:
	case 1:
	case 2:
	{
		static uint16_t sizes[]={128, 64, 32};
		remote_flash_size=sizes[choice];
	} break;
	case 3:
	{
		remote_flash_size=menu_ask16(STRING_PAGE_SIZE);
	} break;
	default: __builtin_unreachable();
	}
}

void main_menu(){
	while(1){
		uint8_t choice=show_menu(MENU_MAIN_MENU);
		switch(choice){
		case 0: 
		{
			do_edit();
		} break;
		case 1: 
		{
			run();
		} break;
		case 2: 
		{
			menu_move();
		} break;
		case 3: 
		{
			menu_config();
		} break;
		default: __builtin_unreachable();
		}
	}
}

int main(){
#ifdef __AVR__
	uint16_t ctr=12345;
	for(uint16_t i=0; i!=0xfffeu; i++){
		ctr*=3;
		decode(i, i|ctr);
		// Avoid optimizing out.
		DDRB=*buf;
		DDRB=buf[1];
	}
#endif
	show_menu(MENU_SPLASH);
	main_menu();
}
