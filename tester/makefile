include ../makefile_base

# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

run_tests: run_tests.c student problem
	make -C ../containers list
	${CC} ${CFLAGS} -o run_tests run_tests.c student.o problem.o \
		../containers/list.o

student: student.c student.h
	${CC} ${CFLAGS} -c student.c

problem: problem.c problem.h
	${CC} ${CFLAGS} -c problem.c

clean:
	rm -rvf main run_tests *.o
