#!/bin/sh

#
# This file compiles the 3 programs to help make the JL_lib programs
#

printf "[COMP/DEVK]compiling Devk programs....\n"
printf "[COMP/DEVK]inch....\n"
gcc inch.c cuhc.c -o ../JLVM_BIN/jlvm_inch -lzip
printf "[COMP/DEVK]jvrp....\n"
gcc jvrp.c -o ../JLVM_BIN/jlvm_jvrp -lzip
printf "[COMP/DEVK]conv....\n"
cd proj/conv/
./comp.sh
printf "[COMP/DEVK] DONE!\n"
