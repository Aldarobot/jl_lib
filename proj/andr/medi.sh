#!/bin/sh
printf "[MEDI]converting media...\n"
./usrc/bin/jlvm_conv
printf "[MEDI]compiling media....\n"
./usrc/bin/jlvm_jvrp
printf "[MEDI]creating header with data....\n"
./usrc/bin/jlvm_inch
printf "[MEDI]compiling....\n"
gcc umed/genr/main.c -c -o umed/genr/main.o -Wall
cp umed/genr/main.h usrc/bin/ANDR/ANDK/ANDROID/jni/src/gen/main.h
printf "[MEDI]cleaning up....\n"
truncate umed/genr/main.h --size 0
printf "[MEDI]done compiling!\n"
