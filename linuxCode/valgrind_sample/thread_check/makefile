#!/bin/make

all:
	gcc -g dead_lock.c -o dead_lock -lpthread
	valgrind --tool=helgrind  --log-file=check.log ./dead_lock

clean:
	rm -fr dead_lock check.log
