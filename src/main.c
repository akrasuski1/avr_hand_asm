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
#include "gen/comp_name_bits.h"
void cheat_sheet(uint16_t* store_location){
	uint8_t index=0;
	while(1){
		compressed_ops_names bs;
		init_ops_names(&bs);

		uint8_t op_type;
		uint16_t op;
		uint8_t i=index;
		do {
			next_string(&op_type, &bs.compressed_op_names);
			uint16_t mask=type_masks[op_type];
			op=get_op_mask(&bs.compressed_op_bs, mask);
		} while(i--);
		select_display_line(0);
		decode(op, 0);
		print_buffer();
		select_display_line(1);
		reset();
		uint16_t mask=type_masks[op_type];
		for(uint8_t bit=0; bit<16u; bit++){
			if(mask&0x8000u){
				if(op&0x8000u){
					append('1');
				}
				else{
					append('0');
				}
			}
			else{
				append('*');
			}
			if((bit&3)==3){
				append(' ');
			}
			op<<=1;
			mask<<=1;
		}
		print_buffer();
		uint8_t ui=poll_user_input();
		switch(ui){
		case A_LEFT: 
		case A_RIGHT:
		{
			index+=(ui-A_LEFT)*2-1;
		} break;
		case B_LEFT: 
		case B_RIGHT:
		{
			index+=((ui-B_LEFT)*2-1)*8;
		} break;
		case B_PRESS:
			*store_location=op;
			// Fallthrough.
		case A_PRESS:
			return;
		default:
		{
			pc_delay();
		} break;
		}
		// Modulo.
		index+=OP_NAMES_NUM;
		if(index>=OP_NAMES_NUM){ index-=OP_NAMES_NUM; }
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
		{
			uint16_t twice=2*edit_addr;
			append_hexbyte(twice>>8);
			append_hexbyte(twice);
		}
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
		print_buffer();
		select_display_line(1);
		decode(op, next);
		print_buffer();
		uint8_t ui=poll_user_input();
		if(edit_mode){
			blink_cursor(((position<2)?10:9)-position);
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
			case B_PRESS:
			{
				edit_mode=1;
			} break;
			case A_PRESS:
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
