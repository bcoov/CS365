#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "is_prime.h"

typedef struct {
	int capacity, num_found;
	int *data;
	pthread_mutex_t lock;
} PrimeList ;

typedef struct {
	PrimeList * work_list;
	int start;
	int end;
} ThreadWork;

// Create a PrimeList with enough capacity to store given
// number of primes.
PrimeList *primelist_alloc(int capacity)
{
	PrimeList *list = malloc(sizeof(PrimeList));
	list->capacity = capacity;
	list->num_found = 0;
	list->data = malloc(capacity * sizeof(int));

	pthread_mutex_init(&list->lock, NULL);

	return list;
}

// Use this function to add a prime to the PrimeList.
// This function must be thread-safe!
// Use the PrimeList's mutex to guard the critical section
// (the code that will access the PrimeList's fields).
void primelist_append(PrimeList *list, int prime)
{
	pthread_mutex_lock(&list->lock);

	list->data[list->num_found] = prime;
	++list->num_found;

	pthread_mutex_unlock(&list->lock);

	return;
}

void * find_primes(void * t_arg)
{
	ThreadWork * t_work = (ThreadWork) t_arg;

	for (int i = t_work->start; i < t_work->end; i++) {
		if (is_prime(i)) {
			primelist_append(t_work->work_list, i);
		}
	}

	pthread_exit(NULL);
}

int main(void)
{
	int min, max;
	int num_threads;

	printf("Min: ");
	scanf("%i", &min);
	printf("Max: ");
	scanf("%i", &max);
	printf("Number of threads: ");
	scanf("%i", &num_threads);

	int est_num_primes = (int) ((max / log(max)) * 1.4);
	printf("Estimated number of primes is %i\n", est_num_primes);

	// Use this data structure to store all of the primes discovered
	PrimeList *list = primelist_alloc(est_num_primes);

	// Use threads to find all primes in range, add them to the PrimeList
	pthread_t threads[num_threads];
	ThreadWork workers[num_threads];

	int range = (max - min) + 1;
	int chunk_size = range / num_threads;
	int leftover = range % num_threads;

	for (int t = 0; t < num_threads; t++) {
		workers[t].work_list = list;
		workers[t].start = min + (i * chunk_size);
		workers[t].end = workers[t].start + chunk_size;
		if (t == num_threads - 1) {
			workers[t].end += leftover;
		}
		pthread_create(&threads[t], NULL, find_primes, &workers[t]);
	}

	for (int t = 0; t < num_threads; t++) {
		pthread_join(threads[t], NULL);
	}

	// Print out all primes found in range (once all threads have completed)
	for (int i = 0; i < list->num_found; i++) {
		printf("Prime %d: %d\n", i, list->data[i]);
	}

	return 0;
}
