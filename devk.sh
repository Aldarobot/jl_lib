#!/bin/sh

#
# This file compiles the 3 programs to help make the JL_lib programs
#

printf "[COMP/DEVK]compiling Devk programs....\n"
printf "[COMP/DEVK]inch....\n"
gcc ../JLVM_BIN/jlib/libzip.o inch.c cuhc.c -o ../JLVM_BIN/jlvm_inch
printf "[COMP/DEVK]jvrp....\n"
gcc jvrp.c -o ../JLVM_BIN/jlvm_jvrp ../JLVM_BIN/jlib/libzip.o -lz
printf "[COMP/DEVK]conv....\n"
cd proj/conv/
sh comp.sh
printf "[COMP/DEVK] DONE!\n"
