#ifdef __AVR__
	#include <avr/io.h>
#else
	#include <stdio.h>
	#include <stdint.h>
	#include <unistd.h>
	#include <poll.h>
	#include <sys/time.h>
#endif

#include "common.h"
#include "compressed.h"
									
#define ARG_EOF 0
#define ARG_REG 1
#define ARG_HEXBYTE 2
#define ARG_HEXWORD 3
#define ARG_HEX3B 4
#define ARG_DECBYTE 5
#define ARG_OFFSET 6
#define ARG_RESERVED 7
#define ARG_MXP 8
#define ARG_YPQ 9

static uint8_t buffer[20];
static uint8_t* buf;

static void reset(){
	buf=buffer;
}

static void append(uint8_t c){
	*buf++=c;
}

static void skip(uint8_t many){
	buf+=many;
}

// Up to 9999.
static void append_decnum(uint16_t num){
	static uint8_t digs[4];
	uint8_t* dig=digs;
	do{
		int16_t remainder=num;
		num=0;
		while(1){
			remainder-=10;
			if(remainder<0){ break; }
			num++;
		}
		*dig++=remainder+10;
	} while(num);
	while(dig--!=digs){
		append(*dig +'0');
	}
}

static void append_hexnibble(uint8_t num){
	num&=0xfu;
	if(num<10){
		append('0'+num);
	}
	else{
		append(('a'-10)+num);
	}
}

typedef struct bit_state {
	uint8_t* ptr;
	uint8_t curbit;
} bit_state;

static uint8_t get_bit(bit_state* bs){
	uint8_t ret=((*bs->ptr)>>(bs->curbit))&1;
	bs->curbit=(bs->curbit+1)&7;
	if(bs->curbit==0){
		(bs->ptr)++;
	}
	return ret;
}

static uint8_t get_bits(uint8_t cnt, bit_state* bs){
	uint8_t ret=0;
	while(cnt--){
		ret*=2;
		ret|=get_bit(bs);
	}
	return ret;
}

static void append_arguments(uint8_t* arguments, uint16_t next);

static uint8_t next_string(uint8_t* op_type, bit_state* bs){
	reset();
	uint8_t skipped=get_bits(2, bs);
	skip(skipped);
	uint8_t len=get_bits(3, bs);
	if(op_type){
		*op_type=get_bits(4, bs);
	}
	uint8_t ls=len+skipped;
	if(ls==MAGIC_LEN_EOF){ return 0; }
	else if(ls==MAGIC_LEN_K4){ // Appears only in opcode decompression.
		len=1; // "des": compressed two bytes, one left.
		*op_type=OP_K4_CHR;
	}
	else if(ls==MAGIC_LEN_NULL_TERMINATED){
		len=255; // Wait until magic null terminator.
	}
	else if(ls==MAGIC_LEN_7B_NULL_TERMINATED){
		reset(); // Skip is meaningless.
		while(1){
			uint8_t bits=get_bits(7, bs);
			if(!bits){ return 1; }
			append(bits);
		}
	}
	while(len--){
		uint8_t bits=get_bits(5, bs);
		if(!bits){ break; }
		append(bits|0x60);
	}
	return 1;
}

static uint8_t check_opcode_match(uint8_t op_type, uint16_t op, bit_state* bs){
	uint8_t fail=0;
	uint16_t mask=type_masks[op_type];
	uint16_t op_tmp=op;
	for(uint8_t bit=0; bit<16; bit++){
		if(mask&1u){
			fail |= ( (op_tmp&1) ^ 
					get_bit(bs));
		}
		mask>>=1;
		op_tmp>>=1;
	}
	return !fail;
}

static void initialize_bit_streams(
		bit_state* compressed_op_bs, 
		bit_state* compressed_op_names
	){
	compressed_op_bs->ptr=compressed_op_bits;
	compressed_op_bs->curbit=0;

	compressed_op_names->ptr=compressed_name_bits;
	compressed_op_names->curbit=0;
}

static void decode(uint16_t op, uint16_t next){
	uint8_t arguments[16];
	uint8_t* args=arguments+sizeof(arguments);
	while(arguments!=args){
		*--args=ARG_EOF;
	}
	*args=ARG_RESERVED;

	uint8_t op_type;

	bit_state compressed_op_bs, compressed_op_names;
	initialize_bit_streams(&compressed_op_bs, &compressed_op_names);

	while(next_string(&op_type, &compressed_op_names)){
		if(check_opcode_match(op_type, op, &compressed_op_bs)){
			// Found match. Let's print the name first.
			uint8_t dreg=(op>>4)&0x1f;
			uint8_t reg=(op&0xf)|((op&0x0200u)>>5);

			*args=ARG_REG;
			switch(op_type){
				case OP_R5_Y_P_CHR:
				{
					uint8_t xyz=(op>>2u)&3u;
					uint8_t p=(op&3u);

					// This complicated condition is check for validity.
					if((p!=0u || xyz==3u) && xyz!=1u && p!=3u){
						// Possible formats:
						//             p st
						// st N+, rD | 1 1
						// st -N, rD | 2 1
						// st X, rD  | 0 1
						// ld rD, N+ | 1 0
						// ld rD, -N | 2 0
						// ld rD, X  | 0 0
						if(op&0x0200u){ // st
							*args++=ARG_MXP;
							*args++=p;
							*args++=xyz;
							*args++=ARG_REG;
							*args++=dreg;
						}
						else{ // ld
							*++args=dreg;
							*++args=ARG_MXP;
							*++args=p;
							*++args=xyz;
						}
					}
					else{
						*args++=ARG_RESERVED;
					}
				} break;
				case OP_R5_K16_CHR:
				{
					if(op&0x0200){ // sts
						*args++=ARG_HEXWORD;
						*args++=next>>8;
						*args++=next;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // lds
						*++args=dreg;
						*++args=ARG_HEXWORD;
						*++args=next>>8;
						*++args=next;
					}
				} break;
				case OP_Q_R5_CHR:
				{
					uint8_t q=(op&7)|((op&0x0c00u)>>7)|((op&0x2000u)>>8);
					char yz=((op>>3)&1);

					if(q){
						append('d');
					}
					if(op&0x0200u){ // st
						*args++=ARG_YPQ;
						*args++=yz;
						*args++=q;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // ld
						*++args=dreg;
						*++args=ARG_YPQ;
						*++args=yz;
						*++args=q;
					}
				} break;
				case OP_RD_D4_R4_CHR:
				case OP_D3_R3_CHR:
				case OP_D4_R4_CHR:
				{
					if(op_type==OP_D3_R3_CHR){
						dreg=(dreg&7)+16;
						reg = (reg&7)+16;
					}
					else if(op_type==OP_D4_R4_CHR){
						if(op&0x0100u){ // movw
							dreg=(dreg&0xf)*2;
							reg = (reg&0xf)*2;
						}
						else{ // muls
							dreg=(dreg&0xf)+16;
							reg = (reg&0xf)+16;
						}
					}
					*++args=dreg;
					*++args=ARG_REG;
					*++args=reg;
				} break;
				case OP_K6_R2_CHR:
				{
					*++args=(dreg&3)*2+24;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>2)&0x30);
				} break;
				case OP_K8_R4_CHR:
				{
					*++args=(dreg&0xf)+16;
					*++args=ARG_HEXBYTE;
					*++args=(op&0xf)|((op>>4)&0xf0);
				} break;
				case OP_R5_CHR:
				{
					*++args=dreg;
					if((op&0xfe0cu)==0x9004u){ // lpm/elpm Z(+)
						*++args=ARG_MXP;
						*++args=op&1;
						*++args=0;
					}
				} break;
				case OP_R5_B_CHR:
				{
					*++args=dreg;
					*++args=ARG_DECBYTE;
					*++args=op&7;
				} break;
				case OP_K12_CHR:
				{
					uint16_t k=op&0xfff;
					*args++=ARG_OFFSET;
					*args++=12;
					*args++=k>>8;
					*args++=k;
				} break;
				case OP_K7_CHR:
				{
					*args++=ARG_OFFSET;
					*args++=7;
					*args++=0;
					*args++=(op>>3)&0x7f;
				} break;
				case OP_K22_CHR:
				{
					*args++=ARG_HEX3B;
					*args++=(op&1)|((op>>3)&0x3e);
				} break;
				case OP_IO_B_CHR:
				{
					*args++=ARG_HEXBYTE;
					*args++=(op>>3)&0x1f;
					*args++=ARG_DECBYTE;
					*args++=op&7;
				} break;
				case OP_IO_R5_CHR:
				{
					uint8_t a=(op&0xf)|((op>>5)&0x30);
					if(op&0x0800u){ // out
						*args++=ARG_HEXBYTE;
						*args++=a;
						*args++=ARG_REG;
						*args++=dreg;
					}
					else{ // in
						*++args=dreg;
						*++args=ARG_HEXBYTE;
						*++args=a;
					}
				} break;
				case OP_CONST_CHR:
				{
					*args++=ARG_EOF;
				} break;
				case OP_K4_CHR:
				{
					*args++=ARG_DECBYTE;
					*args++=(op>>4)&0xfu;
				} break;
				default: __builtin_unreachable();
			}
			break;
		}
	}
	append_arguments(arguments, next);
}

static void append_arguments(uint8_t* arguments, uint16_t next){
	uint8_t* args=arguments;
	while(1){
		uint8_t byte=*args++;
		if(byte==0){ return; }
		if(args!=arguments+1){
			append(',');
		}
		append(' ');
		switch(byte){
			case ARG_HEXBYTE:
			case ARG_HEXWORD:
			{
				append('0');
				append('x');
				uint8_t bytes=byte-(ARG_HEXBYTE-1);
				while(bytes--){
					uint8_t num=*args++;
					append_hexnibble(num>>4);
					append_hexnibble(num);
				}
			} break;
			case ARG_HEX3B:
			{
				uint8_t bytes[3];
				bytes[0]=((*args++)<<1)|(next>>15);
				bytes[1]=(next&0x07f80u)>>7;
				bytes[2]=(next&0x7f)<<1;
				append('0');
				if(bytes[0] || bytes[1] || bytes[2]){
					append('x');
					uint8_t any=0;
					for(uint8_t nib=0; nib<6; nib++){
						uint8_t x=bytes[nib/2];
						if((nib&1) == 0){ x>>=4; }
						any|=x;
						if(any){
							append_hexnibble(x);
						}
					}
				}
			} break;
			case ARG_REG:
				append('r');
			case ARG_DECBYTE: // Fallthrough.
				append_decnum(*args++);
				break;
			case ARG_OFFSET:
			{
				append('.');
				uint8_t bits=*args++;
				uint16_t k=*args++;
				k=(k<<8)|*args++;
				if(k&(1u<<(bits-1))){
					append('-');
					k=(1u<<bits)-k;
				}
				else{
					append('+');
				}
				append_decnum(k*2);
			} break;
			case ARG_RESERVED:
			{
				reset();
				for(const char* c="[reserved]"; *c; c++){
					append(*c);
				}
			} break;
			case ARG_MXP:
			{
				uint8_t p=*args++;
				uint8_t xyz=*args++;
				xyz=('X'+3)-xyz-!xyz;
				if(p==2){ append('-'); }
				append(xyz);
				if(p==1){ append('+'); }
			} break;
			case ARG_YPQ:
			{
				uint8_t yz='Z'-*args++;
				uint8_t q=*args++;
				append(yz);
				if(q){
					append('+');
					append_decnum(q);
				}
			} break;
			default: __builtin_unreachable();
		}
	}
}

#define A_LEFT  1
#define A_RIGHT 2
#define A_PRESS 3

#define B_LEFT  4
#define B_RIGHT 5
#define B_PRESS 6

#ifndef __AVR__
typedef struct timeval timer;
static void select_display_line(uint8_t line){
	if(line==0){ for(int i=0; i<100; i++) printf("\n"); }
	else{ printf("\n"); }
}
static void put_character(uint8_t c){
	printf("%c", c); fflush(stdout);
}
static uint8_t poll_user_input(){
	while(1){
		struct pollfd fds;
		fds.fd=STDIN_FILENO;
		fds.events=POLLIN;
		while(poll(&fds, 1, 0)){
			char c[1];
			read(STDIN_FILENO, c, 1);
			switch(c[0]){
			case 'a': return A_LEFT;
			case 'd': return A_RIGHT;
			case 's': return A_PRESS;
			case 'q': return B_LEFT;
			case 'e': return B_RIGHT;
			case 'w': return B_PRESS;
			};
		}
		return 0;
	}
}
static void start_timer(timer* t){
	gettimeofday(t, NULL);
}
static uint16_t elapsed_time(timer* t){
	timer t2;
	gettimeofday(&t2, NULL);
	return (t2.tv_sec-t->tv_sec)*1000+(t2.tv_usec-t->tv_usec)/1000;
}
int usleep (__useconds_t __useconds);
static void small_delay(){
	usleep(100*1000);
}
#else
// Stubs for now.
typedef struct timer{uint8_t x;} timer;
static void select_display_line(uint8_t line){
	DDRB=line;
}
static void put_character(uint8_t c){
	DDRB=c;
}
static uint8_t poll_user_input(){
	return DDRB;
}
static void start_timer(timer* t){
	t->x=DDRB;
}
static uint16_t elapsed_time(timer* t){
	return t->x+DDRB;
}
static void small_delay(){
}
#endif

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

static void load_string(uint8_t id){
	reset();
	bit_state bs;
	bs.ptr=compressed_string_bits;
	bs.curbit=0;
	do {
		next_string(0, &bs);
	} while(id--);
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

static void themain(){
	show_menu(MENU_SPLASH);
	main_menu();
}

int main(){
#ifdef TEST
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
#endif
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
	themain();
}
