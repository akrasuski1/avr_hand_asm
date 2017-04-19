#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../src/common.h"
#include "full_tables.h"

#define PARTITION_SIZE 8

void print_all(uint8_t* from, uint8_t* to, FILE* f){
	for(int i=0; i<(to-from); i+=8){
		if(i && i%128==0){
			fprintf(f, "\n\t");
		}
		uint8_t a=0;
		for(int j=0; j<8; j++){
			a|=from[i+j]<<j;
		}
		fprintf(f, "0x%02x,", a);
	}
	fprintf(f, "\n};\n");
}

void print_all_bytes(uint8_t* from, uint8_t* to, FILE* f){
	for(int i=0; i<(to-from); i++){
		if(i && i%16==0){
			fprintf(f, "\n\t");
		}
		uint8_t a=from[i];
		fprintf(f, "0x%02x,", a);
	}
	fprintf(f, "\n};\n");
}

void store(uint8_t** where, uint8_t what, uint8_t bits){
	while(bits--){
		**where=(what>>bits)&1;
		(*where)++;
	}
}

char* strdup(const char*); // Stupid clang doesn't see strdup.
void save_file(uint8_t* from, uint8_t* to, const char* tablename, const char* fname,
	   	uint8_t* skiptable, uint8_t* skipend){
	FILE* f=fopen(fname, "w");
	fprintf(f, "#include \"../progmem_utils.h\"\n");
	fprintf(f, "#include <stdint.h>\n");
	fprintf(f, "// %zu bits\n", to-from);
	fprintf(f, "const uint8_t PROGMEM %s[]={\n\t", tablename);
	print_all(from, to, f);
	fprintf(f, "const uint8_t PROGMEM %s_skiptable[]={\n\t", tablename);
	print_all_bytes(skiptable, skipend, f);
	fclose(f);
	char* fn=strdup(fname);
	fn[strlen(fn)-1]='h';
	f=fopen(fn, "w");
	fprintf(f, "#include \"../progmem_utils.h\"\n");
	fprintf(f, "#define PARTITION_SIZE %d\n", PARTITION_SIZE);
	fprintf(f, "extern const uint8_t PROGMEM %s[];\n", tablename);
	fprintf(f, "extern const uint8_t PROGMEM %s_skiptable[];\n", tablename);
	fclose(f);
}

void make_enum(const char* fname){
	FILE* f=fopen(fname, "w");
	fprintf(f, "enum{\n");
	for(const char** pstr=strings; *pstr; pstr++){
		fprintf(f, "\tSTRING_");
		for(const char* str=*pstr; *str; str++){
			char c=*str;
			if(c>='a' && c<='z'){ c-=0x20; }
			if(c>='A' && c<='Z'){ fprintf(f, "%c", c); }
			else if(c>='0' && c<='9'){ fprintf(f, "%c", c); }
			else{ fprintf(f, "_"); }
		}
		fprintf(f, ",\n");
	}
	fprintf(f, "};");
	fclose(f);
}

int main(){
	uint8_t str_bits[10000];
	uint8_t* strbit=str_bits;
	uint8_t skip_n[1000];
	uint8_t* skip_nptr=skip_n;

	char prevop[100]={};
	int i=0;
	int last=0;
	for(const char** pstr=strings; *pstr; pstr++){
		if(i>0 && i%PARTITION_SIZE==0){
			while( (strbit-str_bits)%8 ){
				store(&strbit, 0, 1);
			}
			prevop[0]=0;
			*skip_nptr=(strbit-str_bits)/8-last;
			last+=*skip_nptr;
			skip_nptr++;
		}
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
		strcpy(prevop, str);
		if(type==1){
			store(&strbit, 0, 5);
		}
		else if(type==2){
			store(&strbit, 0, 7);
		}
		i++;
	}

	save_file(str_bits, strbit, "compressed_string_bits",
		   	"src/gen/comp_str_bits.c", skip_n, skip_nptr);
	make_enum("src/gen/strings.h");
}
