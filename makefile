GCC=gcc
CFLAGS=-Wall

all: snake

run: snake
	./snake

snake:
	$(GCC) $(CFLAGS) snake.c -o snake

clean:
	rm -f snake
