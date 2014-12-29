#!/bin/sh

#
# This file overwrites proj/ex/jlvm/ with the newest version of jlvm.
#

printf "[COMP]compiling separate JLVM object files....\n"
#BLIBS
gcc src/amem.c -c -o obj/amem.o -Wall #1
gcc src/clmp.c -c -o obj/clmp.o -Wall #2
gcc src/siop.c -c -o obj/siop.o -Wall #3
gcc src/file.c -c -o obj/file.o -Wall #4
gcc src/comm.c -c -o obj/comm.o -Wall #5
gcc src/inpt.c -c -o obj/inpt.o -Wall #6
gcc src/sgrp.c -c -o obj/sgrp.o -Wall #7
gcc src/lsdl.c -c -o obj/lsdl.o -Wall #8
gcc src/eogl.c -c -o obj/eogl.o -Wall #9
gcc src/grph.c -c -o obj/grph.o -Wall #10
gcc src/vide.c -c -o obj/vide.o -Wall #11
gcc src/audi.c -c -o obj/audi.o -Wall #12
#####

printf "[COMP]compiling singular JLVM object file....\n"
rm ../JLVM_BIN/jlib/jlvm.o
rm ../JLVM_BIN/jlib/jlvm.h
rm ../JLVM_BIN/jlib/jlvm_sgrp.h
rm ../JLVM_BIN/jlib/jlvm_inpt.h
# 

ar csr ../JLVM_BIN/jlib/jlvm.o \
obj/jlvm_sdl_mixer.o \
obj/dont_delete/glew.o \
obj/amem.o obj/clmp.o obj/siop.o obj/file.o \
obj/comm.o obj/inpt.o obj/sgrp.o obj/lsdl.o \
obj/eogl.o obj/grph.o obj/vide.o obj/audi.o \

printf "[COMP]updating jlvm.h....\n"
cp src/header/jlvm.h ../JLVM_BIN/jlib/jlvm.h
cp src/header/jlvm_sgrp.h ../JLVM_BIN/jlib/jlvm_sgrp.h
cp src/header/jlvm_inpt.h ../JLVM_BIN/jlib/jlvm_inpt.h
printf "[COMP]updating projects....\n"
rm proj/jvdk/jlvm/jlvm.h
rm proj/jvdk/jlvm/jlvm.o
rm proj/jlvm/jlvm/jlvm.h
rm proj/jlvm/jlvm/jlvm.o
cp ../JLVM_BIN/jlib/jlvm.h proj/jvdk/jlvm/jlvm.h
cp ../JLVM_BIN/jlib/jlvm.h proj/jlvm/jlvm/jlvm.h
cp ../JLVM_BIN/jlib/jlvm.o proj/jvdk/jlvm/jlvm.o
cp ../JLVM_BIN/jlib/jlvm.o proj/jlvm/jlvm/jlvm.o
printf "[COMP]updating andr...\n"
#rm ../JLVM_BIN/ANDR/ANDK/ANDROID/jni/src/*
cp --recursive -u -t ../JLVM_BIN/ANDR/ANDK/ANDROID/jni/src/ src/*

#cp -u src/amem.c ../JLVM_BIN/ANDR/ANDK/ANDROID/jni/src/amem.c
printf "[COMP]done!\n"
#gcc  -c -o JLVM_DEV/NEWPROJECT/OUT/JLVM.o -Wall -pg

#printf "[COMP]compiling jcfc (JComp File Converter)....\n"
#gcc cheat.c src/glew/glew.o src/jlvm_vmap.c JLL/bjl.o obj/jlvm_sdl_mixer.o src/jlvm_blib_audi.c src/jlvm_blib_inpt.c src/jlvm_user.c obj/jlvm_blib_grph.o obj/jlvmpu.o conv.c -lSDL2 -lSDL2_image -lSDL2_mixer -lzip -lm -lGL -o USER/JLVM/JCFC.ELF -Wall

#printf "[COMP]compiling jlex (JLvm EXecutable file creator)....\n"
#gcc genjle.c -lzip -o USER/NEWPROJECT/jlex

#printf "[COMP]compiling jvrp (JlVm Resource Packager)....\n"
#gcc jvrp.c -lzip -o USER/NEWPROJECT/jvrp

#printf "[COMP]compiling inch (INclude C Header)....\n"
#gcc cuhc.c inch.c -lzip -o USER/NEWPROJECT/inch
