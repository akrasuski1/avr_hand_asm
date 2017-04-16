#ifdef __AVR__

#include <avr/pgmspace.h>
#define pgm_byte(x) pgm_read_byte_near(x)

#else

#define pgm_byte(x) (*(x))
#define PROGMEM

#endif
