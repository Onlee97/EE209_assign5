# Dependency rules for non-file targets
all: ish
clobber: clean
	rm –f *~ \#*\# core
clean:
	rm –f testintmath *.o

# Dependency rules for file targets
ish: dynarray.o ish.o
	gcc dynarray.o ish.o -o ish
dynarray.o: dynarray.c dynarray.h
	gcc -c dynarray.c
ish.o: ish.c dynarray.h
	gcc -c ish.c
hello: hello.c
	gcc -o hello hello.c
testexec: testexec.c
	gcc -o testexec testexec.c
