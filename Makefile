include Makefile.properties

all: build run

clean:
	@rm -f $(EXECUTABLE_NAME)
	@rm -f $(EXECUTABLE_NAME)-debug
	@rm -f $(EXECUTABLE_NAME)-test
	@rm -f $(EXECUTABLE_NAME)-optimize
	@rm -f a.out

build:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
	-o $(EXECUTABLE_NAME)

build_debug:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
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
	-O3 \
	-o $(EXECUTABLE_NAME)-optimize

run_optimize:
	@./$(EXECUTABLE_NAME)-optimize


build_test:
	@$(CC) $(CC_FLAGS) \
	test.c \
	json.c \
	lexer.c \
	parser.c \
	hashmap.c \
	dynamicarray.c \
	-o $(TEST_EXECUTABLE_NAME)
