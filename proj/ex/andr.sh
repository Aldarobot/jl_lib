#!/bin/sh
printf "[COMP]setting up files....\n"
cp usrc/main.c usrc/bin/ANDR/ANDK/ANDROID/jni/src/gen/hack_user_main.c
cp umed/icon.png usrc/bin/ANDR/ANDK/ANDROID/res/drawable/prgm_icon.png
printf "[ANDR]creating header with data....\n"
./usrc/bin/jlvm_inch
cp umed/genr/main.h usrc/bin/ANDR/ANDK/ANDROID/jni/src/gen/main.h
printf "[COMP]compiling....\n"
cd usrc/bin/
./andr
printf "[COMP]done compiling!\n"
