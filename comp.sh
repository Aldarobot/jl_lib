#!/bin/sh

#
# This file overwrites proj/ex/jlvm/ with the newest version of jlvm.
#

printf "[COMP]compiling separate JLVM object files....\n"
#BLIBS
gcc src/amem.c -c -o ../../obj/jlvm/amem.o $(sdl2-config --cflags) -Wall #1
gcc src/clmp.c -c -o ../../obj/jlvm/clmp.o $(sdl2-config --cflags) -Wall #2
gcc src/siop.c -c -o ../../obj/jlvm/siop.o $(sdl2-config --cflags) -Wall #3
gcc src/file.c -c -o ../../obj/jlvm/file.o $(sdl2-config --cflags) -Wall #4
gcc src/comm.c -c -o ../../obj/jlvm/comm.o $(sdl2-config --cflags) -Wall #5
gcc src/inpt.c -c -o ../../obj/jlvm/inpt.o $(sdl2-config --cflags) -Wall #6
gcc src/sgrp.c -c -o ../../obj/jlvm/sgrp.o $(sdl2-config --cflags) -Wall #7
gcc src/lsdl.c -c -o ../../obj/jlvm/lsdl.o $(sdl2-config --cflags) -Wall #8
gcc src/eogl.c -c -o ../../obj/jlvm/eogl.o $(sdl2-config --cflags) -Wall #9
gcc src/grph.c -c -o ../../obj/jlvm/grph.o $(sdl2-config --cflags) -Wall #10
gcc src/vide.c -c -o ../../obj/jlvm/vide.o $(sdl2-config --cflags) -Wall #11
gcc src/audi.c -c -o ../../obj/jlvm/audi.o $(sdl2-config --cflags) -Wall #12
#####

printf "[COMP]compiling singular JLVM object file....\n"
rm ../../bin/jlvm/jlib/jlvm.o
rm ../../bin/jlvm/jlib/jlvm.h
rm ../../bin/jlvm/jlib/jlvm_sgrp.h
rm ../../bin/jlvm/jlib/jlvm_inpt.h
# 

ar csr ../../bin/jlvm/jlib/jlvm.o \
../../obj/jlvm/glew.o \
../../obj/jlvm/amem.o ../../obj/jlvm/clmp.o ../../obj/jlvm/siop.o ../../obj/jlvm/file.o \
../../obj/jlvm/comm.o ../../obj/jlvm/inpt.o ../../obj/jlvm/sgrp.o ../../obj/jlvm/lsdl.o \
../../obj/jlvm/eogl.o ../../obj/jlvm/grph.o ../../obj/jlvm/vide.o ../../obj/jlvm/audi.o \

printf "[COMP]updating jlvm's bin folder....\n"
cp src/header/jlvm.h ../../bin/jlvm/jlib/jlvm.h
cp src/header/jlvm_sgrp.h ../../bin/jlvm/jlib/jlvm_sgrp.h
cp src/header/jlvm_inpt.h ../../bin/jlvm/jlib/jlvm_inpt.h
printf "[COMP]updating andr...\n"
cp --recursive -u -t ../../bin/jlvm/ANDR/andk/ANDROID/jni/src/ src/*

printf "[COMP]done!\n"
