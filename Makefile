CC = gcc
FLAGS = -Wall -Werror -Wextra -Wpedantic

all: build

build:
	$(CC) $(FLAGS) src/main.c libs/cjson/cJSON.c -Ilibs/cjson -o bin/skareader

clean: 
	rm bin/skareader *.txt