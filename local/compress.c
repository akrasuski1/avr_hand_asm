#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../src/common.h"
#include "full_tables.h"

#define IS_METADATA(c) ((c)<=OP_LAST)

void print_all(uint8_t* from, uint8_t* to, FILE* f){
	for(int i=0; i<(to-from); i+=8){
		if(i && i%128==0){
			fprintf(f, "\"\n\t\"");
		}
		uint8_t a=0;
		for(int j=0; j<8; j++){
			a|=from[i+j]<<j;
		}
		fprintf(f, "\\x%02x", a);
	}
	fprintf(f, "\";\n");
}

void store(uint8_t** where, uint8_t what, uint8_t bits){
	while(bits--){
		**where=(what>>bits)&1;
		(*where)++;
	}
}

void save_file(uint8_t* curbit, uint8_t* all_bits, const char* tablename, const char* fname){
	FILE* f=fopen(fname, "w");
	fprintf(f, "#include \"../progmem_utils.h\"\n");
	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "// %zu bits\n", curbit-all_bits);
	fprintf(f, "const uint8_t PROGMEM %s[]=\n\t\"", tablename);
	print_all(all_bits, curbit, f);
	fclose(f);
	char* fn=strdup(fname);
	fn[strlen(fn)-1]='h';
	f=fopen(fn, "w");
	fprintf(f, "#include \"../progmem_utils.h\"\n");
	fprintf(f, "extern const uint8_t PROGMEM %s[];\n", tablename);
}

int main(){
	uint8_t all_bits[10000];
	uint8_t* curbit=all_bits;

	uint8_t name_bits[10000];
	uint8_t* namebit=name_bits;

	uint8_t cat_bits[10000];
	uint8_t* catbit=cat_bits;

	uint8_t str_bits[10000];
	uint8_t* strbit=str_bits;

	uint8_t curop[100];
	uint8_t prevop[100]={};

	uint8_t* ptr=op_names;
	for(uint8_t i=0; i<OP_NAMES_NUM; i++){
		uint8_t op_type=*ptr++;
		uint16_t mask=type_masks[op_type];
		uint16_t opcode=op_bits[i];
		for(int b=0; b<16; b++){
			if((1<<b) & mask){
				*curbit++=!!( (1<<b) & opcode );
			}
		}
		uint8_t len=0;
		while( !IS_METADATA(*ptr) ){
			curop[len++]=*ptr++;
		}
		curop[len]=0;

		uint8_t skip=0;
		while(1){
			if(curop[skip] && curop[skip]==prevop[skip] && skip<3){
				skip++;
			}
			else{
				break;
			}
		}
		uint8_t sent_len=len-skip;
		// This is op_type 16, not enough place in 4 bits.
		if(op_type==OP_K4_CHR){ 
			sent_len=MAGIC_LEN_K4-skip; 
			op_type=0; 
			// Length is implied 3, for "des"
		}
		store(&namebit, skip, 2);
		store(&namebit, sent_len, 3);
		store(&namebit, op_type, 4);
		for(uint8_t j=skip; j<len; ){
			store(&namebit, curop[j]-0x60, 5);
			j++;
		}

		for(uint8_t j=0; j<=len; j++){
			prevop[j]=curop[j];
		}
		store(&catbit, op_categories[i], 2);
	}
	// 3+7==10==MAGIC_LEN_EOF
	store(&namebit, 3, 2);
	store(&namebit, 7, 3);

	prevop[0]=0;
	for(const char** pstr=strings; *pstr; pstr++){
		const char* str=*pstr;
		int len=strlen(str);
		uint8_t skip=0;
		while(1){
			if(str[skip] && str[skip]==prevop[skip] && skip<3){
				skip++;
			}
			else{
				break;
			}
		}
		uint8_t sent_len=len-skip;
		uint8_t type=0;
		if(len>6){
			sent_len=MAGIC_LEN_NULL_TERMINATED-skip;
			type=1;
		}
		uint8_t normal_string=1;
		for(const char* c=str; *c; c++){
			if( (*c>='a' && *c<='\x7f') ){ /* ok */ }
			else{
				normal_string=0;
			}
		}
		if(!normal_string){
			skip=1;
			sent_len=MAGIC_LEN_7B_NULL_TERMINATED-1;
			type=2;
		}

		store(&strbit, skip, 2);
		store(&strbit, sent_len, 3);
		if(!normal_string){ skip=0; }
		for(const char* c=str+skip; *c; c++){
			if(type==2){
				store(&strbit, *c, 7);
			}
			else{
				store(&strbit, *c-0x60, 5);
			}
		}
		if(type==1){
			store(&strbit, 0, 5);
		}
		else if(type==2){
			store(&strbit, 0, 7);
		}
	}

	// save_file(curbit, all_bits, "compressed_op_bits", "src/gen/comp_op_bits.c");
	// save_file(namebit, name_bits, "compressed_name_bits", "src/gen/comp_name_bits.c");
	// save_file(catbit, cat_bits, "compressed_category_bits", "src/gen/comp_cat_bits.c");
	save_file(strbit, str_bits, "compressed_string_bits", "src/gen/comp_str_bits.c");
}
