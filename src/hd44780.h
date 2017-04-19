#ifndef HD44780_H_
#define HD44780_H_

#include <stdint.h>

void lcd_init();
void lcd_xy(uint8_t row, uint8_t col);
void lcd_putc(uint8_t ch);
void lcd_retry();

#endif
