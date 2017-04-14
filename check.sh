SRC="interface.c buffer_utils.c decompression.c common_tables.c decode.c menu.c"
MAIN="main.c"
TEST="test.c"

mkdir -p gen obj

gcc compress.c common_tables.c -o obj/compress
obj/compress

gcc $SRC $TEST -o obj/disas_test
diff <(obj/disas_test | nl) <(nl avr_as)

gcc $SRC $MAIN -o obj/disas

avr-gcc $SRC $MAIN -I"/usr/lib/avr/include" -mmcu=atmega328p -Os -g3 -Wall \
	-Wextra -ffunction-sections -fdata-sections -fpack-struct -fshort-enums \
	-funsigned-char -funsigned-bitfields -DF_CPU=8000000UL -std=c11 \
	-o obj/avr.elf -Wl,--gc-sections -flto
avr-size --format=avr --mcu=atmega328p obj/avr.elf
