#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "mtqueue.h"

#define NUM_ITEMS 26

void *producer_run(void *arg) {
	MTQueue *q = arg;

	for (int i = 0; i < NUM_ITEMS; i++) {
		char *item = malloc(2);
		item[0] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i];
		item[1] = '\0';
		printf("Produce %s\n", item);
		mtqueue_enqueue(q, item);
		usleep(100000); // sleep for .1s
	}

	// Wait for queue to become empty
	printf("Producer waiting for queue to become empty\n");

	mtqueue_wait_until_empty(q);

	printf("Producer finished\n");

	return NULL;
}

void *consumer_run(void *arg) {
	MTQueue *q = arg;

	while (1) {
		void *item = mtqueue_dequeue(q);
		printf("Consumed: %s\n", (char *)item);
		free(item);
		usleep(500000);
	}
}

int main(void) {
	MTQueue *q = mtqueue_create();

	pthread_t producer;
	pthread_t consumer[2];

	pthread_create(&producer, NULL, producer_run, q);
	pthread_create(&consumer[0], NULL, consumer_run, q);
	pthread_create(&consumer[1], NULL, consumer_run, q);

	pthread_join(producer, NULL);
	printf("Producer exited\n");

	// Don't wait for the consumers to finish
	return 0;
}
