# CS 365 - Lab 3 Makefile

PROGS = integrate

CFLAGS = -Wall -std=gnu99 -D_REENTRANT -D_GNU_SOURCE -I/usr/include/openmpi
CC = gcc

all : $(PROGS)

integrate : integrate.o
	gcc -o $@ integrate.o -lmpi

clean :
	rm -f *.o $(PROGS) *~
