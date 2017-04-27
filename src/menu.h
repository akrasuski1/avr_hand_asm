#ifndef MENU_H
#define MENU_H

#include <stdint.h>

enum {
	MENU_SPLASH,
	MENU_LOAD_STORE,
	MENU_FLASH_EEPROM,
	MENU_ARE_YOU_SURE,
	MENU_LOCATION,
	MENU_MAIN_MENU,
	MENU_DEVICE,
	MENU_CATEGORY,
};

typedef struct update_menu16_arg {
	uint16_t val;
	uint8_t position;
	uint8_t startfrom;
} update_menu16_arg;

uint8_t update_menu16(update_menu16_arg* arg);
void print_buffer();
uint8_t show_menu(uint8_t menu_index);
uint16_t menu_ask16(uint8_t id);

#endif // MENU_H
