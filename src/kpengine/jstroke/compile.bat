pilrc jstroke.rcp 

m68k-palmos-coff-gcc -DFOR_PILOT_GCC -O1 -c jstroke.c   -o jstroke.o 
m68k-palmos-coff-gcc -DFOR_PILOT_GCC -O1 -c memowrite.c -o memowrite.o 
m68k-palmos-coff-gcc -DFOR_PILOT_GCC -O1 -c scoring.c   -o scoring.o
m68k-palmos-coff-gcc -DFOR_PILOT_GCC -O1 -c util.c      -o util.o
m68k-palmos-coff-gcc -DFOR_PILOT_GCC -O1 -c bootdata.c  -o bootdata.o

m68k-palmos-coff-gcc -O1 jstroke.o memowrite.o scoring.o util.o bootdata.o -o jstroke

m68k-palmos-coff-obj-res jstroke
build-prc  JStroke.prc "JStroke" jStr *.grc *.bin
dir *.prc
@del jstroke
@del *.grc
@del *.bin
@del *.o
