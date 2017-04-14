gcc compress.c -o obj/compress
obj/compress > obj/compressed.h

gcc disasm.c -DTEST -o obj/disas_test
diff <(obj/disas_test | nl) <(nl avr_as)

gcc disasm.c -o obj/disas

avr-gcc disasm.c -I"/usr/lib/avr/include" -mmcu=atmega328p -Os -g3 -Wall \
	-Wextra -ffunction-sections -fdata-sections -fpack-struct -fshort-enums \
	-funsigned-char -funsigned-bitfields -DF_CPU=8000000UL -std=c11 \
	-o obj/avr.elf -Wl,--gc-sections
avr-size --format=avr --mcu=atmega328p obj/avr.elf
