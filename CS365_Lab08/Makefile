CFLAGS = -g -Wall -std=gnu99 -D_REENTRANT

all : mtqueue_test

mtqueue_test : mtqueue_test.o mtqueue.o
	$(CC) -o $@ mtqueue_test.o mtqueue.o -lpthread

clean :
	rm -f  mtqueue_test *.o depend.mak

depend :
	$(CC) -M $(CFLAGS) *.c > depend.mak

depend.mak :
	touch $@

include depend.mak
