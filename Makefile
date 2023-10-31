CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -g3
INC=-Iinclude
LIB=$(shell pkg-config --libs glfw3) $(shell pkg-config --libs glew) -lm \
    $(shell pkg-config --libs assimp)

SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, %.o, $(SRC))
BIN=dragondrop

default: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)
	rm -f *.o

%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ $(INC) $(LIB)

clean:
	rm -f $(BIN) *.o

betty:
	clear; betty include/*.h src/*.c
