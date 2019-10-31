include ../makefile_base

%_test: %_test.c
	$(CC) $(CFLAGS) -c -o $@.o $<
	make -C ../lunit lunit
	$(CC) -o $@ $@.o ../lunit/lunit.o
	./$@

list: list.c
	$(CC) $(CFLAGS) -c -o $@.o $<

clean:
	rm -rvf *.o *_test
