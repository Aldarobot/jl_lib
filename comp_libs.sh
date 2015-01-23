#!/bin/sh

printf "[COMP]compiling separate SDL_mixer object files....\n"
gcc src/lib/SDL_mixer/external/libogg-1.3.1/src/bitwise.c -c -o obj/mix/bitwise.o
gcc src/lib/SDL_mixer/external/libogg-1.3.1/src/framing.c -c -o obj/mix/framing.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/analysis.c -c -o obj/mix/analysis.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/bitrate.c -c -o obj/mix/bitrate.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/block.c -c -o obj/mix/block.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/codebook.c -c -o obj/mix/codebook.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/envelope.c -c -o obj/mix/envelope.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/floor0.c -c -o obj/mix/floor0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/floor1.c -c -o obj/mix/floor1.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/info.c -c -o obj/mix/info.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lookup.c -c -o obj/mix/lookup.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lpc.c -c -o obj/mix/lpc.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/lsp.c -c -o obj/mix/lsp.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/mapping0.c -c -o obj/mix/mapping0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/mdct.c -c -o obj/mix/mdct.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/psy.c -c -o obj/mix/psy.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/registry.c -c -o obj/mix/registry.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/res0.c -c -o obj/mix/res0.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/sharedbook.c -c -o obj/mix/sharedbook.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/smallft.c -c -o obj/mix/smallft.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/synthesis.c -c -o obj/mix/synthesis.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/vorbisenc.c -c -o obj/mix/vorbisenc.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/vorbisfile.c -c -o obj/mix/vorbisfile.o
gcc src/lib/SDL_mixer/external/libvorbis-1.3.3/lib/window.c -c -o obj/mix/window.o
gcc src/lib/SDL_mixer/music_ogg.c -c -o obj/mix/music_ogg.o
gcc src/lib/SDL_mixer/dynamic_ogg.c -c -o obj/mix/dynamic_ogg.o
gcc src/lib/SDL_mixer/load_ogg.c -c -o obj/mix/load_ogg.o
gcc src/lib/SDL_mixer/load_aiff.c -c -o obj/mix/load_aiff.o
gcc src/lib/SDL_mixer/load_voc.c -c -o obj/mix/load_voc.o
gcc src/lib/SDL_mixer/effects_internal.c -c -o obj/mix/effects_internal.o
gcc src/lib/SDL_mixer/music.c -c -o obj/mix/music.o
gcc src/lib/SDL_mixer/effect_position.c -c -o obj/mix/effect_position.o
gcc src/lib/SDL_mixer/mixer.c -c -o obj/mix/mixer.o

cd obj/mix/

printf "[COMP]compiling singular SDL_mixer object file...\n"
ar -cvq ../jlvm_sdl_mixer.a bitwise.o framing.o analysis.o bitrate.o block.o codebook.o envelope.o floor0.o floor1.o info.o  lookup.o lpc.o lsp.o mapping0.o mdct.o psy.o registry.o res0.o sharedbook.o smallft.o synthesis.o vorbisenc.o vorbisfile.o window.o music_ogg.o dynamic_ogg.o load_ogg.o load_aiff.o load_voc.o effects_internal.o music.o effect_position.o mixer.o

cd ..
cd ..

printf "[COMP]compiling separate SDL files...\n"
gcc src/lib/SDL/src/SDL.c -c -o obj/sdl/SDL.o
gcc src/lib/SDL/src/SDL_assert.c -c -o obj/sdl/SDL_assert.o
gcc src/lib/SDL/src/SDL_error.c -c -o obj/sdl/SDL_error.o
gcc src/lib/SDL/src/SDL_hints.c -c -o obj/sdl/SDL_hints.o
gcc src/lib/SDL/src/SDL_log.c -c -o obj/sdl/SDL_log.o
gcc src/lib/SDL/src/audio/SDL_audio.c -c -o obj/sdl/SDL_audio.o
gcc src/lib/SDL/src/audio/SDL_audiocvt.c -c -o obj/sdl/SDL_audiocvt.o
gcc src/lib/SDL/src/audio/SDL_audiodev.c -c -o obj/sdl/SDL_audiodev.o
gcc src/lib/SDL/src/audio/SDL_audiotypecvt.c -c -o obj/sdl/SDL_audiotypecvt.o
gcc src/lib/SDL/src/audio/SDL_mixer.c -c -o obj/sdl/SDL_mixer.o
gcc src/lib/SDL/src/audio/SDL_wave.c -c -o obj/sdl/SDL_wave.o
gcc src/lib/SDL/src/audio/android/*.c -c -o obj/sdl/SDL_android.o
gcc src/lib/SDL/src/audio/dummy/*.c -c -o obj/sdl/SDL_dummy.o
gcc src/lib/SDL/src/atomic/SDL_atomic.c -c -o obj/sdl/SDL_atomic.o
gcc src/lib/SDL/src/atomic/SDL_spinlock.c -c -o obj/sdl/SDL_spinlock.o
gcc src/lib/SDL/src/core/android/*.c -c -o obj/sdl/android.o
gcc src/lib/SDL/src/cpuinfo/*.c -c -o obj/sdl/cpuinfo.o
gcc src/lib/SDL/src/dynapi/*.c -c -o obj/sdl/dynapi.o
gcc src/lib/SDL/src/events/SDL_clipboardevents.c -c -o obj/sdl/SDL_clipboardevents.o
gcc src/lib/SDL/src/events/SDL_dropevents.c -c -o obj/sdl/SDL_dropevents.o
gcc src/lib/SDL/src/events/SDL_events.c -c -o obj/sdl/SDL_events.o
gcc src/lib/SDL/src/events/SDL_gesture.c -c -o obj/sdl/SDL_gesture.o
gcc src/lib/SDL/src/events/SDL_keyboard.c -c -o obj/sdl/SDL_keyboard.o
gcc src/lib/SDL/src/events/SDL_mouse.c -c -o obj/sdl/SDL_mouse.o
gcc src/lib/SDL/src/events/SDL_quit.c -c -o obj/sdl/SDL_quit.o
gcc src/lib/SDL/src/events/SDL_touch.c -c -o obj/sdl/SDL_touch.o
gcc src/lib/SDL/src/events/SDL_windowevents.c -c -o obj/sdl/SDL_windowevents.o
gcc src/lib/SDL/src/file/*.c -c -o obj/sdl/file.o
gcc src/lib/SDL/src/haptic/*.c -c -o obj/sdl/haptic.o
gcc src/lib/SDL/src/haptic/dummy/*.c -c -o obj/sdl/haptic_dummy.o
gcc src/lib/SDL/src/joystick/SDL_gamecontroller.c -c -o obj/sdl/SDL_gamecontroller.o
gcc src/lib/SDL/src/joystick/SDL_joystick.c -c -o obj/sdl/SDL_joystick.o
gcc src/lib/SDL/src/joystick/android/*.c -c -o obj/sdl/SDL_andrjoy.o
gcc src/lib/SDL/src/loadso/dlopen/*.c -c -o obj/sdl/dlopen.o
gcc src/lib/SDL/src/power/*.c -c -o obj/sdl/power.o
gcc src/lib/SDL/src/power/android/*.c -c -o obj/sdl/android.o
gcc src/lib/SDL/src/filesystem/dummy/*.c -c -o obj/sdl/file_dummy.o
gcc src/lib/SDL/src/render/SDL_d3dmath.c -c -o obj/sdl/SDL_d3dmath.o
gcc src/lib/SDL/src/render/SDL_render.c -c -o obj/sdl/SDL_render.o
gcc src/lib/SDL/src/render/SDL_yuv_mmx.c -c -o obj/sdl/SDL_yuv_mmx.o
gcc src/lib/SDL/src/render/SDL_yuv_sw.c -c -o obj/sdl/SDL_yuv_sw.o
gcc src/lib/SDL/src/render/software/SDL_blendfillrect.c -c -o obj/sdl/SDL_blendfillrect.o
gcc src/lib/SDL/src/render/software/SDL_blendline.c -c -o obj/sdl/SDL_blendline.o
gcc src/lib/SDL/src/render/software/SDL_blendpoint.c -c -o obj/sdl/SDL_blendpoint.o
gcc src/lib/SDL/src/render/software/SDL_drawline.c -c -o obj/sdl/SDL_drawline.o
gcc src/lib/SDL/src/render/software/SDL_drawpoint.c -c -o obj/sdl/SDL_drawpoint.o
gcc src/lib/SDL/src/render/software/SDL_render_sw.c -c -o obj/sdl/SDL_render_sw.o
gcc src/lib/SDL/src/render/software/SDL_rotate.c -c -o obj/sdl/SDL_rotate.o
gcc src/lib/SDL/src/stdlib/SDL_getenv.c -c -o obj/sdl/SDL_getenv.o
gcc src/lib/SDL/src/stdlib/SDL_iconv.c -c -o obj/sdl/SDL_iconv.o
gcc src/lib/SDL/src/stdlib/SDL_malloc.c -c -o obj/sdl/SDL_malloc.o
gcc src/lib/SDL/src/stdlib/SDL_qsort.c -c -o obj/sdl/SDL_qsort.o
gcc src/lib/SDL/src/stdlib/SDL_stdlib.c -c -o obj/sdl/SDL_stdlib.o
gcc src/lib/SDL/src/stdlib/SDL_string.c -c -o obj/sdl/SDL_string.o
gcc src/lib/SDL/src/thread/*.c -c -o obj/sdl/thread.o
gcc src/lib/SDL/src/thread/pthread/SDL_syscond.c -c -o obj/sdl/SDL_syscond.o
gcc src/lib/SDL/src/thread/pthread/SDL_sysmutex.c -c -o obj/sdl/SDL_sysmutex.o
gcc src/lib/SDL/src/thread/pthread/SDL_syssem.c -c -o obj/sdl/SDL_syssem.o
gcc src/lib/SDL/src/thread/pthread/SDL_systhread.c -c -o obj/sdl/SDL_systhread.o
gcc src/lib/SDL/src/thread/pthread/SDL_systls.c -c -o obj/sdl/SDL_systls.o
gcc src/lib/SDL/src/timer/SDL_timer.c -c -o obj/sdl/timer.o
gcc src/lib/SDL/src/timer/SDL_systimer.c -c -o obj/sdl/timer_unix.o #OS SPECIFIC
gcc src/lib/SDL/src/video/SDL_blit.c -c -o obj/sdl/SDL_blit.o
gcc src/lib/SDL/src/video/SDL_blit_0.c -c -o obj/sdl/SDL_blit_0.o
gcc src/lib/SDL/src/video/SDL_blit_1.c -c -o obj/sdl/SDL_blit_1.o
gcc src/lib/SDL/src/video/SDL_blit_A.c -c -o obj/sdl/SDL_blit_A.o
gcc src/lib/SDL/src/video/SDL_blit_auto.c -c -o obj/sdl/SDL_blit_auto.o
gcc src/lib/SDL/src/video/SDL_blit_copy.c -c -o obj/sdl/SDL_blit_copy.o
gcc src/lib/SDL/src/video/SDL_blit_N.c -c -o obj/sdl/SDL_blit_N.o
gcc src/lib/SDL/src/video/SDL_blit_slow.c -c -o obj/sdl/SDL_blit_slow.o
gcc src/lib/SDL/src/video/SDL_bmp.c -c -o obj/sdl/SDL_bmp.o
gcc src/lib/SDL/src/video/SDL_clipboard.c -c -o obj/sdl/SDL_clipboard.o
gcc src/lib/SDL/src/video/SDL_egl.c -c -o obj/sdl/SDL_egl.o
gcc src/lib/SDL/src/video/SDL_fillrect.c -c -o obj/sdl/SDL_fillrect.o
gcc src/lib/SDL/src/video/SDL_pixels.c -c -o obj/sdl/SDL_pixels.o
gcc src/lib/SDL/src/video/SDL_rect.c -c -o obj/sdl/SDL_rect.o
gcc src/lib/SDL/src/video/SDL_RLEaccel.c -c -o obj/sdl/SDL_RLEaccel.o
gcc src/lib/SDL/src/video/SDL_shape.c -c -o obj/sdl/SDL_shape.o
gcc src/lib/SDL/src/video/SDL_stretch.c -c -o obj/sdl/SDL_stretch.o
gcc src/lib/SDL/src/video/SDL_surface.c -c -o obj/sdl/SDL_surface.o
gcc src/lib/SDL/src/video/SDL_video.c -c -o obj/sdl/SDL_video.o
#gcc src/lib/SDL/src/video/android/SDL_androidclipboard.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidevents.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidgl.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidkeyboard.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidtouch.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidvideo.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/video/android/SDL_androidwindow.c -c -o obj/sdl/.o
#gcc src/lib/SDL/src/main/android/SDL_android_main.c -c -o obj/sdl/.o
cd obj/sdl/
printf "[COMP]compiling singular SDL object file...\n"
#ar csr ../jlvm_sdl2.o SDL_video.o SDL_surface.o SDL_stretch.o SDL_shape.o SDL_RLEaccel.o SDL_rect.o SDL_pixels.o SDL_fillrect.o SDL_egl.o SDL_clipboard.o SDL_bmp.o SDL_blit_slow.o SDL_blit_N.o SDL_blit_copy.o SDL_blit_auto.o SDL_blit_A.o SDL_blit_1.o SDL_blit_0.o SDL_blit.o timer_unix.o timer.o SDL_systls.o SDL_systhread.o SDL_syssem.o SDL_sysmutex.o SDL_syscond.o thread.o SDL_string.o SDL_stdlib.o SDL_qsort.o SDL_malloc.o SDL_iconv.o SDL_getenv.o SDL_rotate.o SDL_render_sw.o SDL_drawpoint.o SDL_drawline.o SDL_blendpoint.o SDL_blendline.o SDL_blendfillrect.o SDL_yuv_sw.o SDL_yuv_mmx.o SDL_render.o SDL_d3dmath.o file_dummy.o android.o power.o dlopen.o SDL_andrjoy.o SDL_joystick.o SDL_gamecontroller.o haptic_dummy.o haptic.o file.o SDL_windowevents.o SDL_touch.o SDL_quit.o SDL_mouse.o SDL_keyboard.o SDL_gesture.o SDL_events.o SDL_dropevents.o SDL_clipboardevents.o dynapi.o cpuinfo.o android.o SDL_atomic.o SDL_dummy.o SDL_android.o SDL_wave.o SDL_mixer.o SDL_audiotypecvt.o SDL_audiodev.o SDL_audiocvt.o SDL_audio.o SDL_log.o SDL_hints.o SDL_error.o SDL_assert.o SDL.o
printf "[COMP]compiling separate LibZip files...\n"
cd ..


printf "[COMP]copying stuff...\n"
cp jlvm_sdl_mixer.a ../../JLVM_BIN/jlib/jlvm_sdl_mixer.a
cp jlvm_sdl2.o ../../JLVM_BIN/jlib/jlvm_sdl2.o

printf "[COMP]done!\n"
