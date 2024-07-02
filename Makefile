include Makefile.properties

all: build run

clean:
	rm $(EXECUTABLE_NAME)

build:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
	util.c \
	-o $(EXECUTABLE_NAME)

build_debug:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
	util.c \
	-O0 \
	-g \
	-o $(EXECUTABLE_NAME)-debug


run:
	@./$(EXECUTABLE_NAME)

check_leaks: build run_leaks

run_leaks:
	@leaks --atExit -- ./$(EXECUTABLE_NAME)

optimize: build_optimize run_optimize

build_optimize:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
	util.c \
	-O3 \
	-o $(EXECUTABLE_NAME)-optimize

run_optimize:
	@./$(EXECUTABLE_NAME)-optimize
