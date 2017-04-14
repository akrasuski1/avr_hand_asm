#ifndef BUFFER_UTILS_H_
#define BUFFER_UTILS_H_

#include <stdint.h>

extern uint8_t buffer[20];
extern uint8_t* buf;

void reset();
void append(uint8_t c);
void skip(uint8_t many);
// Up to 9999.
void append_decnum(uint16_t num);
void append_hexnibble(uint8_t num);

#endif
