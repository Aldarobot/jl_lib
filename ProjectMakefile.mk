# Build Project
CFLAGS_INCLUDES = -I$(shell echo $(JLL_HOME))/src/include/\
	-I$(shell echo $(JLL_HOME))/src/lib/include/\
	-Isrc/ -Isrc/include/
SRC = src
BUILD = build/objs
BUILD_TEST = build/test
MODULES = $(shell basename -a $(subst .c,, $(shell find src/ -type f -name '*.c')))
OBJS = $(addprefix $(BUILD)/, $(addsuffix .o, $(MODULES)))
TEST = $(addprefix $(BUILD_TEST)/, $(addsuffix .o, $(MODULES)))
OUT = $(BUILD)/*.o
LIB = $(shell echo $(JLL_HOME))/build/jl.o
CFLAGS = $(CFLAGS_INCLUDES) $(GL_VERSION) -Wall
COMPILE = printf "[COMP] Compiling $< -to- $@....\n";$(CC) $(CFLAGS) -o $@ -c $<

test: -debug $(TEST) -build
	./$(JL_OUT)
android:
	sh $(shell echo $(JLL_HOME))/compile-scripts/jl_android\
	 $(shell echo $(JLL_HOME))
install: -publish $(OBJS) -build
	printf "Installing....\n"
	if [ -z "$(JLL_PATH)" ]; then \
		printf "Where to install? ( hint: /bin or $$HOME/bin ) [ Set JLL_PATH ]\n"; \
		read JLL_PATH; \
	fi; \
	printf "Copying files to $$JLL_PATH/....\n"; \
	cp -u --recursive -t $$JLL_PATH/ build/bin/*; \
	printf "Done!\n"

# low-level: compile project:

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

-debug:
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-g)
	$(eval JL_OUT=build/test.out)
-publish:
#	$(eval GL_VERSION=-lGL) ## OpenGL
	$(eval GL_VERSION=-lGLESv2) ## OpenGL ES
	$(eval JL_DEBUG=-O3)
	$(eval JL_OUT=build/bin/$(shell echo `sed -n '4p' data.txt`))
-build:
	printf "[COMP] Linking ....\n"
	gcc $(OBJS) $(LIB) -o $(JL_OUT) $(CFLAGS) $(JL_DEBUG)\
		 -lm -lz -ldl -lpthread
	printf "[COMP] Done [ OpenGL Version = $(GL_VERSION) ]!\n"
#end#
