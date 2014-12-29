#!/bin/sh
printf "[COMP]setting up files....\n"
cp usrc/main.c lbin/bin/ANDR/ANDK/ANDROID/jni/src/main.c
printf "[COMP]compiling....\n"
cd lbin/bin/
./andr
printf "[COMP]done compiling!\n"
