#include "menu.h"

#include "interface.h"
#include "common.h"
#include "buffer_utils.h"
#include "decompression.h"

void uppercase_buffer(){
	*buffer-=0x20; // Just the first letter.
}

void print_buffer(){
	for(const uint8_t* c=buffer; c!=buf; c++){
		uint8_t d=*c;
		if(d>'z' && d<='\x7f'){ // The six special expanded characters.
			for(const char* cc=short_strings[d-('z'+1)]; *cc; cc++){
				put_character(*cc);
			}
		}
		else{
			put_character(d);
		}
	}
}

#define MENU_MAGIC_INCREMENT_OPTIONS 0xffu
static uint8_t menus[]={          /* 1-option menus */
	STRING_COOL_AVR_PROGRAMMER, STRING_CONTINUE, // MENU_SPLASH

	MENU_MAGIC_INCREMENT_OPTIONS, /* 2-option menus */

	STRING_OPERATION, STRING_LOAD, STRING_STORE, // MENU_LOAD_STORE
	STRING_TYPE, STRING_FLASH, STRING_EEPROM, // MENU_FLASH_EEPROM
	STRING_ARE_YOU_SURE, STRING_YES, STRING_NO, // MENU_ARE_YOU_SURE

	MENU_MAGIC_INCREMENT_OPTIONS, /* 3-option menus */

	STRING_LOCATION, STRING_LOCAL, STRING_REMOTE, STRING_SD, // MENU_LOCATION

	MENU_MAGIC_INCREMENT_OPTIONS, /* 4-option menus */

	STRING_MAIN_MENU, STRING_EDIT, STRING_RUN, STRING_MOVE, STRING_CONFIG, // MENU_MAIN_MENU
	STRING_DEVICE, STRING_M32, STRING_M8, STRING_T13, STRING_OTHER, // MENU_DEVICE
	STRING_CATEGORY, STRING_ARITH, STRING_JUMP, STRING_MOVE, STRING_MISC, // MENU_CATEGORY
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
	uint8_t sel=0;
	while(1){
		uint8_t* m=menu;
		select_display_line(0);
		load_string(*m++);
		uppercase_buffer();
		print_buffer();
		select_display_line(1);
		uint8_t cursor=sel;
		for(uint8_t i=0; i<options_cnt; i++){
			if(i){
				put_character(' ');
			}
			load_string(*m++);
			uppercase_buffer();
			print_buffer();
			if(i<sel){
				cursor+=buf-buffer;
			}
		}
		blink_cursor(cursor);
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
		pc_delay();
	}
}

uint8_t update_menu16(update_menu16_arg* arg){
	select_display_line(1);
	reset();
	append('0');
	append('x');
	uint16_t ch=arg->val;
	uint8_t selected_nibble=0;
	uint8_t pos=arg->position;
	for(uint8_t i=3; i!=0xffu; i--){
		uint8_t nib=(ch>>12)&0xf;
		ch<<=4;
		if(i==pos){
			selected_nibble=nib;
		}
		append_hexnibble(nib); 
	}
	print_buffer();
	blink_cursor(5-pos);
	uint16_t p=1u<<(pos*4);
	uint8_t ui=poll_user_input();
	uint16_t op=arg->val;
	switch(ui){
	case A_LEFT:  { pos++; } break;
	case A_RIGHT: { pos--; } break;
	case B_LEFT:
	{
		op-=p;
		if(selected_nibble==0){
			op+=p<<4;
		}
	} break;
	case B_RIGHT:
	{
		op+=p;
		if(selected_nibble==0xf){
			op-=p<<4;
		}
	} break;
	default: return ui;
	}
	arg->val=op;
	arg->position=pos&3;
	return 0;
}

uint16_t menu_ask16(uint8_t id){
	update_menu16_arg arg;
	arg.val=0;
	arg.position=0;
	select_display_line(0);
	load_string(id);
	uppercase_buffer();
	print_buffer();
	while(1){
		uint8_t ui=update_menu16(&arg);
		switch(ui){
		case A_PRESS: { return arg.val; } break;
		default:      { pc_delay();    } break;
		}
	}
}
