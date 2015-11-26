#include $(call all-subdir-makefiles)
SRCB := $(call my-dir)
include $(call my-dir)/src/Android.mk
#include $(call my-dir)/SDL_mixer/Android.mk
