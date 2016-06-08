# CS 365 - Lab 2 Makefile

PROGS = rule30_seq rule30_par
SRCS = $(PROGS:%=%.c) rule30.c

CFLAGS = -Wall -std=gnu99 -D_REENTRANT -D_GNU_SOURCE -I/usr/include/openmpi
CC = gcc

all : $(PROGS)

rule30_seq : rule30_seq.o rule30.o
	gcc -o $@ rule30_seq.o rule30.o

rule30_par : rule30_par.o rule30.o
	gcc -o $@ rule30_par.o rule30.o -lmpi

clean :
	rm -f *.o $(PROGS) *~ depend.mak

depend.mak :
	touch $@

depend :
	$(CC) -M $(CFLAGS) $(SRCS) > depend.mak

include depend.mak
