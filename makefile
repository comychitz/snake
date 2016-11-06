GCC=gcc
CFLAGS=-Wall

all: clean snake

run: clean snake
	./snake

snake:
	$(GCC) $(CFLAGS) snake.c -o snake -ggdb -lncurses

clean:
	rm -f snake
