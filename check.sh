gcc disasm.c
./a.out > out
diff <(nl out) <(nl avr_as)

avr-gcc disasm.c -mmcu=atmega328p -Os -Wall -Wextra -fpack-struct \
	-fshort-enums -funsigned-char -funsigned-bitfields \
	-Wl,--gc-sections -fno-exceptions \
	-DF_CPU=8000000UL -std=c11 -o avr.elf

avr-size --format=avr --mcu=atmega328p avr.elf
