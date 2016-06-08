# CS 365 - Lab 1 Makefile

PROGS = hello_mpi

CFLAGS = -Wall -std=gnu99 -D_REENTRANT -D_GNU_SOURCE -I/usr/include/openmpi
CC = gcc

all : $(PROGS)

hello_mpi : hello_mpi.o
	gcc -o $@ hello_mpi.o -lmpi

clean :
	rm -f *.o $(PROGS) *~

