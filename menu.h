#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#define MOD_NONE 0
#define MOD_UPPERCASE 1

enum {
	MENU_SPLASH,
	MENU_LOAD_STORE,
	MENU_FLASH_EEPROM,
	MENU_ARE_YOU_SURE,
	MENU_LOCATION,
	MENU_MAIN_MENU,
	MENU_DEVICE,
	MENU_OPCODE_TYPE
};

void print_buffer(uint8_t size_change);
uint8_t show_menu(uint8_t menu_index);
uint16_t menu_ask16(uint8_t id);

#endif // MENU_H
