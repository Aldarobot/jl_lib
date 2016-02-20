# Makefile to build the C version of jl_lib.

ifneq ("$(shell uname | grep Linux)", "")
 ifneq ("$(shell uname -m | grep arm)", "")
  include compile-scripts/rpi.mk
 else
  include compile-scripts/linux.mk
 endif
else
 $(error "Platform is not supported")
endif
#TODO: Darwin is mac OS for uname

HEADER = -Isrc/lib/ -Isrc/lib/include/ -I/opt/vc/include/ -Isrc/C/header/\
	-Isrc/C/include
CFLAGS_MEDIA = $(HEADER) -O3
CFLAGS_DEBUG = $(HEADER) -Wall -g
CFLAGS_RELEASE = $(HEADER) -O3

CC = gcc
CFLAGS = $(CFLAGS_DEBUG)

CLUMP_SRC = src/lib/clump

SRC = src/C
BUILD = build/obj
# 
MODULES = $(subst .c,, $(shell basename -a \
	$(shell find $(SRC)/ -type f -name '*.c') \
	$(shell find $(CLUMP_SRC)/ -type f -name '*.c') \
))
HEADERS = $(shell find $(SRC)/ -type f -name '*.h')
# Special MAKE variable - do not rename.
VPATH = $(shell find $(SRC)/ -type d) $(shell find $(CLUMP_SRC)/ -type d)
#
OBJS = $(addprefix $(BUILD)/, $(addsuffix .o,$(MODULES)))
SHARED = $(BUILD)/jl.so
STATIC = $(BUILD)/jl.a

# Locations
LIBZIP = build/android/jni/src/lib/libzip/
SRC_NDK = android-ndk-r10e

# The Set-Up Options.
init-all: init-build init-deps-all
init-all-wo-android: init-build-wo-android init-deps-most

# The Clean Options.
clean-all: clean-build clean-deps
clean-build:
	# Empty directory: build/obj/
	printf "[COMP] Cleaning up....\n"
	rm -f -r build/obj/
	mkdir -p build/obj/
	rm -f build/*.o
	printf "[COMP] Done!\n"
clean-build-all:
	rm -r build/
clean-build-andr:
	rm -r build/android/
clean-deps:
	rm -r deps/

# The Build Options.
build-depends: src/lib/include/ build-sdl build-libzip build-sdl-net build-sdl-image build-clump
build-android:
	# Copy android build files into android build project.
	cp -u -t build/android/jni/ android-src/*.mk
	cp -u -t build/android/ android-src/*.properties
	cp -u -t build/android/src/org/libsdl/app/ android-src/SDL*.java 
	# Copy SDL2 into android build project
	cp -u android-src/SDL_android_main.c build/android/jni/SDL_android_main.c
	cp -u --recursive -t build/android/jni/src/ src/*
	cp -u --recursive -t build/android/jni/src/lib/sdl/\
	 deps/SDL2-2.0.3/src/*
	rm build/android/jni/src/lib/include/SDL_config.h
	cp build/android/jni/src/lib/include/SDL_config_android.h\
	 build/android/jni/src/lib/include/SDL_config.h
	# Copy Libzip into android project
	cp -u deps/libzip-1.0.1/config.h build/android/jni/src/lib/libzip/
	cp -u --recursive -t build/android/jni/src/lib/libzip/\
	 deps/libzip-1.0.1/lib/*.c
	cp -u --recursive -t build/android/jni/src/lib/libzip/\
	 deps/libzip-1.0.1/lib/*.h
	rm $(LIBZIP)zipwin32.h $(LIBZIP)*win32*.c $(LIBZIP)zip_add.c \
		$(LIBZIP)zip_add_dir.c $(LIBZIP)zip_error_get.c \
		$(LIBZIP)zip_error_get_sys_type.c $(LIBZIP)zip_error_to_str.c \
		$(LIBZIP)zip_file_error_get.c $(LIBZIP)zip_get_file_comment.c \
		$(LIBZIP)zip_get_num_files.c $(LIBZIP)zip_rename.c \
		$(LIBZIP)zip_replace.c $(LIBZIP)zip_set_file_comment.c
	# Copy SDL2_mixer into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/external/\
	 deps/SDL2_mixer-2.0.0/external/*
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/\
	 deps/SDL2_mixer-2.0.0/*.c
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/\
	 deps/SDL2_mixer-2.0.0/*.h
	rm build/android/jni/src/lib/sdl-mixer/play*.c
	# Copy SDL2_image into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-image/\
	 deps/SDL2_image-2.0.0/*
	cp android-src/jconfig.h\
	 build/android/jni/src/lib/sdl-image/external/jpeg-9/jconfig.h
	# Copy SDL2_net into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-net/\
	 deps/SDL2_net-2.0.0/*.c
	cp -u --recursive -t build/android/jni/src/lib/sdl-net/\
	 deps/SDL2_net-2.0.0/*.h
src/lib/include/:
	mkdir -p src/lib/include/
	###

$(BUILD)/%.o: %.c $(HEADERS)
	printf "[COMP] compiling $<....\n"
	$(CC) $(CFLAGS) -o $@ -c $<

build/jl.o: $(BUILD) $(OBJS)
	printf "[COMP] compiling singular jl_lib object file....\n"
	ar csr build/jl.o build/obj/*.o build/deps/*.o

build-notify:
	echo Modules: $(MODULES)
	echo Headers: $(HEADERS)
	echo Folders: $(VPATH)
	printf "[COMP] Building jl_lib for target=$(PLATFORM)\n"

# Build modules.
build-library: build-notify build/jl.o
	# Make "jl.o"
	printf "[COMP] done!\n"

# Lower Level
init-deps-all: init-deps-most init-deps-ndk
init-deps-most: init-deps init-deps-sdl init-deps-sdl-image init-deps-sdl-mixer\
	init-deps-sdl-net init-deps-zip
init-build: init-build-wo-android init-build-android
init-build-wo-android: init-build-folder

init-build-folder:
	mkdir -p build/deps/
	mkdir -p build/obj/

init-build-android:
	mkdir -p build/android/jni/src/
	mkdir -p build/android/src/org/libsdl/app/
	mkdir -p build/android/res/values/
	mkdir -p build/android/res/drawable/
	mkdir -p build/android/gcc/
	cp -t build/android/gcc/ \
	 `find deps/$(SRC_NDK)/toolchains -type f -name '*gcc' | grep 4.9`

init-deps:
	mkdir -p deps/

init-deps-sdl:
	cd deps/ && \
	wget https://www.libsdl.org/release/SDL2-2.0.3.zip && \
	unzip SDL2-2.0.3.zip && \
	rm SDL2-2.0.3.zip

init-deps-sdl-image:
	cd deps/ && \
	wget\
	 https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.0.zip\
	 && \
	unzip SDL2_image-2.0.0.zip && \
	rm SDL2_image-2.0.0.zip

init-deps-sdl-mixer:
	cd deps/ && \
	wget\
	 https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.0.zip\
	 && \
	unzip SDL2_mixer-2.0.0.zip && \
	rm SDL2_mixer-2.0.0.zip

init-deps-sdl-net:
	cd deps/ && \
	wget https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.0.zip\
	 && \
	unzip SDL2_net-2.0.0.zip && \
	rm SDL2_net-2.0.0.zip

init-deps-zip:
	cd deps/ && \
	wget -4 http://www.nih.at/libzip/libzip-1.0.1.tar.gz && \
	tar -xzf libzip-1.0.1.tar.gz && \
	rm libzip-1.0.1.tar.gz

init-deps-ems:
	cd deps/&& \
	wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz && \
	tar -xzf emsdk-portable.tar.gz && \
	rm emsdk-portable.tar.gz

init-deps-ndk:
	cd deps/ && \
	wget http://dl.google.com/android/ndk/$SRC_NDK-linux-x86_64.bin\
	 --progress=bar && \
	chmod a+x $SRC_NDK-linux-x86_64.bin && \
	./$SRC_NDK-linux-x86_64.bin && \
	rm $SRC_NDK-linux-x86_64.bin

init-deps-sdk:
	cd deps/ && \
	wget https://dl.google.com/android/android-sdk_r24.4.1-linux.tgz && \
	tar -zxvf android-sdk_r24.4.1-linux.tgz && \
	rm android-sdk_r24.4.1-linux.tgz && \
	cd android-sdk-linux/ && \
	./tools/android

# Low Level / Build
build-emscripten:
	printf "[COMP] comiling emscripten...\n" && \
	cd deps/emsdk_portable/ && \
	./emsdk update && \
	./emsdk install latest && \
	./emsdk activate latest
build-libzip:
	printf "[COMP] compiling libzip...\n" && \
	cd deps/libzip-1.0.1/ && \
	sh configure && \
	make && \
	ld -r lib/*.o -o ../../build/deps/lib_zip.o && \
	cp lib/*.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
build-sdl-image:
	export PATH=$$PATH:`pwd`/deps/SDL2-2.0.3/usr_local/bin/ && \
	printf "[COMP] compiling SDL_image...\n" && \
	cd deps/SDL2_image-2.0.0/ && \
#	autoreconf -vfi && \
	sh configure && \
	make && \
	ld -r .libs/*.o -o ../../build/deps/lib_SDL_image.o && \
	cp -t ../../src/lib/include/ SDL_image.h external/jpeg-9/jpeglib.h \
		external/jpeg-9/jconfig.h external/jpeg-9/jmorecfg.h && \
	printf "[COMP] done!\n"
build-sdl-net:
	printf "[COMP] compiling SDL_net...\n" && \
	cd deps/SDL2_net-2.0.0/ && \
	export SDL2_CONFIG=`pwd`/../SDL2-2.0.3/usr_local/bin/sdl2-config && \
	sh configure && \
	make && \
	ar csr ../../build/deps/lib_SDL_net.o .libs/*.o && \
	cp SDL_net.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
build-sdl-mixer:
	export PATH=$$PATH:`pwd`/deps/SDL2-2.0.3/usr_local/bin/ && printf "[COMP] compiling SDL_mixer...\n" && \
	cd deps/SDL2_mixer-2.0.0/ && \
	sh configure && \
	make && \
	rm -f build/playmus.o build/playwave.o && \
	printf "[COMP] Linking...\n" && \
	ld -r build/*.o -o ../../build/deps/lib_SDL_mixer.o && \
	cp SDL_mixer.h ../../src/lib/include/SDL_mixer.h && \
	printf "[COMP] done!\n"
build-clump:
	printf "[COMP] compiling clump...\n"
	gcc src/lib/clump/bitarray.c -c -o build/obj/clump_bitarray.o
	gcc src/lib/clump/clump.c -c -o build/obj/clump_clump.o
	gcc src/lib/clump/hash.c -c -o build/obj/clump_hash.o
	gcc src/lib/clump/hcodec.c -c -o build/obj/clump_hcodec.o
	gcc src/lib/clump/list.c -c -o build/obj/clump_list.o
	gcc src/lib/clump/pool.c -c -o build/obj/clump_pool.o
	gcc src/lib/clump/tree.c -c -o build/obj/clump_tree.o
	ar csr build/deps/lib_clump.o build/obj/clump_*.o
	printf "[COMP] done!\n"

################################################################################
