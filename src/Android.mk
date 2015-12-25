LOCAL_PATH := $(call my-dir)/

include $(CLEAR_VARS)

LOCAL_MODULE := main

LIB_ZIP := lib/libzip/
LIB_SDL := lib/sdl/
LIB_MIX := lib/sdl-mixer/
LIB_NET := lib/sdl-net/
LIB_IMG := lib/sdl-image/

LOCAL_C_INCLUDES :=

# Load The Programmers C & media files
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH),,\
	$(wildcard $(LOCAL_PATH)gen/med/*.c)\
	$(wildcard $(LOCAL_PATH)gen/src/*.c)\
)

# Load The Default Graphics
LOCAL_SRC_FILES += gen/media.c

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
LOCAL_C_INCLUDES += $(LOCAL_PATH)$(LIB_IMG)external/jpeg-9\
	$(LOCAL_PATH)$(LIB_IMG)external/libpng-1.6.2\

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
	$(wildcard $(LOCAL_PATH)lib/libzip/*.c) \
)

#load SDL_mixer
LOCAL_SRC_FILES +=\
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

# Load SDL_net


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

LOCAL_CFLAGS += -Ijni/src/include/ -Ijni/src/lib/include/ -DGL_GLEXT_PROTOTYPES -DHAVE_CONFIG_H -DNDEBUG -O3

LOCAL_C_INCLUDES +=\
	jni/src/lib/libzip/\
	jni/src/lib/sdl-mixer/external/libogg-1.3.1/include/\
	jni/src/lib/sdl-mixer/external/libvorbis-1.3.3/include/\
	jni/src/lib/sdl-mixer/external/libvorbis-1.3.3/lib/

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS := -Wl,--undefined=Java_org_libsdl_app_SDLActivity_nativeInit -ldl -lGLESv1_CM -lz -lEGL -lGLESv2 -llog -landroid

APP_OPTIM := release

include $(BUILD_SHARED_LIBRARY)
#$(call import-module,SDL)LOCAL_PATH := $(call my-dir)
#$(call import-module,SDL_mixer)LOCAL_PATH := $(call my-dir)
