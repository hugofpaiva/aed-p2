#
# makefile to compile AED Assignment 2
#

clean:
	rm -f main

all:		main main_lucas main_duarte

run_all:	all
	./main

main:	main.c
	cc -g -Wall -O2 main.c -o main -lm 

main_lucas:	main_lucas.c
	cc -g -Wall -O2 main_lucas.c -o main_lucas -lm 

main_duarte:	main_duarte.c
	cc -g -Wall -O2 main_duarte.c -o main_duarte -lm 