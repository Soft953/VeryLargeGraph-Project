CC = gcc
CFLAGS = -Werror -Wall -Wextra -std=c99 -pedantic -g
SRC = main.c
BIN = reordering
.PHONY : all clean

all:
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)
clean:
	$(RM) $(BIN)
