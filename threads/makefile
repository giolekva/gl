include ../makefile_base

# TODO(giolekva): remove hardcoded pool
%_test: %_test.c pool
	$(CC) $(CFLAGS) -c -o $@.o $<
	make -C ../lunit lunit
	$(CC) -o $@ $@.o pool.o ../lunit/lunit.o
	./$@

pool: pool.c
	$(CC) $(CFLAGS) -c -o $@.o $<

clean:
	rm -rvf *.o *_test
