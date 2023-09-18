all: Counter Process
Counter : Counter.c
	gcc Counter.c -o Counter -lm
Process : main.c
	gcc main.c -o Process -lm

.PHONY: clean
clean:
	rm -f Counter Process