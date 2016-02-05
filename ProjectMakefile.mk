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
CFLAGS = $(CFLAGS_INCLUDES) $(GL_VERSION) -Wall
COMPILE = printf "[COMP] Compiling $< -to- $@....\n";$(CC) $(CFLAGS) -o $@ -c $<
# target: init
FOLDERS = build/ libs/ media/ src/

################################################################################

test: -debug $(TEST) $(OBJS_DEPS) -build
	./$(JL_OUT)

android:
	sh $(shell echo $(JLL_HOME))/compile-scripts/jl_android\
	 $(shell echo $(JLL_HOME))

install: -publish $(OBJS) -build
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
	$(COMPILE) $(JL_DEBUG)
$(BUILD)/%.o: $(SRC)/*/%.c
	$(COMPILE) $(JL_DEBUG)
$(BUILD)/%.o: $(SRC)/*/*/%.c
	$(COMPILE) $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/%.c
	$(COMPILE) $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/*/%.c
	$(COMPILE) $(JL_DEBUG)
$(BUILD_TEST)/%.o: $(SRC)/*/*/%.c
	$(COMPILE) $(JL_DEBUG)

build-deps-var/%.o:
	$(eval CFILE_DEPS = $(subst .o,, $(addprefix $(SRC_DEPS)/, \
		$(subst $(BUILD_DEPS)/,,	\
		$(subst -,/,$(subst build-deps-var/,,$@.c*))))))
#	echo CFILE_DEPS = $(CFILE_DEPS)

$(BUILD_DEPS)/%.o: build-deps-var/%.o $(CFILE_DEPS)
#	echo CFILE_DEPS = $(CFILE_DEPS)
	printf "[COMP] Compiling \"$(CFILE_DEPS)\" -to- \"$@\"....\n";
	$(CC) $(CFLAGS) -o $@ -c $(CFILE_DEPS) -O3 -iquote $(INCLUDES_DEPS)

-init-vars:
	# Build Project
	$(eval INCLUDES_DEPS=\
		$(addprefix -I, $(shell find $(SRC_DEPS)/ -type d)))
	$(eval CFLAGS_INCLUDES=\
		-I$(shell echo $(JLL_HOME))/src/include/\
		-I$(shell echo $(JLL_HOME))/src/lib/include/\
		-Isrc/ $(addprefix -I, $(shell find src/ -type d ))\
		-iquote $(INCLUDES_DEPS))

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
	gcc $(OBJS) $(LIB) -o $(JL_OUT) $(CFLAGS) $(JL_DEBUG)\
		 -lm -lz -ldl -lpthread -L/usr/local/lib/  \
		-lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_videoio
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
