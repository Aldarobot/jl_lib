HEADER = -Isrc/lib/include/
CFLAGS_MEDIA = $(HEADER) -O3
CFLAGS_DEBUG = $(HEADER) $(sdl2-config --cflags) -Wall -g
CFLAGS_RELEASE = $(HEADER) $(sdl2-config --cflags) -O3
CFLAGS = $(CFLAGS_DEBUG)

# The Set-Up Options.
init-all: init-build init-deps-all
init-all-wo-android: init-build-wo-android init-deps-most

# The Clean Options.
clean-all: clean-build clean-deps
clean-build:
	rm -r build/
clean-deps:
	rm -r deps/

# The Build Options.
build-depends: build-libzip build-sdl-net build-sdl-image build-clump
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
	rm build/android/jni/src/lib/libzip/zipwin32.h
	rm build/android/jni/src/lib/libzip/*win32*.c
	# Copy SDL2_mixer into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/external/\
	 deps/SDL2_mixer-2.0.0/external/*
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/\
	 deps/SDL2_mixer-2.0.0/*.c
	cp -u --recursive -t build/android/jni/src/lib/sdl-mixer/\
	 deps/SDL2_mixer-2.0.0/*.h
	# Copy SDL2_image into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-image/\
	 deps/SDL2_image-2.0.0/*.c
	cp -u --recursive -t build/android/jni/src/lib/sdl-image/\
	 deps/SDL2_image-2.0.0/*.h
	# Copy SDL2_net into the android project.
	cp -u --recursive -t build/android/jni/src/lib/sdl-net/\
	 deps/SDL2_net-2.0.0/*.c
	cp -u --recursive -t build/android/jni/src/lib/sdl-net/\
	 deps/SDL2_net-2.0.0/*.h
	###
build-library: 
	# Build modules.
	printf "[COMP] compiling separate jl_lib object files....\n"
	gcc src/me.c -c -o build/obj/me.o $(CFLAGS) #1
	gcc src/cl.c -c -o build/obj/cl.o $(CFLAGS) #2
	gcc src/io.c -c -o build/obj/io.o $(CFLAGS) #3
	gcc src/fl.c -c -o build/obj/fl.o $(CFLAGS) #4
	gcc src/cm.c -c -o build/obj/cm.o $(CFLAGS) #5
	gcc src/ct.c -c -o build/obj/ct.o $(CFLAGS) #6
	gcc src/sg.c -c -o build/obj/sg.o $(CFLAGS) #7
	gcc src/dl.c -c -o build/obj/dl.o $(CFLAGS) #8
	gcc src/gl.c -c -o build/obj/gl.o $(CFLAGS) #9
	gcc src/gr.c -c -o build/obj/gr.o $(CFLAGS) #10
	gcc src/vi.c -c -o build/obj/vi.o $(CFLAGS) #11
	gcc src/au.c -c -o build/obj/au.o $(CFLAGS) #12
	gcc src/Main.c -c -o build/obj/Main.o $(CFLAGS)
	gcc src/gen/media.c -c -o build/obj/media.o $(CFLAGS)
	# Make "jl.o"
	printf "[COMP] compiling singular jl_lib object file....\n"
	ar csr build/jl.o build/obj/*.o
	# Empty directory: build/obj/
	printf "[COMP] cleaning up....\n"
	rm -r build/obj/
	mkdir build/obj/
	printf "[COMP] done!\n"
build-media: compile_media
build-project: compile_es
build-project-android:

# Lower Level
init-deps-all: init-deps-most init-deps-ndk
init-deps-most: init-deps init-deps-sdl init-deps-sdl-image init-deps-sdl-mixer\
	init-deps-sdl-net init-deps-zip
init-build: init-build-wo-android init-build-android
init-build-wo-android: init-build-folder

test:
	make compile_gles --directory=test/
	make run --directory=test/

init-build-folder:
	mkdir build/
	mkdir build/deps/
	mkdir build/obj/

init-build-android:
	mkdir build/android/
	mkdir build/android/jni/
	mkdir build/android/jni/src/
	mkdir build/android/src/
	mkdir build/android/src/org/
	mkdir build/android/src/org/libsdl/
	mkdir build/android/src/org/libsdl/app/
	mkdir build/android/res/
	mkdir build/android/res/values/
	mkdir build/android/res/drawable/

init-deps:
	mkdir deps/

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
	wget http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin\
	 --progress=bar && \
	chmod a+x android-ndk-r10e-linux-x86_64.bin && \
	./android-ndk-r10e-linux-x86_64.bin && \
	rm android-ndk-r10e-linux-x86_64.bin

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
	ar csr ../../build/deps/lib_zip.o lib/*.o && \
	cp lib/zip.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
build-sdl:
	printf "[COMP] compiling SDL...\n" && \
	cd deps/SDL2-2.0.3/ && \
	sh configure && \
	make && \
	ld -r build/.libs/*.o -o ../../build/deps/lib_SDL.o && \
	cp include/*.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
build-sdl-image:
	printf "[COMP] compiling SDL_image...\n" && \
	cd deps/SDL2_image-2.0.0/ && \
	sh configure && \
	make && \
	#ar Tcsr ../../build/deps/lib_SDL_image.o .libs/IMG_*.o  && \
	ld -r .libs/*.o -o ../../build/deps/lib_SDL_image.o && \
	cp SDL_image.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
build-sdl-net:
	printf "[COMP] compiling SDL_net...\n" && \
	cd deps/SDL2_net-2.0.0/ && \
	sh configure && \
	make && \
	ar csr ../../build/deps/lib_SDL_net.o .libs/*.o && \
	cp SDL_net.h ../../src/lib/include/ && \
	#cp SDL_net.h ../../src/lib/include/SDL_net.h && \
	#sed -i "s|\"SDL.h\"|<SDL2/SDL.h>|g" ../../src/lib/include/SDL_net.h && \
	#sed -i "s|\"SDL_endian.h\"|<SDL2/SDL_endian.h>|g"\
	# ../../src/lib/include/SDL_net.h && \
	#sed -i "s|\"SDL_version.h\"|<SDL2/SDL_version.h>|g"\
	# ../../src/lib/include/SDL_net.h && \
	#sed -i "s|\"begin_code.h\"|<SDL2/begin_code.h>|g"\
	# ../../src/lib/include/SDL_net.h && \
	#sed -i "s|\"close_code.h\"|<SDL2/close_code.h>|g"\
	# ../../src/lib/include/SDL_net.h && \
	printf "[COMP] done!\n"
build-sdl-mixer:
	printf "[COMP] compiling SDL_mixer...\n" && \
	cd deps/SDL2_mixer-2.0.0/ && \
	sh configure && \
	make && \
	ar csr ../../build/deps/lib_SDL_mixer.o build/*.o && \
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

# low-level: compile project:
compile_gl: nc_compile_gl__ nc_compile__
compile_es: nc_compile_es__ nc_compile__

compile_media:
	printf "[COMP] compiling media\n"
	gcc media/genr/*.c -c -o media/media.o $(CFLAGS_MEDIA)

nc_compile_gl__:
	printf "[COMP] compiling with OPENGL\n"
	$(eval GL_VERSION=-lGL)
nc_compile_es__:
	printf "[COMP] compiling with OPENGLES Version 2\n"
	$(eval GL_VERSION=-lGLESv2)

nc_compile__:
	printf "[COMP] cleaning up....\n"
	rm -f bin/comp/`sed -n '4p' data.txt`
	printf "[COMP] compiling with GL $(GL_VERSION)....\n"
	gcc\
	 -I`sed -n '2p' pref.txt`/src/include/\
	 -I`sed -n '2p' pref.txt`/src/lib/include/\
	 `sed -n '2p' pref.txt`/build/jl.o `sed -n '2p' pref.txt`/build/deps/*.o\
	 src/*.c media/media.o\
	 -lSDL2 -lpthread -lm -ldl -lpthread -lrt\
	 $(GL_VERSION) -lm -lz\
	 -o bin/comp/`sed -n '4p' data.txt`\
	 -Wall -g
	printf "[COMP] done compiling!\n"

################################################################################
