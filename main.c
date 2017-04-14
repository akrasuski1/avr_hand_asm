#ifdef __AVR__
	#include <avr/io.h>
#endif

#include "interface.h"
#include "decode.h"
#include "menu.h"

#include <stdint.h>

uint16_t remote_flash_size=128u;
uint16_t edit_addr;
uint16_t program[256];

#include "gen/comp_op_bits.h"
#include "gen/comp_cat_bits.h"
#include "gen/comp_name_bits.h"
void cheat_sheet(uint16_t* store_location){
	uint8_t op_cat=show_menu(MENU_OPCODE_TYPE);
	uint8_t index=0;
	while(1){
		uint8_t i=index;
		bit_state cat_bs, compressed_op_bs, compressed_op_names;

		cat_bs.ptr=compressed_category_bits;
		cat_bs.curbit=0;

		compressed_op_bs.ptr=compressed_op_bits;
		compressed_op_bs.curbit=0;

		compressed_op_names.ptr=compressed_name_bits;
		compressed_op_names.curbit=0;

		uint8_t op_type;
		uint16_t op=0;
		uint16_t mask;
		while(next_string(&op_type, &compressed_op_names)){
			uint8_t cat=get_bits(2, &cat_bs);
			mask=type_masks[op_type];
			for(uint8_t bit=0; bit<16; bit++){
				op>>=1;
				if(mask&1u){
					if(get_bit(&compressed_op_bs)){
						op|=0x8000u;
					}
				}
				mask>>=1;
			}
			if(cat==op_cat){
				if(i--==0){
					mask=type_masks[op_type];
					break;
				}
			}
		}
		select_display_line(0);
		decode(op, 0);
		print_buffer(MOD_NONE);
		select_display_line(1);
		uint8_t ui=poll_user_input();
		switch(ui){
		case A_LEFT: 
		{
			index--;
		} break;
		case A_RIGHT:
		{
			index++;
		} break;
		case A_PRESS:
		{
			return;
		} break;
		case B_PRESS:
		{
			*store_location=op;
			return;
		} break;
		default:
		{
			pc_delay();
		} break;
		}
	}
}

void do_edit(){
	for(uint8_t i=0; i<254u; i++){
		program[i]=(i<<8)|i;
	}
	uint8_t edit_mode=0;
	uint8_t position=0;
	while(1){
		uint16_t op=program[edit_addr];
		uint16_t next=program[edit_addr+1];
		select_display_line(0);
		reset();
		append_hexbyte((2*edit_addr)>>8);
		append_hexbyte(2*edit_addr);
		append(':');
		append(' ');
		append_hexbyte(op>>8);
		append(' ');
		append_hexbyte(op);
		append(' ');
		append('[');
		append_hexbyte(next>>8);
		append(' ');
		append_hexbyte(next);
		append(']');
		print_buffer(MOD_NONE);
		select_display_line(1);
		decode(op, next);
		print_buffer(MOD_NONE);
		uint8_t ui=poll_user_input();
		if(edit_mode){
			blink_cursor(9-position);
			uint8_t raw_nibble=(op>>(4*position))&0xfu;
			uint16_t add=1u<<(4*position);
			switch(ui){
			case A_LEFT: 
			{
				position++;
			} break;
			case A_RIGHT:
			{
				position--;
			} break;
			case B_LEFT:
			{
				op-=add;
				if(raw_nibble==0){
					op+=add<<4;
				}
			} break;
			case B_RIGHT:
			{
				op+=add;
				if(raw_nibble==0xf){
					op-=add<<4;
				}
			} break;
			case A_PRESS:
			{
				edit_mode=0;
			} break;
			case B_PRESS:
			{
				cheat_sheet(&op);
			} break;
			default:
			{
				pc_delay();
			} break;
			}
			position&=3;
			program[edit_addr]=op;
		}
		else{
			switch(ui){
			case A_LEFT: 
			{
				edit_addr--;
			} break;
			case A_RIGHT:
			{
				edit_addr++;
			} break;
			case B_LEFT:
			{
				edit_addr-=8;
			} break;
			case B_RIGHT:
			{
				edit_addr+=8;
			} break;
			case A_PRESS:
			{
				edit_mode=1;
			} break;
			case B_PRESS:
			{
				return;
			} break;
			default:
			{
				pc_delay();
			} break;
			}
		}
	}
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
	if(sure==0){ // Yes.
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

void menu_config(){
	uint8_t choice=show_menu(MENU_DEVICE);
	if(choice<3u){
		static uint16_t sizes[]={128u, 64u, 32u};
		remote_flash_size=sizes[choice];
	}
	else{ // Custom size.
		remote_flash_size=menu_ask16(STRING_PAGE_SIZE);
	}
}

void main_menu(){
	while(1){
		uint8_t choice=show_menu(MENU_MAIN_MENU);
		switch(choice){
		case 0: { do_edit();     } break;
		case 1: { run();         } break;
		case 2: { menu_move();   } break;
		case 3: { menu_config(); } break;
		default: __builtin_unreachable();
		}
	}
}

int main(){
	show_menu(MENU_SPLASH);
	main_menu();
}
