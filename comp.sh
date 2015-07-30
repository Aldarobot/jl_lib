#!/bin/sh

# Empty directory: build/obj/
rm -r build/obj/
mkdir build/obj/
# Remove header files.
rm ../../bin/jl_lib/jlib/jl.o
rm ../../bin/jl_lib/jlib/jl.h
rm ../../bin/jl_lib/jlib/jl_ty.h
rm ../../bin/jl_lib/jlib/jl_en.h
rm ../../bin/jl_lib/jlib/jl_ct.h
#
printf "[COMP] compiling separate jl_lib object files....\n"
#BLIBS
gcc src/me.c -c -o build/obj/me.o $(sdl2-config --cflags) -Wall -g #1
gcc src/cl.c -c -o build/obj/cl.o $(sdl2-config --cflags) -Wall -g #2
gcc src/io.c -c -o build/obj/io.o $(sdl2-config --cflags) -Wall -g #3
gcc src/fl.c -c -o build/obj/fl.o $(sdl2-config --cflags) -Wall -g #4
gcc src/cm.c -c -o build/obj/cm.o $(sdl2-config --cflags) -Wall -g #5
gcc src/ct.c -c -o build/obj/ct.o $(sdl2-config --cflags) -Wall -g #6
gcc src/sg.c -c -o build/obj/sg.o $(sdl2-config --cflags) -Wall -g #7
gcc src/dl.c -c -o build/obj/dl.o $(sdl2-config --cflags) -Wall -g #8
gcc src/gl.c -c -o build/obj/gl.o $(sdl2-config --cflags) -Wall -g #9
gcc src/gr.c -c -o build/obj/gr.o $(sdl2-config --cflags) -Wall -g #10
gcc src/vi.c -c -o build/obj/vi.o $(sdl2-config --cflags) -Wall -g #11
gcc src/au.c -c -o build/obj/au.o $(sdl2-config --cflags) -Wall -g #12
gcc src/Main.c -c -o build/obj/Main.o $(sdl2-config --cflags) -Wall -g
#####

# Make "jl.o"
printf "[COMP] compiling singular jl_lib object file....\n"
ar csr ../../bin/jl_lib/jlib/jl.o build/obj/*.o
# Re-add header files.
printf "[COMP] updating jl_lib's bin folder....\n"
cp src/header/jl.h ../../bin/jl_lib/jlib/jl.h
cp src/header/jl_ty.h ../../bin/jl_lib/jlib/jl_ty.h
cp src/header/jl_en.h ../../bin/jl_lib/jlib/jl_en.h
cp src/header/jl_ct.h ../../bin/jl_lib/jlib/jl_ct.h
cp src/header/jl_me.h ../../bin/jl_lib/jlib/jl_me.h
cp src/header/clump.h ../../bin/jl_lib/jlib/clump.h
cp ../../obj/lb/lib_zip.o ../../bin/jl_lib/jlib/lib_zip.o
cp ../../obj/lb/lib_SDL_image.o ../../bin/jl_lib/jlib/lib_SDL_image.o
# Update the android directory
printf "[COMP] updating andr...\n"
cp --recursive -u -t ../../bin/jl_lib/android/jni/src/ src/*
cp --recursive -u -t ../../bin/jl_lib/android/jni/bin/jl_lib/jlib/ \
 ../../bin/jl_lib/jlib/*

printf "[COMP] done!\n"
