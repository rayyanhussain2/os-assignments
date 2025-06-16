# this is makefile for MeMS
.PHONY: shared example test
all: shared 

shared:
	gcc -shared -o mems.so -fPIC src/mems.c
	mv mems.so build/

example:
	gcc -o example examples/main.c src/mems.c

test: 
	gcc -o testExec test/mallocbomb.c src/mems.c
	
clean:
	rm -f example
	rm -f testExec
	rm -rf build/*