################################################################################

# Dependencies
SRCS_DEPS = \
	$(shell find $(SRC_DEPS)/ -type f -name '*.c')\
	$(shell find $(SRC_DEPS)/ -type f -name '*.cpp')
MODULES_DEPS = \
	$(subst .c,, $(subst .cpp,, \
	$(subst /,-, $(SRCS_DEPS))))
OBJS_DEPS = \
	$(addprefix $(BUILD_DEPS)/, $(subst $(BUILD_DEPS)/libs-,,\
	$(foreach x, $(MODULES_DEPS), $(BUILD_DEPS)/$(x).o)))

# directories
SRC = src
SRC_DEPS = libs
BUILD = build/objs
BUILD_TEST = build/test
BUILD_DEPS = build/deps
#
MODULES = $(shell basename -a $(subst .c,,\
	$(shell find $(SRC)/ -type f -name '*.c')))
OBJS = $(addprefix $(BUILD)/, $(addsuffix .o, $(MODULES)))
TEST = $(addprefix $(BUILD_TEST)/, $(addsuffix .o, $(MODULES)))
LIB = $(shell echo $(JLL_HOME))/build/jl.o\
	$(shell find $(BUILD_DEPS)/ -type f -name '*.o')
COMPILE = printf "[COMP/PROJ] Compiling $< -to- $@....\n";$(CC)
# target: init
FOLDERS = build/ libs/ media/ src/

################################################################################

test: $(FOLDERS) -debug $(TEST) $(OBJS_DEPS) -build
	sudo ./$(JL_OUT)

android:
	sh $(shell echo $(JLL_HOME))/compile-scripts/jl_android\
	 $(shell echo $(JLL_HOME))

install: $(FOLDERS) -publish $(OBJS) -build
	printf "Installing....\n"
	if [ -z "$(JLL_PATH)" ]; then \
		printf "Where to install? ( hint: /bin or $$HOME/bin ) [ Set"\
		 " JLL_PATH ]\n"; \
		read JLL_PATH; \
	fi; \
	printf "Copying files to $$JLL_PATH/....\n"; \
	cp -u --recursive -t $$JLL_PATH/ build/bin/*; \
	printf "Done!\n"

init: $(FOLDERS)
	printf "[COMPILE] Done!\n"

################################################################################

$(BUILD)/%.o: $(SRC)/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD)/%.o: $(SRC)/*/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD)/%.o: $(SRC)/*/*/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/*/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/*/*/%.c
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)

build-deps-var/%.o:
	$(eval CFILE_DEPS = $(subst .o,, $(addprefix $(SRC_DEPS)/, \
		$(subst $(BUILD_DEPS)/,,	\
		$(subst -,/,$(subst build-deps-var/,,$@.c*))))))
#	echo CFILE_DEPS = $(CFILE_DEPS)

$(BUILD_DEPS)/%.o: build-deps-var/%.o $(CFILE_DEPS)
#	echo CFILE_DEPS = $(CFILE_DEPS)
	printf "[COMP/DEPS] Compiling \"$(CFILE_DEPS)\" -to- \"$@\"....\n";
	$(CC) -o $@ -c $(CFILE_DEPS) -O3 $(CFLAGS)

-init-vars:
	# Build Project
	$(eval INCLUDES_DEPS=\
#		$(addprefix -I, $(shell find deps/ -type d -name "include")) \
		$(addprefix -I, $(shell find $(SRC_DEPS)/ -type d)))
	$(eval CFLAGS_INCLUDES=\
		-I$(shell echo $(JLL_HOME))/src/include/\
		-I$(shell echo $(JLL_HOME))/src/lib/include/\
		-iquote $(addprefix -I, $(shell find src/ -type d ))\
		$(addprefix -I, $(shell find $(SRC_DEPS)/ -type d)))
	$(eval CFLAGS=\
		$(CFLAGS_INCLUDES) -Wall)
	echo the in $(CFLAGS)

-debug: -init-vars
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-g)
	$(eval JL_OUT=build/test.out)
	$(eval OBJS=$(TEST))
-publish: -init-vars
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-O3)
	$(eval JL_OUT=build/bin/$(shell echo `sed -n '4p' data.txt`))
-build:
	printf "[COMP] Linking ....\n"
	gcc $(OBJS) $(LIB) -o $(JL_OUT) $(CFLAGS) \
		-L/opt/vc/lib/ -lm -lz -ldl -lpthread \
		$(GL_VERSION) $(JL_DEBUG) \
		`pkg-config --libs opencv` \
		`$(shell echo $(JLL_HOME))/deps/SDL2-2.0.3/sdl2-config --libs` -lbcm_host
#		-lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_videoio
#		-lcv -lhighgui -lcvaux -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_video -lopencv_video_proc -lopencv_calib3d -lopencv_imgcodecs
	printf "[COMP] Done [ OpenGL Version = $(GL_VERSION) ]!\n"
build/:
	# Generated Files
	mkdir -p build/bin/ # Where the output files are stored
	mkdir -p build/deps/ # Where your project's dependencies are stored (.o)
	mkdir -p build/objs/ # Where your program's object files are stored (.o)
	mkdir -p build/test/ # Unoptimized version of build/objs/
libs/:
	mkdir -p libs/ # Where the dependencies for your project are stored (.c*)
media/:
	mkdir -p media/aud/
	mkdir -p media/gen/
	mkdir -p media/img/
src/:
	# Where your program's code files are stored (.c*)
	mkdir -p src/include/ # Where your program's header files are stored.
	mkdir -p src/media/ # Where your media files are stored.
#end#
