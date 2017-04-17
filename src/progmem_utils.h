#ifdef __AVR__

#include <avr/pgmspace.h>
#define pgm_byte(x) pgm_read_byte_near(x)
#define pgm_word(x) pgm_read_word_near(x)

#else

#define pgm_byte(x) (*(x))
#define pgm_word(x) (*(x))
#define PROGMEM

#endif
