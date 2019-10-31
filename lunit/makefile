include ../makefile_base

%_test: %_test.c lunit
	$(CC) $(CFLAGS) -c -o $@.o $<
	$(CC) -o $@ $@.o lunit.o
	./$@

cmp: cmp.c
	$(CC) $(CFLAGS) -c -o $@.o $<

lunit: lunit.c cmp
	$(CC) $(CFLAGS) -c -o tmp.o $<
	make -C ../containers list
	$(LD) -r tmp.o cmp.o ../containers/list.o -o $@.o

clean:
	rm -rvf *.o *_test
