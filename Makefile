include Makefile.properties

all: build run

clean:
	@rm -f $(EXECUTABLE_NAME)
	@rm -f $(EXECUTABLE_NAME)-debug
	@rm -f $(EXECUTABLE_NAME)-optimize
	@rm -f a.out
	@rm -f $(DYLIB_NAME)

build:
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-o $(EXECUTABLE_NAME)

run:
	@DYLD_LIBRARY_PATH=$(DYLIB_PATH) ./$(EXECUTABLE_NAME)

build_debug:
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O0 \
	-g \
	-o $(EXECUTABLE_NAME)-debug

check_leaks: build run_leaks

run_leaks:
	@leaks --atExit -- ./$(EXECUTABLE_NAME)

optimize: build_optimize

build_optimize:
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O3 \
	-o $(EXECUTABLE_NAME)-optimize

release: build_release move_files

build_release:
	@$(CC) \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O3 \
	-dynamiclib \
	-current_version 1.0.0 \
	-o $(DYLIB_NAME)

move_files:
	@sudo mv $(DYLIB_NAME) $(DYLIB_PATH)
	@sudo cp $(EXECUTABLE_NAME).h $(DYLIB_INCLUDE_PATH)
