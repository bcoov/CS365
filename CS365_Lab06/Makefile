CC = gcc
CFLAGS = -O -Wall -std=gnu99 -D_REENTRANT

PROGS = sumu16 sumu16_par

all : sumu16 sumu16_par

sumu16 : sumu16.o common.o
	$(CC) -o $@ sumu16.o common.o

sumu16_par : sumu16_par.o common.o
	$(CC) -o $@ sumu16_par.o common.o -lpthread

clean :
	rm -f *.o $(PROGS) *~ solution.zip submit.properties

depend :
	$(CC) $(CFLAGS) -M sumu16.c sumu16_par.c common.c > depend.mak

depend.mak :
	touch $@

include depend.mak
