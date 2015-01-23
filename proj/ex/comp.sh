#!/bin/sh
printf "[COMP]cleaning up....\n"
rm bin/comp/program.elf
printf "[COMP]compiling....\n"
gcc umed/genr/main.o usrc/bin/jlib/jlvm.o usrc/main.c -lSDL2_mixer $(sdl2-config --static-libs) -lSDL2_image -lGL -lzip -lm -o bin/comp/program.elf -Wall 
printf "[COMP]done compiling!\n"
