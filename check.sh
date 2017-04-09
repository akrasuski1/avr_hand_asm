gcc compress.c -o compress
./compress > compressed.h

gcc disasm.c -o disas
#diff <(./disas | nl) <(nl avr_as)

avr-gcc disasm.c -I"/usr/lib/avr/include" -mmcu=atmega328p -Os -g3 -Wall \
	-Wextra -ffunction-sections -fdata-sections -fpack-struct -fshort-enums \
	-funsigned-char -funsigned-bitfields -DF_CPU=8000000UL -std=c11 \
	-o avr.elf -Wl,--gc-sections
avr-size --format=avr --mcu=atmega328p avr.elf
