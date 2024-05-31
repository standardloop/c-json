include Makefile.properties

all: build run

clean:
	rm main

build:
	@$(CC) $(CC_FLAGS) \
	main.c \
	json.c \
	-o main

run:
	./main
