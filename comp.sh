#!/bin/sh

#
# This file overwrites proj/ex/jl_lib/ with the newest version of jl_lib.
#

printf "[COMP] compiling separate jl_lib object files....\n"
#BLIBS
gcc src/me.c -c -o ../../obj/jl_lib/me.o $(sdl2-config --cflags) -Wall -g #1
gcc src/cl.c -c -o ../../obj/jl_lib/cl.o $(sdl2-config --cflags) -Wall -g #2
gcc src/io.c -c -o ../../obj/jl_lib/io.o $(sdl2-config --cflags) -Wall -g #3
gcc src/fl.c -c -o ../../obj/jl_lib/fl.o $(sdl2-config --cflags) -Wall -g #4
gcc src/cm.c -c -o ../../obj/jl_lib/cm.o $(sdl2-config --cflags) -Wall -g #5
gcc src/ct.c -c -o ../../obj/jl_lib/ct.o $(sdl2-config --cflags) -Wall -g #6
gcc src/sg.c -c -o ../../obj/jl_lib/sg.o $(sdl2-config --cflags) -Wall -g #7
gcc src/dl.c -c -o ../../obj/jl_lib/dl.o $(sdl2-config --cflags) -Wall -g #8
gcc src/gl.c -c -o ../../obj/jl_lib/gl.o $(sdl2-config --cflags) -Wall -g #9
gcc src/gr.c -c -o ../../obj/jl_lib/gr.o $(sdl2-config --cflags) -Wall -g #10
gcc src/vi.c -c -o ../../obj/jl_lib/vi.o $(sdl2-config --cflags) -Wall -g #11
gcc src/au.c -c -o ../../obj/jl_lib/au.o $(sdl2-config --cflags) -Wall -g #12
#####

printf "[COMP] compiling singular jl_lib object file....\n"
rm ../../bin/jl_lib/jlib/jl.o
rm ../../bin/jl_lib/jlib/jl.h
rm ../../bin/jl_lib/jlib/jl_sg.h
rm ../../bin/jl_lib/jlib/jl_ct.h
# 

ar csr ../../bin/jl_lib/jlib/jl.o \
../../obj/jl_lib/me.o ../../obj/jl_lib/cl.o ../../obj/jl_lib/io.o ../../obj/jl_lib/fl.o \
../../obj/jl_lib/cm.o ../../obj/jl_lib/ct.o ../../obj/jl_lib/sg.o ../../obj/jl_lib/dl.o \
../../obj/jl_lib/gl.o ../../obj/jl_lib/gr.o ../../obj/jl_lib/vi.o ../../obj/jl_lib/au.o \

printf "[COMP] updating jl_lib's bin folder....\n"
cp src/header/jl.h ../../bin/jl_lib/jlib/jl.h
cp src/header/jl_sg.h ../../bin/jl_lib/jlib/jl_sg.h
cp src/header/jl_ct.h ../../bin/jl_lib/jlib/jl_ct.h
cp src/header/jl_me.h ../../bin/jl_lib/jlib/jl_me.h
cp src/header/clump.h ../../bin/jl_lib/jlib/clump.h
cp ../../obj/lb/lib_zip.o ../../bin/jl_lib/jlib/lib_zip.o
cp ../../obj/lb/lib_SDL_image.o ../../bin/jl_lib/jlib/lib_SDL_image.o
printf "[COMP] updating andr...\n"
cp --recursive -u -t ../../bin/jl_lib/android/andk/ANDROID/jni/src/ src/*
cp --recursive -u -t ../../bin/jl_lib/android/andk/ANDROID/jni/bin/jl_lib/jlib/ \
 ../../bin/jl_lib/jlib/*

printf "[COMP] done!\n"
