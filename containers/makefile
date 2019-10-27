CC=gcc
CFLAGS=-Wall -Werror -std=c99

list: list.c
	$(CC) $(CFLAGS) -c -o $@.o $<

clean:
	rm -rvf *.o *_test
