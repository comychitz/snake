GCC=gcc
CFLAGS=-Wall
LFLAGS=-lncurses

all: snake

run: snake
	./snake

snake:
	$(GCC) $(CFLAGS) $(LFLAGS) snake.c -o snake -ggdb

clean:
	rm -f snake
