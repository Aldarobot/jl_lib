LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LIB_ZIP := lib/libzip/jni/
LIB_SDL := lib/SDL/
LIB_MIX := lib/SDL_mixer/

LOCAL_SRC_FILES := gen/main.c

#load SDL
LOCAL_SRC_FILES += \
	$(LIB_SDL)src/SDL.c \
	$(LIB_SDL)src/SDL_assert.c \
	$(LIB_SDL)src/SDL_error.c \
	$(LIB_SDL)src/SDL_hints.c \
	$(LIB_SDL)src/SDL_log.c \
	$(LIB_SDL)src/audio/SDL_audio.c \
	$(LIB_SDL)src/audio/SDL_audiocvt.c \
	$(LIB_SDL)src/audio/SDL_audiodev.c \
	$(LIB_SDL)src/audio/SDL_audiotypecvt.c \
	$(LIB_SDL)src/audio/SDL_mixer.c \
	$(LIB_SDL)src/audio/SDL_wave.c \
	$(LIB_SDL)src/audio/android/*.c \
	$(LIB_SDL)src/audio/dummy/*.c \
	$(LIB_SDL)src/atomic/SDL_atomic.c \
	$(LIB_SDL)src/atomic/SDL_spinlock.c.arm \
	$(LIB_SDL)src/core/android/*.c \
	$(LIB_SDL)src/cpuinfo/*.c \
	$(LIB_SDL)src/dynapi/*.c \
	$(LIB_SDL)src/events/SDL_clipboardevents.c \
	$(LIB_SDL)src/events/SDL_dropevents.c \
	$(LIB_SDL)src/events/SDL_events.c \
	$(LIB_SDL)src/events/SDL_gesture.c \
	$(LIB_SDL)src/events/SDL_keyboard.c \
	$(LIB_SDL)src/events/SDL_mouse.c \
	$(LIB_SDL)src/events/SDL_quit.c \
	$(LIB_SDL)src/events/SDL_touch.c \
	$(LIB_SDL)src/events/SDL_windowevents.c \
	$(LIB_SDL)src/file/*.c \
	$(LIB_SDL)src/haptic/*.c \
	$(LIB_SDL)src/haptic/dummy/*.c \
	$(LIB_SDL)src/joystick/SDL_gamecontroller.c \
	$(LIB_SDL)src/joystick/SDL_joystick.c \
	$(LIB_SDL)src/joystick/android/*.c \
	$(LIB_SDL)src/loadso/dlopen/*.c \
	$(LIB_SDL)src/power/*.c \
	$(LIB_SDL)src/power/android/*.c \
	$(LIB_SDL)src/filesystem/dummy/*.c \
	$(LIB_SDL)src/render/SDL_d3dmath.c \
	$(LIB_SDL)src/render/SDL_render.c \
	$(LIB_SDL)src/render/SDL_yuv_mmx.c \
	$(LIB_SDL)src/render/SDL_yuv_sw.c \
	$(LIB_SDL)src/render/software/SDL_blendfillrect.c \
	$(LIB_SDL)src/render/software/SDL_blendline.c \
	$(LIB_SDL)src/render/software/SDL_blendpoint.c \
	$(LIB_SDL)src/render/software/SDL_drawline.c \
	$(LIB_SDL)src/render/software/SDL_drawpoint.c \
	$(LIB_SDL)src/render/software/SDL_render_sw.c \
	$(LIB_SDL)src/render/software/SDL_rotate.c \
	$(LIB_SDL)src/stdlib/SDL_getenv.c \
	$(LIB_SDL)src/stdlib/SDL_iconv.c \
	$(LIB_SDL)src/stdlib/SDL_malloc.c \
	$(LIB_SDL)src/stdlib/SDL_qsort.c \
	$(LIB_SDL)src/stdlib/SDL_stdlib.c \
	$(LIB_SDL)src/stdlib/SDL_string.c \
	$(LIB_SDL)src/thread/*.c \
	$(LIB_SDL)src/thread/pthread/SDL_syscond.c \
	$(LIB_SDL)src/thread/pthread/SDL_sysmutex.c \
	$(LIB_SDL)src/thread/pthread/SDL_syssem.c \
	$(LIB_SDL)src/thread/pthread/SDL_systhread.c \
	$(LIB_SDL)src/thread/pthread/SDL_systls.c \
	$(LIB_SDL)src/timer/*.c \
	$(LIB_SDL)src/timer/unix/*.c \
	$(LIB_SDL)src/video/SDL_blit.c \
	$(LIB_SDL)src/video/SDL_blit_0.c \
	$(LIB_SDL)src/video/SDL_blit_1.c \
	$(LIB_SDL)src/video/SDL_blit_A.c \
	$(LIB_SDL)src/video/SDL_blit_auto.c \
	$(LIB_SDL)src/video/SDL_blit_copy.c \
	$(LIB_SDL)src/video/SDL_blit_N.c \
	$(LIB_SDL)src/video/SDL_blit_slow.c \
	$(LIB_SDL)src/video/SDL_bmp.c \
	$(LIB_SDL)src/video/SDL_clipboard.c \
	$(LIB_SDL)src/video/SDL_egl.c \
	$(LIB_SDL)src/video/SDL_fillrect.c \
	$(LIB_SDL)src/video/SDL_pixels.c \
	$(LIB_SDL)src/video/SDL_rect.c \
	$(LIB_SDL)src/video/SDL_RLEaccel.c \
	$(LIB_SDL)src/video/SDL_shape.c \
	$(LIB_SDL)src/video/SDL_stretch.c \
	$(LIB_SDL)src/video/SDL_surface.c \
	$(LIB_SDL)src/video/SDL_video.c \
	$(LIB_SDL)src/video/android/SDL_androidclipboard.c \
	$(LIB_SDL)src/video/android/SDL_androidevents.c \
	$(LIB_SDL)src/video/android/SDL_androidgl.c \
	$(LIB_SDL)src/video/android/SDL_androidkeyboard.c \
	$(LIB_SDL)src/video/android/SDL_androidtouch.c \
	$(LIB_SDL)src/video/android/SDL_androidvideo.c \
	$(LIB_SDL)src/video/android/SDL_androidwindow.c \
	$(LIB_SDL)src/main/android/SDL_android_main.c

#load libzip
LOCAL_SRC_FILES += \
	$(LIB_ZIP)zip_add.c \
	$(LIB_ZIP)zip_add_dir.c \
	$(LIB_ZIP)zip_close.c \
	$(LIB_ZIP)zip_delete.c \
	$(LIB_ZIP)zip_dirent.c \
	$(LIB_ZIP)zip_entry_free.c \
	$(LIB_ZIP)zip_entry_new.c \
	$(LIB_ZIP)zip_err_str.c \
	$(LIB_ZIP)zip_error.c \
	$(LIB_ZIP)zip_error_clear.c \
	$(LIB_ZIP)zip_error_get.c \
	$(LIB_ZIP)zip_error_get_sys_type.c \
	$(LIB_ZIP)zip_error_strerror.c \
	$(LIB_ZIP)zip_error_to_str.c \
	$(LIB_ZIP)zip_fclose.c \
	$(LIB_ZIP)zip_file_error_clear.c \
	$(LIB_ZIP)zip_file_error_get.c \
	$(LIB_ZIP)zip_file_get_offset.c \
	$(LIB_ZIP)zip_file_strerror.c \
	$(LIB_ZIP)zip_filerange_crc.c \
	$(LIB_ZIP)zip_fopen.c \
	$(LIB_ZIP)zip_fopen_index.c \
	$(LIB_ZIP)zip_fread.c \
	$(LIB_ZIP)zip_free.c \
	$(LIB_ZIP)zip_get_archive_comment.c \
	$(LIB_ZIP)zip_get_archive_flag.c \
	$(LIB_ZIP)zip_get_file_comment.c \
	$(LIB_ZIP)zip_get_num_files.c \
	$(LIB_ZIP)zip_get_name.c \
	$(LIB_ZIP)zip_memdup.c \
	$(LIB_ZIP)zip_name_locate.c \
	$(LIB_ZIP)zip_new.c \
	$(LIB_ZIP)zip_open.c \
	$(LIB_ZIP)zip_rename.c \
	$(LIB_ZIP)zip_replace.c \
	$(LIB_ZIP)zip_set_archive_comment.c \
	$(LIB_ZIP)zip_set_archive_flag.c \
	$(LIB_ZIP)zip_set_file_comment.c \
	$(LIB_ZIP)zip_source_buffer.c \
	$(LIB_ZIP)zip_source_file.c \
	$(LIB_ZIP)zip_source_filep.c \
	$(LIB_ZIP)zip_source_free.c \
	$(LIB_ZIP)zip_source_function.c \
	$(LIB_ZIP)zip_source_zip.c \
	$(LIB_ZIP)zip_set_name.c \
	$(LIB_ZIP)zip_stat.c \
	$(LIB_ZIP)zip_stat_index.c \
	$(LIB_ZIP)zip_stat_init.c \
	$(LIB_ZIP)zip_strerror.c \
	$(LIB_ZIP)zip_unchange.c \
	$(LIB_ZIP)zip_unchange_all.c \
	$(LIB_ZIP)zip_unchange_archive.c \
	$(LIB_ZIP)zip_unchange_data.c

#load SDL_mixer
LOCAL_SRC_FILES += \
	$(LIB_MIX)external/libogg-1.3.1/src/bitwise.c\
	$(LIB_MIX)external/libogg-1.3.1/src/framing.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/analysis.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/bitrate.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/block.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/codebook.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/envelope.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/floor0.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/floor1.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/info.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/lookup.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/lpc.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/lsp.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/mapping0.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/mdct.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/psy.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/registry.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/res0.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/sharedbook.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/smallft.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/synthesis.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/tone.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/vorbisenc.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/vorbisfile.c\
	$(LIB_MIX)external/libvorbis-1.3.3/lib/window.c\
	$(LIB_MIX)music_ogg.c\
	$(LIB_MIX)dynamic_ogg.c\
	$(LIB_MIX)load_ogg.c\
	$(LIB_MIX)load_aiff.c\
	$(LIB_MIX)load_voc.c\
	$(LIB_MIX)effects_internal.c\
	$(LIB_MIX)music.c\
	$(LIB_MIX)effect_position.c\
	$(LIB_MIX)mixer.c

# Load Clump
LOCAL_SRC_FILES +=\
	lib/clump/clump.c\
	lib/clump/pool.c\
	lib/clump/list.c\
	lib/clump/hash.c\
	lib/clump/tree.c\
	lib/clump/bitarray.c\
	lib/clump/hcodec.c

# Load JLVM
LOCAL_SRC_FILES +=\
	amem.c clmp.c siop.c file.c comm.c inpt.c\
	sgrp.c lsdl.c eogl.c grph.c vide.c audi.c

# Load The User's Program Merged Into One File
LOCAL_SRC_FILES +=\
	gen/hack_user_main.c

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS := -Wl -lz -ldl -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
#$(call import-module,SDL_mixer)LOCAL_PATH := $(call my-dir)
