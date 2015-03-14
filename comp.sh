#!/bin/sh

#
# This file overwrites proj/ex/jlvm/ with the newest version of jlvm.
#

printf "[COMP]compiling separate JLVM object files....\n"
#BLIBS
gcc src/me.c -c -o ../../obj/jlvm/me.o $(sdl2-config --cflags) -Wall #1
gcc src/cl.c -c -o ../../obj/jlvm/cl.o $(sdl2-config --cflags) -Wall #2
gcc src/io.c -c -o ../../obj/jlvm/io.o $(sdl2-config --cflags) -Wall #3
gcc src/fl.c -c -o ../../obj/jlvm/fl.o $(sdl2-config --cflags) -Wall #4
gcc src/cm.c -c -o ../../obj/jlvm/cm.o $(sdl2-config --cflags) -Wall #5
gcc src/ct.c -c -o ../../obj/jlvm/ct.o $(sdl2-config --cflags) -Wall #6
gcc src/sg.c -c -o ../../obj/jlvm/sg.o $(sdl2-config --cflags) -Wall #7
gcc src/dl.c -c -o ../../obj/jlvm/dl.o $(sdl2-config --cflags) -Wall #8
gcc src/gl.c -c -o ../../obj/jlvm/gl.o $(sdl2-config --cflags) -Wall #9
gcc src/gr.c -c -o ../../obj/jlvm/gr.o $(sdl2-config --cflags) -Wall #10
gcc src/vi.c -c -o ../../obj/jlvm/vi.o $(sdl2-config --cflags) -Wall #11
gcc src/au.c -c -o ../../obj/jlvm/au.o $(sdl2-config --cflags) -Wall #12
#####

printf "[COMP]compiling singular JLVM object file....\n"
rm ../../bin/jlvm/jlib/jl.o
rm ../../bin/jlvm/jlib/jl.h
rm ../../bin/jlvm/jlib/jl_sg.h
rm ../../bin/jlvm/jlib/jl_ct.h
# 

ar csr ../../bin/jlvm/jlib/jl.o \
../../obj/jlvm/glew.o \
../../obj/jlvm/me.o ../../obj/jlvm/cl.o ../../obj/jlvm/io.o ../../obj/jlvm/fl.o \
../../obj/jlvm/cm.o ../../obj/jlvm/ct.o ../../obj/jlvm/sg.o ../../obj/jlvm/dl.o \
../../obj/jlvm/gl.o ../../obj/jlvm/gr.o ../../obj/jlvm/vi.o ../../obj/jlvm/au.o \

printf "[COMP]updating jlvm's bin folder....\n"
cp src/header/jl.h ../../bin/jlvm/jlib/jl.h
cp src/header/jl_sg.h ../../bin/jlvm/jlib/jl_sg.h
cp src/header/jl_ct.h ../../bin/jlvm/jlib/jl_ct.h
printf "[COMP]updating andr...\n"
cp --recursive -u -t ../../bin/jlvm/ANDR/andk/ANDROID/jni/src/ src/*
cp --recursive -u -t ../../bin/jlvm/ANDR/andk/ANDROID/bin/jlvm/jlib/ \
 ../../bin/jlvm/jlib/*

printf "[COMP]done!\n"
