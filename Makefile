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
build-android: android
build-project: 
build-library: 
build-depends: build-libzip build-sdl-net build-sdl-image

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

init-deps-ndk:
	cd deps/ && \
	wget http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin\
	 --progress=bar && \
	chmod a+x android-ndk-r10e-linux-x86_64.bin && \
	./android-ndk-r10e-linux-x86_64.bin && \
	rm android-ndk-r10e-linux-x86_64.bin

init-deps-sdk:
	cd deps/ && \
	wget http://dl.google.com/android/android-sdk_r24.4-linux.tgz && \
	tar -zxvf android-sdk_r24.4-linux.tgz && \
	rm android-sdk_r24.4-linux.tgz

android:
	cp --recursive -u -t build/android/jni/src/ src/*

# Low Level / Build
build-libzip:
	printf "[COMP] compiling libzip...\n" && \
	cd deps/libzip-1.0.1/ && \
	sh configure && \
	make && \
	ar csr ../../build/deps/lib_zip.o lib/*.o && \
	cp lib/zip.h ../../src/lib/zip.h && \
	printf "[COMP] done!\n"
build-sdl-image:
	printf "[COMP] compiling SDL_image...\n" && \
	cd deps/SDL2_image-2.0.0/ && \
	sh configure && \
	make && \
	rm showimage.o && \
	ar csr ../../build/deps/lib_SDL_image.o *.o && \
	printf "[COMP] done!\n"
build-sdl-net:
	printf "[COMP] compiling SDL_net...\n" && \
	cd deps/SDL2_net-2.0.0/ && \
	sh configure && \
	make && \
	ar csr ../../build/deps/lib_SDL_net.o .libs/*.o && \
	cp SDL_net.h ../../src/lib/lib_SDL_net.h && \
	sed -i "s|\"SDL.h\"|<SDL2/SDL.h>|g" ../../src/lib/lib_SDL_net.h && \
	sed -i "s|\"SDL_endian.h\"|<SDL2/SDL_endian.h>|g"\
	 ../../src/lib/lib_SDL_net.h && \
	sed -i "s|\"SDL_version.h\"|<SDL2/SDL_version.h>|g"\
	 ../../src/lib/lib_SDL_net.h && \
	sed -i "s|\"begin_code.h\"|<SDL2/begin_code.h>|g"\
	 ../../src/lib/lib_SDL_net.h && \
	sed -i "s|\"close_code.h\"|<SDL2/close_code.h>|g"\
	 ../../src/lib/lib_SDL_net.h && \
	printf "[COMP] done!\n"

################################################################################
