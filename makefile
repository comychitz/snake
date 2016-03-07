GCC=gcc
CFLAGS=-Wall
LFLAGS=-lncurses

all: clean snake

run: clean snake
	./snake

snake:
	$(GCC) $(CFLAGS) $(LFLAGS) snake.c -o snake -ggdb

clean:
	rm -f snake
