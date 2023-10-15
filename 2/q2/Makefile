all: Counter Process Prac
Counter : Counter.c
	gcc Counter.c -o Counter -lm
Process : main.c
	gcc main.c -o Process -lm
	sudo taskset -c 0 ./Process
	
.PHONY: clean
clean:
	rm -f Counter Process
