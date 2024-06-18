include Makefile.properties

all: build run

clean:
	rm $(EXECUTABLE_NAME)

build:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	hashmap.c \
	dynamic-array.c \
	util.c \
	-o $(EXECUTABLE_NAME)

run:
	@./main

check_leaks:
	@leaks --atExit -- ./$(EXECUTABLE_NAME)
