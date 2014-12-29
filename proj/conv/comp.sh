#!/bin/sh
printf "[COMP]cleaning up....\n"
rm usrc/bin/jlvm_conv
printf "[COMP]compiling....\n"
gcc umed/genr/main.o usrc/bin/jlib/jlvm.o usrc/main.c -lSDL2 -lSDL2_image -lSDL2_mixer -lGL -lzip -lm -o usrc/bin/jlvm_conv -Wall
printf "[COMP]done compiling!\n"
