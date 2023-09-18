all: Counter Process
Counter : Counter.c
	gcc -o Counter Counter.c
Process : main.c
	gcc -o main.c Process

.PHONY: clean
clean:
	rm -f Counter Process