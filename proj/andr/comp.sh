#!/bin/sh
printf "[COMP]cleaning up....\n"
rm bin/program.elf
printf "[COMP]compiling....\n"
gcc umed/genr/main.o usrc/bin/jlib/jlvm.o usrc/main.c -lSDL2 -lSDL2_image -lSDL2_mixer -lGL -lzip -lm -o bin/program.elf -Wall
printf "[COMP]done compiling!\n"
