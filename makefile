#
# makefile to compile AED Assignment 2
#

clean:
	rm -f main

all:		main

run_all:	all
	./main

main:	main.c
	cc -g -Wall -O2 main.c -o main -lm 
