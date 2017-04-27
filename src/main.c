#ifdef __AVR__
	#include <avr/io.h>
#endif

#include "interface.h"
#include "decode.h"
#include "menu.h"
#include "buffer_utils.h"

#include <stdint.h>

uint16_t remote_flash_size=128u;
uint16_t edit_addr;
uint16_t program[256];

#include "op_tree.h"
#include <stdio.h>

#define WALK_RIGHT  1
#define WALK_NONE   0
#define WALK_LEFT (-1)

#define IP_MAGIC 0xfeu

// More than op tree depth.
#define STACK_SIZE 15
void walk(uint16_t* ptr, uint8_t category){
	// This function is basically dfs with possibility to move sideways between
	// leaves, but rewritten into iterative form.
	typedef struct stack_entry{
		uintptr_t node;
		uint8_t i;
	} stack_entry;
	static stack_entry stack[STACK_SIZE];
	stack_entry* sp=stack-1;
	int8_t where=WALK_RIGHT;
	uint16_t cmask=0;

	while(where!=WALK_NONE){
		if(sp==stack-1){
			// We reached stack bottom, so we wrap around tree root.
			sp++;
			sp->node=NODE(op_node_root);
			sp->i=IP_MAGIC;
		}
		uintptr_t node=sp->node;
		if(IS_LEAF(node)){
			uint8_t name=NAME_FROM_IP(node);
			if(name==STRING_RESERVED){ /* Ignore node */ }
			else if(category!=CATEGORY_FROM_IP(node)){ /* Ignore node */ }
			else{
				decode(cmask, 0);
				print_buffer();
				while(1){
					uint8_t ui=poll_user_input();
					switch(ui){
					case A_LEFT:  where=WALK_LEFT;  break;  // opcode--
					case A_RIGHT: where=WALK_RIGHT; break;  // opcode++
					case B_PRESS: where=WALK_NONE;  break;  // exit cheat sheet
					case A_PRESS: *ptr=cmask;       return; // select opcode
					default:      pc_delay();       continue;
					}
					break;
				}
			}
			sp--;
			continue;
		}
		op_node* n=(op_node*)node;
		uint8_t bits=1;
		uint16_t mask=n->switchmask;
		uint8_t j=16;
		do {
			if(mask&1){
				bits<<=1;
			}
			mask>>=1;
		} while(j--);
		uint8_t i=sp->i;
		if(i==IP_MAGIC){
			i=0xffu;
			if(where==WALK_LEFT){
				i=bits;
			}
		}
		i+=where;
		if(i==0xffu || i==bits){
			sp--;
			continue;
		}
		mask=n->switchmask;
		cmask&=~mask;
		uint8_t k=i;
		for(uint16_t j=1; j; j<<=1){
			if(j&mask){
				if(k&1){
					cmask|=j;
				}
				k>>=1;
			}
		}
		sp->i=i;
		sp++;
		sp->node=n->next[i];
		sp->i=IP_MAGIC;
	}
}

void cheat_sheet(uint16_t* store_location){
	uint8_t category=show_menu(MENU_CATEGORY);
	walk(store_location, category);
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
			case A_LEFT:  { position++;       } break;
			case A_RIGHT: { position--;       } break;
			case B_PRESS: { edit_mode=0;      } break;
			case A_PRESS: { cheat_sheet(&op); } break;
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
			default: { pc_delay(); } break;
			}
			position&=3;
			program[edit_addr]=op;
		}
		else{
			switch(ui){
			case A_LEFT:  { edit_addr--;  } break;
			case A_RIGHT: { edit_addr++;  } break;
			case B_LEFT:  { edit_addr-=8; } break;
			case B_RIGHT: { edit_addr+=8; } break;
			case A_PRESS: { edit_mode=1;  } break;
			case B_PRESS: { return; }
			default:      { pc_delay();   } break;
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
	uint16_t ro=menu_ask16(STRING_READ_OFFSET);
	uint16_t wo=menu_ask16(STRING_WRITE_OFFSET);
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
		remote_flash_size=menu_ask16(STRING_PAGE_SIZE_IN_BYTES);
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

#include "hd44780.h"
int main(){
/*#ifdef __AVR__
	lcd_init();
	for(uint16_t i=0; ; i++){
		decode(i, 0);
		for(uint8_t* b=buffer; b!=buf; b++){
			if(*b==255){ while(1); }
			//lcd_putc(*b);
		}

		if(i%10==0){
			lcd_xy(1, 0);
			lcd_putc('0'+i/10000);
			lcd_putc('0'+(i%10000)/1000);
			lcd_putc('0'+(i%1000)/100);
			lcd_putc('0'+(i%100)/10);
			lcd_putc('0'+(i%10)/1);
		}
	}
#endif*/
	do_edit();
	//show_menu(MENU_SPLASH);
	//main_menu();
}
