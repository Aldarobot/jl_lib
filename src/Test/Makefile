compile_gl: __compile_gl __compile
compile_gles: __compile_gles __compile
action:
	echo argument is $(argument)
run:
	./bin/comp/program.elf
	printf "[DONE] ran it!\n"
run_android:
	../../bin/sdk/platform-tools/adb logcat | grep E/AndroidRuntime
mediao:
	echo [MEDIA] compiling media: $(data)
	cp ../../bin/jl_lib/jlib/share/main.c media/genr/$(data).c
	sed -i "s|JL_LIB_MEDIA|$(data)|g" media/genr/$(data).c
	printf "[MEDIA] compiled media!\n"
__compile:
	printf "[COMP]cleaning up....\n"
	rm -f bin/comp/program.elf
	printf "[COMP]compiling with GL $(GL_VERSION)....\n"
	if [ ! -e media/genr/*.o ]; then \
		ar csr media/media.o; \
	else \
		ar csr media/media.o media/genr/*.o; \
	fi
	gcc ../../../bin/jl_lib/jlib/*.o \
	src/*.c media/media.o \
	-lSDL2 -lpthread -Wl,--no-undefined -lm -ldl -lpthread -lrt \
	$(GL_VERSION) -lm -lz \
	-o bin/comp/program.elf \
	-Wall -g
	printf "[COMP]done compiling!\n"
__compile_gl:
	$(eval GL_VERSION=-lGL)
__compile_gles:
	$(eval GL_VERSION=-lGLESv2)
compile_android:
	env ANDROID_HOME=../../bin/jl_lib/android/andk/sdk NDK_HOME=../../bin/install/android-ndk-r10d/ NDK_STANDALONE=/opt/ndk_standalone PATH=$(PATH):$HOME/prg/bin/install/android-ndk-r10d/toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86_64/bin/ cargo build --target=arm-linux-androideabi
	mv target/arm-linux-androideabi/debug/jl_lib-example target/arm-linux-androideabi/debug/jl_lib-example.apk
	./../../bin/jl_lib/android/andk/sdk/platform-tools/adb install -r target/arm-linux-androideabi/debug/jl_lib-example.apk
	./../../bin/jl_lib/android/andk/sdk/platform-tools/adb logcat | grep jl

