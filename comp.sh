#!/bin/sh

#
# This file overwrites proj/ex/jlvm/ with the newest version of jlvm.
#

printf "[COMP]compiling separate JLVM object files....\n"
#BLIBS
gcc src/amem.c -c -o ../obj/jlvm/amem.o $(sdl2-config --cflags) -Wall #1
gcc src/clmp.c -c -o ../obj/jlvm/clmp.o $(sdl2-config --cflags) -Wall #2
gcc src/siop.c -c -o ../obj/jlvm/siop.o $(sdl2-config --cflags) -Wall #3
gcc src/file.c -c -o ../obj/jlvm/file.o $(sdl2-config --cflags) -Wall #4
gcc src/comm.c -c -o ../obj/jlvm/comm.o $(sdl2-config --cflags) -Wall #5
gcc src/inpt.c -c -o ../obj/jlvm/inpt.o $(sdl2-config --cflags) -Wall #6
gcc src/sgrp.c -c -o ../obj/jlvm/sgrp.o $(sdl2-config --cflags) -Wall #7
gcc src/lsdl.c -c -o ../obj/jlvm/lsdl.o $(sdl2-config --cflags) -Wall #8
gcc src/eogl.c -c -o ../obj/jlvm/eogl.o $(sdl2-config --cflags) -Wall #9
gcc src/grph.c -c -o ../obj/jlvm/grph.o $(sdl2-config --cflags) -Wall #10
gcc src/vide.c -c -o ../obj/jlvm/vide.o $(sdl2-config --cflags) -Wall #11
gcc src/audi.c -c -o ../obj/jlvm/audi.o $(sdl2-config --cflags) -Wall #12
#####

printf "[COMP]compiling singular JLVM object file....\n"
rm ../JLVM_BIN/jlib/jlvm.o
rm ../JLVM_BIN/jlib/jlvm.h
rm ../JLVM_BIN/jlib/jlvm_sgrp.h
rm ../JLVM_BIN/jlib/jlvm_inpt.h
# 

ar csr ../JLVM_BIN/jlib/jlvm.o \
../obj/jlvm/dont_delete/glew.o \
../obj/jlvm/amem.o ../obj/jlvm/clmp.o ../obj/jlvm/siop.o ../obj/jlvm/file.o \
../obj/jlvm/comm.o ../obj/jlvm/inpt.o ../obj/jlvm/sgrp.o ../obj/jlvm/lsdl.o \
../obj/jlvm/eogl.o ../obj/jlvm/grph.o ../obj/jlvm/vide.o ../obj/jlvm/audi.o \
#../obj/jlvm/mix/bitwise.o ../obj/jlvm/mix/framing.o ../obj/jlvm/mix/analysis.o ../obj/jlvm/mix/bitrate.o ../obj/jlvm/mix/block.o ../obj/jlvm/mix/codebook.o ../obj/jlvm/mix/envelope.o ../obj/jlvm/mix/floor0.o ../obj/jlvm/mix/floor1.o ../obj/jlvm/mix/info.o ../obj/jlvm/mix/lookup.o ../obj/jlvm/mix/lpc.o ../obj/jlvm/mix/lsp.o ../obj/jlvm/mix/mapping0.o ../obj/jlvm/mix/mdct.o ../obj/jlvm/mix/psy.o ../obj/jlvm/mix/registry.o ../obj/jlvm/mix/res0.o ../obj/jlvm/mix/sharedbook.o ../obj/jlvm/mix/smallft.o ../obj/jlvm/mix/synthesis.o ../obj/jlvm/mix/vorbisenc.o ../obj/jlvm/mix/vorbisfile.o ../obj/jlvm/mix/window.o ../obj/jlvm/mix/music_ogg.o ../obj/jlvm/mix/dynamic_ogg.o ../obj/jlvm/mix/load_ogg.o ../obj/jlvm/mix/load_aiff.o ../obj/jlvm/mix/load_voc.o ../obj/jlvm/mix/effects_internal.o ../obj/jlvm/mix/music.o ../obj/jlvm/mix/effect_position.o ../obj/jlvm/mix/mixer.o \
#../obj/jlvm/sdl/SDL_video.o ../obj/jlvm/sdl/SDL_surface.o ../obj/jlvm/sdl/SDL_stretch.o ../obj/jlvm/sdl/SDL_shape.o ../obj/jlvm/sdl/SDL_RLEaccel.o ../obj/jlvm/sdl/SDL_rect.o ../obj/jlvm/sdl/SDL_pixels.o ../obj/jlvm/sdl/SDL_fillrect.o ../obj/jlvm/sdl/SDL_egl.o ../obj/jlvm/sdl/SDL_clipboard.o ../obj/jlvm/sdl/SDL_bmp.o ../obj/jlvm/sdl/SDL_blit_slow.o ../obj/jlvm/sdl/SDL_blit_N.o ../obj/jlvm/sdl/SDL_blit_copy.o ../obj/jlvm/sdl/SDL_blit_auto.o ../obj/jlvm/sdl/SDL_blit_A.o ../obj/jlvm/sdl/SDL_blit_1.o ../obj/jlvm/sdl/SDL_blit_0.o ../obj/jlvm/sdl/SDL_blit.o ../obj/jlvm/sdl/timer_unix.o ../obj/jlvm/sdl/timer.o ../obj/jlvm/sdl/SDL_systls.o ../obj/jlvm/sdl/SDL_systhread.o ../obj/jlvm/sdl/SDL_syssem.o ../obj/jlvm/sdl/SDL_sysmutex.o ../obj/jlvm/sdl/SDL_syscond.o ../obj/jlvm/sdl/thread.o ../obj/jlvm/sdl/SDL_string.o ../obj/jlvm/sdl/SDL_stdlib.o ../obj/jlvm/sdl/SDL_qsort.o ../obj/jlvm/sdl/SDL_malloc.o ../obj/jlvm/sdl/SDL_iconv.o ../obj/jlvm/sdl/SDL_getenv.o ../obj/jlvm/sdl/SDL_rotate.o ../obj/jlvm/sdl/SDL_render_sw.o ../obj/jlvm/sdl/SDL_drawpoint.o ../obj/jlvm/sdl/SDL_drawline.o ../obj/jlvm/sdl/SDL_blendpoint.o ../obj/jlvm/sdl/SDL_blendline.o ../obj/jlvm/sdl/SDL_blendfillrect.o ../obj/jlvm/sdl/SDL_yuv_sw.o ../obj/jlvm/sdl/SDL_yuv_mmx.o ../obj/jlvm/sdl/SDL_render.o ../obj/jlvm/sdl/SDL_d3dmath.o ../obj/jlvm/sdl/file_dummy.o ../obj/jlvm/sdl/android.o ../obj/jlvm/sdl/power.o ../obj/jlvm/sdl/dlopen.o ../obj/jlvm/sdl/SDL_andrjoy.o ../obj/jlvm/sdl/SDL_joystick.o ../obj/jlvm/sdl/SDL_gamecontroller.o ../obj/jlvm/sdl/haptic_dummy.o ../obj/jlvm/sdl/haptic.o ../obj/jlvm/sdl/file.o ../obj/jlvm/sdl/SDL_windowevents.o ../obj/jlvm/sdl/SDL_touch.o ../obj/jlvm/sdl/SDL_quit.o ../obj/jlvm/sdl/SDL_mouse.o ../obj/jlvm/sdl/SDL_keyboard.o ../obj/jlvm/sdl/SDL_gesture.o ../obj/jlvm/sdl/SDL_events.o ../obj/jlvm/sdl/SDL_dropevents.o ../obj/jlvm/sdl/SDL_clipboardevents.o ../obj/jlvm/sdl/cpuinfo.o ../obj/jlvm/sdl/android.o ../obj/jlvm/sdl/SDL_atomic.o ../obj/jlvm/sdl/SDL_spinlock.o ../obj/jlvm/sdl/SDL_dummy.o ../obj/jlvm/sdl/SDL_android.o ../obj/jlvm/sdl/SDL_wave.o ../obj/jlvm/sdl/SDL_audiotypecvt.o ../obj/jlvm/sdl/SDL_audiodev.o ../obj/jlvm/sdl/SDL_audiocvt.o ../obj/jlvm/sdl/SDL_audio.o ../obj/jlvm/sdl/SDL_log.o ../obj/jlvm/sdl/SDL_hints.o ../obj/jlvm/sdl/SDL_error.o ../obj/jlvm/sdl/SDL_assert.o ../obj/jlvm/sdl/SDL.o \

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

printf "[COMP]done!\n"
