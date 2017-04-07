gcc compress.c -o compress
./compress > compressed.h

gcc disasm.c -o disas
./disas > out
diff <(nl out) <(nl avr_as)

avr-gcc -c disasm.c -I"/usr/lib/avr/include" -mmcu=atmega328p -Os -g3 -Wall -Wextra -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -MMD -MP -DF_CPU=8000000UL -std=c11 -o main.o
avr-gcc -mmcu=atmega328p -Wl,--gc-sections  -fno-exceptions -fno-rtti -o avr.elf main.o
avr-size --format=avr --mcu=atmega328p avr.elf
