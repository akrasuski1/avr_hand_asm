#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdint.h>

#define A_LEFT  1
#define A_RIGHT 2
#define A_PRESS 3

#define B_LEFT  4
#define B_RIGHT 5
#define B_PRESS 6

#ifndef __AVR__
#include <sys/time.h>
typedef struct timeval timer;
#else
typedef struct timer{uint8_t x;} timer;
#endif

void select_display_line(uint8_t line);
void put_character(uint8_t c);
uint8_t poll_user_input();
void start_timer(timer* t);
uint16_t elapsed_time(timer* t);
void small_delay();

#endif
