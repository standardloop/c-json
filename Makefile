include Makefile.properties

all: download_dependencies build run

clean:
	@rm -f $(EXECUTABLE_NAME)
	@rm -f $(EXECUTABLE_NAME)-sanitize
	@rm -f $(EXECUTABLE_NAME)-debug
	@rm -f $(EXECUTABLE_NAME)-optimize
	@rm -f a.out
	@rm -f $(DYLIB_NAME)
	@rm -f $(DYLIB_NAME).zip

download_dependencies:
	mkdir -p tmp && \
	cd tmp && \
	curl -O -J -L https://github.com/standardloop/c-util/releases/download/v0.0.1/libstandardloop-util.zip && \
	unzip libstandardloop-util.zip && \
	sudo mkdir -p /usr/local/lib/standardloop/ && \
	sudo mkdir -p /usr/local/include/standardloop/ && \
	sudo mv libstandardloop-util.dylib /usr/local/lib/standardloop/ && \
	sudo mv util.h /usr/local/include/standardloop/ && rm libstandardloop-util.zip

build: download_dependencies
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-o $(EXECUTABLE_NAME)

run:
	@DYLD_LIBRARY_PATH=$(DYLIB_PATH) ./$(EXECUTABLE_NAME)

build_debug: download_dependencies
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O0 \
	-g \
	-o $(EXECUTABLE_NAME)-debug

sanitize: build_sanitize run_sanitize

build_sanitize: download_dependencies
	@$(CC) $(CC_FLAGS) \
	main.c \
	-fsanitize=address \
	-fno-omit-frame-pointer \
	-g \
	-O0 \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-o $(EXECUTABLE_NAME)-sanitize

run_sanitize:
	@DYLD_LIBRARY_PATH=$(DYLIB_PATH) ./$(EXECUTABLE_NAME)-sanitize

check_leaks: build run_leaks

# FIXME DYLIB issues
run_leaks:
	@export DYLD_LIBRARY_PATH=$(DYLIB_PATH) && leaks --atExit -- ./$(EXECUTABLE_NAME)

optimize: build_optimize

build_optimize: download_dependencies
	@$(CC) $(CC_FLAGS) \
	main.c \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O3 \
	-o $(EXECUTABLE_NAME)-optimize

release: build_release move_files

build_release: download_dependencies
	@$(CC) $(CC_FLAGS) \
	$(SOURCE_FILES) \
	$(DYN_LIBS_USED_PATH) \
	$(DYN_LIBS_USED) \
	-O3 \
	-dynamiclib \
	-current_version $(RELEASE_VERSION) \
	-o $(DYLIB_NAME)

move_files:
	@sudo mv $(DYLIB_NAME) $(DYLIB_PATH)
	@sudo cp $(EXECUTABLE_NAME).h $(DYLIB_INCLUDE_PATH)

# local testing
download_release:
	mkdir -p tmp && \
	cd tmp && \
	curl -O -J -L https://github.com/standardloop/c-json/releases/download/v0.0.1/libstandardloop-json.zip && \
	unzip libstandardloop-json.zip && \
	sudo mv libstandardloop-json.dylib /usr/local/lib/standardloop/ && \
	sudo mv json.h /usr/local/include/standardloop/ && rm libstandardloop-json.zip

.PHONY: lab FORCE

lab: FORCE
	@$(CC) $(CC_FLAGS) \
	lab.c \
	-L/usr/local/lib/standardloop \
	-lstandardloop-util \
	-lstandardloop-json \
	-o lab

FORCE:

clean_release:
	sudo rm /usr/local/lib/standardloop/libstandardloop-json.dylib
	sudo rm /usr/local/include/standardloop/json.h
