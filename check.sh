gcc disasm.c && ./a.out > out && diff <(nl out) <(nl avr_as)
