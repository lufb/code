#!/bin/make

all:
	g++ -g valgrind_memcheck.cpp -o valgrind_memcheck
	valgrind --leak-check=full --show-reachable=yes ./valgrind_memcheck
clean:
	rm -f valgrind_memcheck
	
