LOCAL_PATH := $(call my-dir)/

include $(CLEAR_VARS)

LOCAL_MODULE := main

LIB_ZIP := ../lib/libzip/
LIB_SDL := ../lib/sdl/
LIB_MIX := ../lib/sdl-mixer/
LIB_NET := ../lib/sdl-net/
LIB_IMG := ../lib/sdl-image/
LIB_CLM := ../lib/clump/

LOCAL_C_INCLUDES := $(shell find jni/src/C/ -type d)

# Load The Programmers C & media files
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)gen/med/*.c)\
	$(wildcard $(LOCAL_PATH)gen/src/*.c)\
)

# Load The JL_Lib
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)*.c)\
	$(wildcard $(LOCAL_PATH)jl_audio/*.c)\
	$(wildcard $(LOCAL_PATH)jl_comm/*.c)\
	$(wildcard $(LOCAL_PATH)jl_graphics/*.c)\
)

# Load SDL_image
LOCAL_SRC_FILES +=\
	$(LIB_IMG)external/libpng-1.6.2/png.c \
	$(LIB_IMG)external/libpng-1.6.2/pngerror.c \
	$(LIB_IMG)external/libpng-1.6.2/pngget.c \
	$(LIB_IMG)external/libpng-1.6.2/pngmem.c \
	$(LIB_IMG)external/libpng-1.6.2/pngpread.c \
	$(LIB_IMG)external/libpng-1.6.2/pngread.c \
	$(LIB_IMG)external/libpng-1.6.2/pngrio.c \
	$(LIB_IMG)external/libpng-1.6.2/pngrtran.c \
	$(LIB_IMG)external/libpng-1.6.2/pngrutil.c \
	$(LIB_IMG)external/libpng-1.6.2/pngset.c \
	$(LIB_IMG)external/libpng-1.6.2/pngtrans.c \
	$(LIB_IMG)external/libpng-1.6.2/pngwio.c \
	$(LIB_IMG)external/libpng-1.6.2/pngwrite.c \
	$(LIB_IMG)external/libpng-1.6.2/pngwtran.c \
	$(LIB_IMG)external/libpng-1.6.2/pngwutil.c \
	$(LIB_IMG)external/jpeg-9/jaricom.c \
	$(LIB_IMG)external/jpeg-9/jcapimin.c \
	$(LIB_IMG)external/jpeg-9/jcapistd.c \
	$(LIB_IMG)external/jpeg-9/jcarith.c \
	$(LIB_IMG)external/jpeg-9/jccoefct.c \
	$(LIB_IMG)external/jpeg-9/jccolor.c \
	$(LIB_IMG)external/jpeg-9/jcdctmgr.c \
	$(LIB_IMG)external/jpeg-9/jchuff.c \
	$(LIB_IMG)external/jpeg-9/jcinit.c \
	$(LIB_IMG)external/jpeg-9/jcmainct.c \
	$(LIB_IMG)external/jpeg-9/jcmarker.c \
	$(LIB_IMG)external/jpeg-9/jcmaster.c \
	$(LIB_IMG)external/jpeg-9/jcomapi.c \
	$(LIB_IMG)external/jpeg-9/jcparam.c \
	$(LIB_IMG)external/jpeg-9/jcprepct.c \
	$(LIB_IMG)external/jpeg-9/jcsample.c \
	$(LIB_IMG)external/jpeg-9/jctrans.c \
	$(LIB_IMG)external/jpeg-9/jdapimin.c \
	$(LIB_IMG)external/jpeg-9/jdapistd.c \
	$(LIB_IMG)external/jpeg-9/jdarith.c \
	$(LIB_IMG)external/jpeg-9/jdatadst.c \
	$(LIB_IMG)external/jpeg-9/jdatasrc.c \
	$(LIB_IMG)external/jpeg-9/jdcoefct.c \
	$(LIB_IMG)external/jpeg-9/jdcolor.c \
	$(LIB_IMG)external/jpeg-9/jddctmgr.c \
	$(LIB_IMG)external/jpeg-9/jdhuff.c \
	$(LIB_IMG)external/jpeg-9/jdinput.c \
	$(LIB_IMG)external/jpeg-9/jdmainct.c \
	$(LIB_IMG)external/jpeg-9/jdmarker.c \
	$(LIB_IMG)external/jpeg-9/jdmaster.c \
	$(LIB_IMG)external/jpeg-9/jdmerge.c \
	$(LIB_IMG)external/jpeg-9/jdpostct.c \
	$(LIB_IMG)external/jpeg-9/jdsample.c \
	$(LIB_IMG)external/jpeg-9/jdtrans.c \
	$(LIB_IMG)external/jpeg-9/jerror.c \
	$(LIB_IMG)external/jpeg-9/jfdctflt.c \
	$(LIB_IMG)external/jpeg-9/jfdctfst.c \
	$(LIB_IMG)external/jpeg-9/jfdctint.c \
	$(LIB_IMG)external/jpeg-9/jidctflt.c \
	$(LIB_IMG)external/jpeg-9/jidctfst.S \
	$(LIB_IMG)external/jpeg-9/jidctint.c \
	$(LIB_IMG)external/jpeg-9/jquant1.c \
	$(LIB_IMG)external/jpeg-9/jquant2.c \
	$(LIB_IMG)external/jpeg-9/jutils.c \
	$(LIB_IMG)external/jpeg-9/jmemmgr.c \
	$(LIB_IMG)external/jpeg-9/jmem-android.c
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)$(LIB_IMG)IMG*.c)\
)
LOCAL_CFLAGS := -DLOAD_BMP -DLOAD_GIF -DLOAD_LBM -DLOAD_PCX -DLOAD_PNM \
	-DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV -DLOAD_JPG \
	-DLOAD_PNG
LOCAL_CFLAGS += -fstrict-aliasing -DAVOID_TABLES -DPNG_NO_CONFIG_H
LOCAL_C_INCLUDES +=\
	$(LOCAL_PATH)$(LIB_IMG)external/jpeg-9\
	$(LOCAL_PATH)$(LIB_IMG)external/libpng-1.6.2

#load SDL_mixer
LOCAL_CFLAGS += -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1\
 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1\
 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_DLFCN_H=1\
 -D_THREAD_SAFE -DTHREADED_AUDIO -DNOCONTROLS
LOCAL_CFLAGS += -DOGG_MUSIC -DOGG_USE_TREMOR -DMP3_MUSIC -DWAV_MUSIC
LOCAL_C_INCLUDES +=\
	jni/src/lib/sdl-mixer/external/smpeg2-2.0.0\
	jni/src/lib/sdl-mixer/external/libvorbisidec-1.2.1\
	jni/src/lib/sdl-mixer/external/libogg-1.3.1/include/
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)$(LIB_MIX)*.c)\
)
LOCAL_SRC_FILES +=\
	$(LIB_MIX)external/smpeg2-2.0.0/smpeg.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/MPEG.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/MPEGlist.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/MPEGring.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/MPEGstream.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/MPEGsystem.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/MPEGaudio.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/bitwindow.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/filter.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/filter_2.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/hufftable.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/mpeglayer1.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/mpeglayer2.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/mpeglayer3.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/mpegtable.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/audio/mpegtoraw.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/MPEGvideo.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/decoders.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/floatdct.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/gdith.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/jrevdct.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/motionvec.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/parseblock.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/readfile.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/util.cpp \
	$(LIB_MIX)external/smpeg2-2.0.0/video/video.cpp\
	$(LIB_MIX)external/libvorbisidec-1.2.1/mdct.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/block.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/window.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/synthesis.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/info.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/floor1.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/floor0.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/vorbisfile.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/res012.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/mapping0.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/registry.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/codebook.c \
	$(LIB_MIX)external/libvorbisidec-1.2.1/sharedbook.c \
	$(LIB_MIX)external/libogg-1.3.1/src/bitwise.c\
	$(LIB_MIX)external/libogg-1.3.1/src/framing.c

# Load SDL
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)$(LIB_SDL)render/opengles/*.c)\
	$(wildcard $(LOCAL_PATH)$(LIB_SDL)render/opengles2/*.c)\
)
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
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),, \
	$(wildcard $(LOCAL_PATH)$(LIB_ZIP)*.c) \
)

# Load SDL_net


# Load Clump
LOCAL_SRC_FILES +=\
	$(LIB_CLM)/clump.c\
	$(LIB_CLM)/pool.c\
	$(LIB_CLM)/list.c\
	$(LIB_CLM)/hash.c\
	$(LIB_CLM)/tree.c\
	$(LIB_CLM)/bitarray.c\
	$(LIB_CLM)/hcodec.c

LOCAL_CFLAGS += -Ijni/src/include/ -Ijni/src/lib/include/ -DGL_GLEXT_PROTOTYPES -DHAVE_CONFIG_H -DNDEBUG -O3 -Ijni/src/lib/sdl-mixer/external/libvorbis-1.3.3/include/ -Ijni/src/lib/sdl-mixer/external/libvorbis-1.3.3/lib/ -Ijni/src/lib/sdl-image/external/jpeg-9/

LOCAL_C_INCLUDES +=\
	jni/src/lib/libzip/

LOCAL_LDLIBS := -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit -ldl -lGLESv1_CM -lz -lEGL -lGLESv2 -llog -landroid

APP_OPTIM := release

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
#$(call import-module,SDL_mixer)LOCAL_PATH := $(call my-dir)
