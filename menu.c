#include "menu.h"

#include "interface.h"
#include "common.h"
#include "buffer_utils.h"
#include "decompression.h"

void print_buffer(uint8_t size_change){
	for(const uint8_t* c=buffer; c!=buf; c++){
		uint8_t d=*c;
		if(d>'z' && d<='\x7f'){ // The six special expanded characters.
			for(const char* cc=short_strings[d-('z'+1)]; *cc; cc++){
				put_character(*cc);
			}
		}
		else{
			if(size_change==MOD_UPPERCASE && d>='a' && d<='z'){
			   	d-=0x20; 
			}
			put_character(d);
		}
	}
}

#define MENU_MAGIC_INCREMENT_OPTIONS 0xffu
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

uint8_t show_menu(uint8_t menu_index){
	uint8_t* menu=menus;
	uint8_t options_cnt=2; // For now, including header.
	while(menu_index--){
		menu+=options_cnt;
		// This should normally be "while", but "if" works too, since
		// there is at least one menu of each size.
		if(*menu==MENU_MAGIC_INCREMENT_OPTIONS){
			options_cnt++;
			menu++;
		}
	};
	// Actual option count does not include header.
	options_cnt--;
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

#define BLINK_DELAY 500
uint16_t menu_ask16(uint8_t id){
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
		do {
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
		} while(elapsed_time(&t)<BLINK_DELAY);
		blink^=1;
	}
}
