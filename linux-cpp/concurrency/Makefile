all:thread

CC=g++
CPPFLAGS=-Wall -std=c++11 -ggdb
LDFLAGS=-pthread

thread:thread_create.o
	$(CC) $(LDFLAGS) -o $@ $^

thread_create.o:thread_create.cpp
	$(CC) $(CPPFLAGS) -o $@ -c $^


.PHONY:
	clean

clean:
	rm thread_create.o thread 
