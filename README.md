# AVR Hand Assembler
Tiny disassembler for AVR architecture, along with code editor able to fit on ATmega8 or similar chips.

## Components
The thing consists of a couple of components, the most important of which is probably tiny disassembler.
The disassembler itself, when compiled to AVR architecture, takes around 2kB of flash space, which is 
very little - there are over 100 different instructions, for each of which we have to remember opcode
name (usually 3-4 characters), constant bits, variable bits and their meaning and so on. We use simple
compression algorithm to store that data in smaller form (notably, decompression routine is also included
in that 2kB).

TODO
