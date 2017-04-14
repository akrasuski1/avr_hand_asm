#ifdef __AVR__
	#include <avr/io.h>
#else
	#include <stdio.h>
#endif

#include "common.h"
#include "compressed.h"
#include "interface.h"
#include "buffer_utils.h"
#include "decompression.h"
#include "decode.h"

#include <stdint.h>

#define MOD_NONE 0
#define MOD_UPPERCASE 1
static void print_buffer(uint8_t size_change){
	for(const uint8_t* c=buffer; c!=buf; c++){
		uint8_t d=*c;
		if(d>'z' && d<='\x7f'){
			for(const char* cc=short_strings[d-('z'+1)]; *cc; cc++){
				put_character(*cc);
			}
		}
		else{
			if(size_change==MOD_UPPERCASE && d>='a' && d<='z'){ d-=0x20; }
			put_character(d);
		}
	}
}

#define MENU_MAGIC_INCREMENT_OPTIONS 0xffu
enum {
	MENU_SPLASH,
	MENU_LOAD_STORE,
	MENU_FLASH_EEPROM,
	MENU_ARE_YOU_SURE,
	MENU_LOCATION,
	MENU_MAIN_MENU,
	MENU_DEVICE
};
static uint8_t menus[]={          /* 1-option menus */
	STRING_SPLASH, STRING_CONTINUE, // MENU_SPLASH

	MENU_MAGIC_INCREMENT_OPTIONS, /* 2-option menus */

	STRING_OPERATION, STRING_LOAD, STRING_STORE, // MENU_LOAD_STORE
	STRING_TYPE, STRING_FLASH, STRING_EEPROM, // MENU_FLASH_EEPROM
	STRING_ARE_YOU_SURE, STRING_YES, STRING_NO, // MENU_ARE_YOU_SURE

	MENU_MAGIC_INCREMENT_OPTIONS, /* 3-option menus */

	STRING_LOCATION, STRING_LOCAL, STRING_REMOTE, STRING_SD, // MENU_LOCATION

	MENU_MAGIC_INCREMENT_OPTIONS, /* 4-option menus */

	STRING_MAIN_MENU, STRING_EDIT, STRING_RUN, STRING_MOVE, STRING_CONFIG, // MENU_MAIN_MENU
	STRING_DEVICE, STRING_M32, STRING_M8, STRING_T13, STRING_OTHER, // MENU_DEVICE
};

static uint8_t show_menu(uint8_t menu_index){
	uint8_t* menu=menus;
	uint8_t options_cnt=1;
	while(menu_index--){
		menu+=options_cnt+(uint8_t)1u;
		while(*menu==MENU_MAGIC_INCREMENT_OPTIONS){
			options_cnt++;
			menu++;
		}
	};
	int8_t sel=0;
	while(1){
		uint8_t* m=menu;
		select_display_line(0);
		load_string(*m++);
		print_buffer(MOD_UPPERCASE);
		select_display_line(1);
		for(uint8_t i=0; i<options_cnt; i++){
			put_character(sel==i ? '~' : ' '); // Tilde is right arrow
			load_string(*m++);
			print_buffer(MOD_UPPERCASE);
		}
		uint8_t ui=poll_user_input();
		switch(ui){
		case A_LEFT: 
		{
			if(sel==0){ sel=options_cnt; }
			sel--;
		} break;
		case A_RIGHT: 
		{
			sel++;
			if(sel==options_cnt){ sel=0; }
		} break;
		case A_PRESS: 
		{
			return sel;
		} break;
		};
		small_delay();
	}
}

static void run(){
	// jmp to reset vector.
}

#define BLINK_DELAY 500
static uint16_t menu_ask16(uint8_t id){
	uint16_t choice=0;
	uint8_t position=0;
	uint8_t blink=0;
	while(1){
		select_display_line(0);
		load_string(id);
		print_buffer(MOD_UPPERCASE);
		select_display_line(1);
		put_character('0');
		put_character('x');
		reset();
		uint16_t ch=choice;
		uint8_t selected_nibble=0;
		for(uint8_t i=3; i!=0xffu; i--){
			uint8_t nib=(ch>>12)&0xf;
			uint8_t draw=1;
			if(i==position){
				selected_nibble=nib;
				if(blink){
					append('_');
					draw=0;
				}
			}
			if(draw){
			   	append_hexnibble(nib); 
			}
			ch<<=4;
		}
		print_buffer(MOD_NONE);
		timer t;
		start_timer(&t);
		while(elapsed_time(&t)<BLINK_DELAY){
			uint8_t ui=poll_user_input();
			uint16_t p=1u<<(position*4);
			switch(ui){
			case A_LEFT: 
			{
				position++;
				position&=3;
			} break;
			case A_RIGHT:
			{
				position--;
				position&=3;
			} break;
			case B_LEFT:
			{
				choice-=p;
				if(selected_nibble==0){
					choice+=p<<4;
				}
			} break;
			case B_RIGHT:
			{
				choice+=p;
				if(selected_nibble==0xf){
					choice-=p<<4;
				}
			} break;
			case A_PRESS:
			{
				return choice;
			} break;
			default:
			{
				small_delay();
			} break;
			}
		}
		blink^=1;
	}
}

static void do_edit(){
	
}

static void store_local_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('S');
	put_character('L');
	put_character('E');
	select_display_line(write_offset-write_offset+1);
}
static void load_local_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('L');
	put_character('L');
	put_character('E');
	select_display_line(write_offset-write_offset+1);
}
static void store_local_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('S');
	put_character('L');
	put_character('F');
	select_display_line(write_offset-write_offset+1);
}
static void load_local_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('L');
	put_character('L');
	put_character('F');
	select_display_line(write_offset-write_offset+1);
}
static void store_remote_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('S');
	put_character('R');
	put_character('E');
	select_display_line(write_offset-write_offset+1);
}
static void load_remote_eeprom(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('L');
	put_character('R');
	put_character('E');
	select_display_line(write_offset-write_offset+1);
}
static void store_remote_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('S');
	put_character('R');
	put_character('F');
	select_display_line(write_offset-write_offset+1);
}
static void load_remote_flash(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('L');
	put_character('R');
	put_character('F');
	select_display_line(write_offset-write_offset+1);
}
static void store_sd(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('S');
	put_character('S');
	select_display_line(write_offset-write_offset+1);
}
static void load_sd(uint16_t read_offset, uint16_t write_offset){
	select_display_line(read_offset-read_offset);
	put_character('L');
	put_character('S');
	select_display_line(write_offset-write_offset+1);
}

static void menu_move(){
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
static void menu_config(){
	uint8_t choice=show_menu(MENU_DEVICE);
	switch(choice){
	case 0:
	case 1:
	case 2:
	{
		static uint8_t sizes[]={128, 64, 32};
		remote_flash_size=sizes[choice];
	} break;
	case 3:
	{
		remote_flash_size=menu_ask16(STRING_PAGE_SIZE);
	} break;
	default: __builtin_unreachable();
	}
}

static void main_menu(){
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
