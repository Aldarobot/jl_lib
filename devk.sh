#!/bin/sh

#
# This file compiles the 3 programs to help make the JL_lib programs
#

printf "[COMP/DEVK]compiling Devk programs....\n"
printf "[COMP/DEVK]inch....\n"
gcc ../../bin/jlvm/jlib/libzip.o inch.c cuhc.c -o ../../bin/jlvm/jlvm_inch
printf "[COMP/DEVK]jvrp....\n"
gcc jvrp.c -o ../../bin/jlvm/jlvm_jvrp ../../bin/jlvm/jlib/libzip.o -lz
printf "[COMP/DEVK]conv....\n"
cd ../jlvm_proj/conv/
sh comp.sh
printf "[COMP/DEVK] DONE!\n"
