PLATFORM = Linux

build-sdl:
	printf "[COMP] compiling SDL...\n" && \
	cd deps/SDL2-2.0.3/ && \
	sh configure --prefix=`pwd`/usr_local/ && \
	make -j 4 && make install && \
	ld -r build/.libs/*.o -o ../../build/deps/lib_SDL.o && \
	cp include/*.h ../../src/lib/include/ && \
	printf "[COMP] done!\n"
