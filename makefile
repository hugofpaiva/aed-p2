#
# makefile to compile AED Assignment 2
#

clean:
	rm -f main

all:		main

run_all:	all
	./main

main:	main.cpp
	c++ -Wall -O2 main.cpp -o $@
