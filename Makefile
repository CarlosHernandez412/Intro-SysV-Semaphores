# Makefile for week05

all:	create_thread  threads1 sample1 sample2 file_io

sample1: sample1.c
	gcc -o sample1 sample1.c -lpthread

sample2: sample2.c
	gcc -o sample2 sample2.c -lpthread

create_thread:	create_thread.c
	gcc -o create_thread create_thread.c -lpthread

threads1:	threads1.c
	gcc -o threads1 threads1.c -lpthread

file_io:	file_io.c
	gcc -o file_io file_io.c

clean:
	rm -f create_thread threads1 sample1 sample2 file_io
