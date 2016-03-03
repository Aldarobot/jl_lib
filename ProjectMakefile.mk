################################################################################

# Figure out which platform.
include $(shell echo $(JLL_HOME))/compile-scripts/platform.mk

# directories
SRC = src
SRC_DEPS = libs
BUILD_OBJS = build/objs
BUILD_TEST = build/test
BUILD_DEPS = build/deps

# Dependencies
#	$(shell find $(SRC_DEPS)/ -type f -name '*.cpp')
MODULES_DEPS = $(subst .c,, $(shell basename -a $(subst .cpp,, \
	$(shell find $(SRC_DEPS)/ -type f -name '*.c') \
)))
OBJS_DEPS = \
	$(addprefix $(BUILD_DEPS)/, $(addsuffix .o,$(MODULES_DEPS)))

# Program
#	$(shell find $(SRC)/ -type f -name '*.cpp')
MODULES_PRG = \
	$(subst .c,, $(subst .cpp,, $(shell basename -a \
	$(shell find $(SRC)/ -type f -name '*.c') \
)))

# Test & Release
OBJS_PRG = \
	$(OBJS_DEPS) \
	$(addprefix $(BUILD_TEST)/, $(addsuffix .o,$(MODULES_PRG)))
OBJS_RLS = \
	$(OBJS_DEPS) \
	$(addprefix $(BUILD_OBJS)/, $(addsuffix .o,$(MODULES_PRG)))

# Special MAKE variable - do not rename.
VPATH = \
	$(shell find $(SRC)/ -type d) \
	$(shell find $(SRC_DEPS)/ -type d)
#
HEADERS = $(shell find $(SRC)/ -type f -name '*.h')
LIB = $(shell echo $(JLL_HOME))/build/jl.o
COMPILE = printf "[COMP/PROJ] Compiling $<....\n";$(CC) # -to- $@.
# target: init
FOLDERS = build/ libs/ media/ src/

################################################################################

test: build-notify $(FOLDERS) -debug $(OBJS_PRG) -build
#	echo run
	./$(JL_OUT)

android:
	sh $(shell echo $(JLL_HOME))/compile-scripts/jl_android\
	 $(shell echo $(JLL_HOME))

install: build-notify $(FOLDERS) -publish $(OBJS_RLS) -build
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

build-notify:
	echo Modules: $(MODULES_PRG) $(MODULES_DEPS)
	echo Headers: $(HEADERS)
	echo Folders: N/A #$(VPATH)
	printf "[COMP] Building program for target=$(PLATFORM)....\n"

clean:
	rm -r build/bin/ build/deps build/objs/ build/test/
	mkdir -p build/bin/ build/deps build/objs/ build/test/

################################################################################

$(BUILD)/%.o: %.c $(HEADERS)
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)
$(BUILD_TEST)/%.o: %.c $(HEADERS)
	$(COMPILE) $(CFLAGS) -o $@ -c $< $(JL_DEBUG)

build-deps-var/%.o:
	$(eval CFILE_DEPS = $(subst .o,, $(addprefix $(SRC_DEPS)/, \
		$(subst $(BUILD_DEPS)/,,	\
		$(subst -,/,$(subst build-deps-var/,,$@.c*))))))
#	echo CFILE_DEPS = $(CFILE_DEPS)

$(BUILD_DEPS)/%.o: %.c $(HEADERS)
#	echo CFILE_DEPS = $(CFILE_DEPS)
	printf "[COMP/DEPS] Compiling \"$<\" -to- \"$@\"....\n";
	$(CC) -o $@ -c $< -O3 $(CFLAGS)

-init-vars:
	# Build Project
	$(eval INCLUDES_DEPS=\
#		$(addprefix -I, $(shell find deps/ -type d -name "include")) \
		$(addprefix -I, $(shell find $(SRC_DEPS)/ -type d)))
	$(eval CFLAGS_INCLUDES=\
		-I$(shell echo $(JLL_HOME))/src/C/include/\
		-I$(shell echo $(JLL_HOME))/src/lib/include/\
		-iquote $(addprefix -I, $(shell find src/ -type d ))\
		$(addprefix -I, $(shell find $(SRC_DEPS)/ -type d)))
	$(eval CFLAGS=\
		$(CFLAGS_INCLUDES) -Wall -Werror)
#	echo the in $(CFLAGS)

-debug: -init-vars
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-g)
	$(eval JL_OUT=build/test.out)
	$(eval OBJS=$(OBJS_PRG))
-publish: -init-vars
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-O3)
	$(eval JL_OUT=build/bin/$(shell echo `sed -n '4p' data.txt`))
	$(eval OBJS=$(OBJS_RLS))
-build:
	printf "[COMP] Linking ....\n"
	$(CC) $(OBJS) $(LIB) -o $(JL_OUT) $(CFLAGS) \
		-lm -lz -ldl -lpthread -lstdc++ -ljpeg \
		`$(shell echo $(JLL_HOME))/deps/SDL2-2.0.3/sdl2-config --libs` \
		$(LINKER_LIBS) $(PLATFORM_CFLAGS) \
		$(GL_VERSION) $(JL_DEBUG)
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
