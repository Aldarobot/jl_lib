LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

LIB_ZIP := lib/libzip-0.11.2/lib/
LIB_SDL := lib/sdl/src/
LIB_MIX := lib/SDL_mixer/

#load SDL
LOCAL_SRC_FILES += \
	$(LIB_SDL)SDL.c\
	$(LIB_SDL)SDL_assert.c \
	$(LIB_SDL)SDL_error.c \
	$(LIB_SDL)SDL_hints.c \
	$(LIB_SDL)SDL_log.c \
	$(LIB_SDL)audio/SDL_audio.c \
	$(LIB_SDL)audio/SDL_audiocvt.c \
	$(LIB_SDL)audio/SDL_audiodev.c \
	$(LIB_SDL)audio/SDL_audiotypecvt.c \
	$(LIB_SDL)audio/SDL_mixer.c \
	$(LIB_SDL)audio/SDL_wave.c \
	$(LIB_SDL)audio/android/*.c \
	$(LIB_SDL)audio/dummy/*.c \
	$(LIB_SDL)atomic/SDL_atomic.c \
	$(LIB_SDL)atomic/SDL_spinlock.c.arm \
	$(LIB_SDL)core/android/*.c \
	$(LIB_SDL)cpuinfo/*.c \
	$(LIB_SDL)dynapi/*.c \
	$(LIB_SDL)events/SDL_clipboardevents.c \
	$(LIB_SDL)events/SDL_dropevents.c \
	$(LIB_SDL)events/SDL_events.c \
	$(LIB_SDL)events/SDL_gesture.c \
	$(LIB_SDL)events/SDL_keyboard.c \
	$(LIB_SDL)events/SDL_mouse.c \
	$(LIB_SDL)events/SDL_quit.c \
	$(LIB_SDL)events/SDL_touch.c \
	$(LIB_SDL)events/SDL_windowevents.c \
	$(LIB_SDL)file/*.c \
	$(LIB_SDL)haptic/*.c \
	$(LIB_SDL)haptic/dummy/*.c \
	$(LIB_SDL)joystick/SDL_gamecontroller.c \
	$(LIB_SDL)joystick/SDL_joystick.c \
	$(LIB_SDL)joystick/android/*.c \
	$(LIB_SDL)loadso/dlopen/*.c \
	$(LIB_SDL)power/*.c \
	$(LIB_SDL)power/android/*.c \
	$(LIB_SDL)filesystem/dummy/*.c \
	$(LIB_SDL)render/SDL_d3dmath.c \
	$(LIB_SDL)render/SDL_render.c \
	$(LIB_SDL)render/SDL_yuv_mmx.c \
	$(LIB_SDL)render/SDL_yuv_sw.c \
	$(LIB_SDL)render/software/SDL_blendfillrect.c \
	$(LIB_SDL)render/software/SDL_blendline.c \
	$(LIB_SDL)render/software/SDL_blendpoint.c \
	$(LIB_SDL)render/software/SDL_drawline.c \
	$(LIB_SDL)render/software/SDL_drawpoint.c \
	$(LIB_SDL)render/software/SDL_render_sw.c \
	$(LIB_SDL)render/software/SDL_rotate.c \
	$(LIB_SDL)stdlib/SDL_getenv.c \
	$(LIB_SDL)stdlib/SDL_iconv.c \
	$(LIB_SDL)stdlib/SDL_malloc.c \
	$(LIB_SDL)stdlib/SDL_qsort.c \
	$(LIB_SDL)stdlib/SDL_stdlib.c \
	$(LIB_SDL)stdlib/SDL_string.c \
	$(LIB_SDL)thread/*.c \
	$(LIB_SDL)thread/pthread/SDL_syscond.c \
	$(LIB_SDL)thread/pthread/SDL_sysmutex.c \
	$(LIB_SDL)thread/pthread/SDL_syssem.c \
	$(LIB_SDL)thread/pthread/SDL_systhread.c \
	$(LIB_SDL)thread/pthread/SDL_systls.c \
	$(LIB_SDL)timer/SDL_timer.c \
	$(LIB_SDL)timer/unix/SDL_systimer.c \
	$(LIB_SDL)video/SDL_blit.c \
	$(LIB_SDL)video/SDL_blit_0.c \
	$(LIB_SDL)video/SDL_blit_1.c \
	$(LIB_SDL)video/SDL_blit_A.c \
	$(LIB_SDL)video/SDL_blit_auto.c \
	$(LIB_SDL)video/SDL_blit_copy.c \
	$(LIB_SDL)video/SDL_blit_N.c \
	$(LIB_SDL)video/SDL_blit_slow.c \
	$(LIB_SDL)video/SDL_bmp.c \
	$(LIB_SDL)video/SDL_clipboard.c \
	$(LIB_SDL)video/SDL_egl.c \
	$(LIB_SDL)video/SDL_fillrect.c \
	$(LIB_SDL)video/SDL_pixels.c \
	$(LIB_SDL)video/SDL_rect.c \
	$(LIB_SDL)video/SDL_RLEaccel.c \
	$(LIB_SDL)video/SDL_shape.c \
	$(LIB_SDL)video/SDL_stretch.c \
	$(LIB_SDL)video/SDL_surface.c \
	$(LIB_SDL)video/SDL_video.c \
	$(LIB_SDL)video/android/SDL_androidclipboard.c \
	$(LIB_SDL)video/android/SDL_androidevents.c \
	$(LIB_SDL)video/android/SDL_androidgl.c \
	$(LIB_SDL)video/android/SDL_androidkeyboard.c \
	$(LIB_SDL)video/android/SDL_androidtouch.c \
	$(LIB_SDL)video/android/SDL_androidvideo.c \
	$(LIB_SDL)video/android/SDL_androidwindow.c \
	$(LIB_SDL)main/android/SDL_android_main.c

#load libzip
LOCAL_SRC_FILES += \
	$(LIB_ZIP)mkstemp.c \
	$(LIB_ZIP)zip_add.c \
	$(LIB_ZIP)zip_new.c \
	$(LIB_ZIP)zip_open.c \
	$(LIB_ZIP)zip_stat.c \
	$(LIB_ZIP)zip_close.c \
	$(LIB_ZIP)zip_entry.c \
	$(LIB_ZIP)zip_error.c \
	$(LIB_ZIP)zip_fopen.c \
	$(LIB_ZIP)zip_fread.c \
	$(LIB_ZIP)zip_utf-8.c \
	$(LIB_ZIP)zip_delete.c \
	$(LIB_ZIP)zip_dirent.c \
	$(LIB_ZIP)zip_fdopen.c \
	$(LIB_ZIP)zip_fclose.c \
	$(LIB_ZIP)zip_memdup.c \
	$(LIB_ZIP)zip_rename.c \
	$(LIB_ZIP)zip_string.c \
	$(LIB_ZIP)zip_add_dir.c \
	$(LIB_ZIP)zip_dir_add.c \
	$(LIB_ZIP)zip_discard.c \
	$(LIB_ZIP)zip_err_str.c \
	$(LIB_ZIP)zip_replace.c \
	$(LIB_ZIP)zip_file_add.c \
	$(LIB_ZIP)zip_get_name.c \
	$(LIB_ZIP)zip_set_name.c \
	$(LIB_ZIP)zip_strerror.c \
	$(LIB_ZIP)zip_unchange.c \
	$(LIB_ZIP)zip_add_entry.c \
	$(LIB_ZIP)zip_error_get.c \
	$(LIB_ZIP)zip_stat_init.c \
	$(LIB_ZIP)zip_source_crc.c \
	$(LIB_ZIP)zip_source_pop.c \
	$(LIB_ZIP)zip_source_zip.c \
	$(LIB_ZIP)zip_stat_index.c \
	$(LIB_ZIP)zip_error_clear.c \
	$(LIB_ZIP)zip_extra_field.c \
	$(LIB_ZIP)zip_file_rename.c \
	$(LIB_ZIP)zip_fopen_index.c \
	$(LIB_ZIP)zip_name_locate.c \
	$(LIB_ZIP)zip_source_file.c \
	$(LIB_ZIP)zip_source_free.c \
	$(LIB_ZIP)zip_source_open.c \
	$(LIB_ZIP)zip_source_read.c \
	$(LIB_ZIP)zip_source_stat.c \
	$(LIB_ZIP)zip_error_to_str.c \
	$(LIB_ZIP)zip_file_replace.c \
	$(LIB_ZIP)zip_source_close.c \
	$(LIB_ZIP)zip_source_error.c \
	$(LIB_ZIP)zip_source_filep.c \
	$(LIB_ZIP)zip_unchange_all.c \
	$(LIB_ZIP)zip_filerange_crc.c \
	$(LIB_ZIP)zip_file_strerror.c \
	$(LIB_ZIP)zip_get_num_files.c \
	$(LIB_ZIP)zip_source_buffer.c \
	$(LIB_ZIP)zip_source_pkware.c \
	$(LIB_ZIP)zip_source_window.c \
	$(LIB_ZIP)zip_unchange_data.c \
	$(LIB_ZIP)zip_error_strerror.c \
	$(LIB_ZIP)zip_file_error_get.c \
	$(LIB_ZIP)zip_source_deflate.c \
	$(LIB_ZIP)zip_source_layered.c \
	$(LIB_ZIP)zip_source_zip_new.c \
	$(LIB_ZIP)zip_extra_field_api.c \
	$(LIB_ZIP)zip_file_get_offset.c \
	$(LIB_ZIP)zip_fopen_encrypted.c \
	$(LIB_ZIP)zip_get_num_entries.c \
	$(LIB_ZIP)zip_source_function.c \
	$(LIB_ZIP)zip_file_error_clear.c \
	$(LIB_ZIP)zip_file_get_comment.c \
	$(LIB_ZIP)zip_file_set_comment.c \
	$(LIB_ZIP)zip_get_archive_flag.c \
	$(LIB_ZIP)zip_get_file_comment.c \
	$(LIB_ZIP)zip_set_archive_flag.c \
	$(LIB_ZIP)zip_set_file_comment.c \
	$(LIB_ZIP)zip_unchange_archive.c \
	$(LIB_ZIP)zip_error_get_sys_type.c \
	$(LIB_ZIP)zip_get_archive_comment.c \
	$(LIB_ZIP)zip_set_archive_comment.c \
	$(LIB_ZIP)zip_set_default_password.c \
	$(LIB_ZIP)zip_set_file_compression.c \
	$(LIB_ZIP)zip_fopen_index_encrypted.c \
	$(LIB_ZIP)zip_file_get_external_attributes.c \
	$(LIB_ZIP)zip_file_set_external_attributes.c \
	$(LIB_ZIP)zip_get_encryption_implementation.c \
	$(LIB_ZIP)zip_get_compression_implementation.c

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
	me.c cl.c io.c fl.c cm.c ct.c\
	sg.c dl.c gl.c gr.c vi.c au.c

# Load The Programmers C files
LOCAL_SRC_FILES +=\
	gen/jlvm_hack_main.c\
	gen/usrc/main.c

LOCAL_CFLAGS += -DGL_GLEXT_PROTOTYPES

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS := -Wl -lz -ldl -lGLESv2 -llog -landroid

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
#$(call import-module,SDL_mixer)LOCAL_PATH := $(call my-dir)
