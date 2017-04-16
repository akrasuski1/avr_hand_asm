SRC="interface.c buffer_utils.c decompression.c common_tables.c decode.c "
SRC+="menu.c gen/*.c "
AVRSRC="petitfs/*.c "
MAIN="main.c"
TEST="test.c"

mkdir -p src/gen bin

gcc local/compress.c src/common_tables.c -o bin/compress
bin/compress

cd src
#gcc $SRC $TEST -o ../bin/disas_test
cd ..
#diff <(bin/disas_test | nl) <(nl local/avr_as)

cd src
gcc $SRC $MAIN -o ../bin/disas
cd ..

cd src
# TO-DO: somehow disable interrupt table
avr-gcc $AVRSRC $SRC $MAIN -I"/usr/lib/avr/include" -mmcu=atmega8 -Os -g3 -Wall \
	-Wextra -ffunction-sections -fdata-sections -fpack-struct -fshort-enums \
	-funsigned-char -funsigned-bitfields -DF_CPU=8000000UL -std=c11 \
	-o ../bin/avr.elf -Wl,--gc-sections \
	-flto -mrelax -mcall-prologues -mstrict-X -fno-early-inlining -fweb -funroll-loops
cd ..

avr-size --format=avr --mcu=atmega8 bin/avr.elf
