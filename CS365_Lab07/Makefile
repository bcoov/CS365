CC = gcc
CFLAGS = -g -O -Wall -std=gnu99 -D_REENTRANT

all : find_primes

find_primes : find_primes.o is_prime.o
	$(CC) -o $@ find_primes.o is_prime.o -lpthread -lm

clean :
	rm -f *.o find_primes *~ depend.mak

depend :
	$(CC) $(CFLAGS) -M find_primes.c is_prime.c > depend.mak

depend.mak :
	touch $@

include depend.mak
