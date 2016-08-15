#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "mapfile.h"

// No data value will be larger than this
#define MAX_ELEMENT_VALUE 40000

// Each bucket counts occurrences of this many values
// (in a contiguous range)
#define BUCKET_SIZE       4000

// Number of buckets in the histogram
#define NUM_BUCKETS       (MAX_ELEMENT_VALUE/BUCKET_SIZE)

// The number of threads to use (note that your program should work
// with this set to 2, *or other reasonable values*)
#define NUM_THREADS       2

// TODO: define a worker data structure type
typedef struct {
	size_t start;
	size_t end;
	uint16_t * data;
	int * hist;
	pthread_mutex_t lock;
} Worker;

// TODO: define a worker function
void increment_bucket(Worker * t_work, int bucket) {
	pthread_mutex_lock(&t_work->lock);

	t_work->hist[bucket]++;

	pthread_mutex_unlock(&t_work->lock);
}

void * fill_buckets(void * t_arg) {
	Worker * t_work = t_arg;

	for (size_t t = t_work->start; t < t_work->end; t++) {
		uint16_t val = t_work->data[t];
		int bucket = (val - 1) / BUCKET_SIZE;
		if (bucket < 0) {
			bucket = 0;
		} else if (bucket >= NUM_BUCKETS) {
			bucket = NUM_BUCKETS - 1;
		}
		increment_bucket(t_work, bucket);
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: ./parhist <filename>\n");
		exit(1);
	}

	const char *filename = argv[1];

	// create a memory-mapped view of the file
	size_t file_size = get_file_size(filename);
	struct mmap_region region;
	map_file_region(filename, file_size, &region);

	// This is the array of values and the number of elements
	uint16_t *arr = region.addr;
	size_t num_elements = file_size/sizeof(uint16_t);

	// This is the histogram: each element counts the number
	// of data values that fell into a particular 4000-wide range:
	// Histogram element 0 counts number of values from 1..4000
	// Histogram element 1 counts number of values from 4001..8000
	// etc.  The elements are initialized to 0.
	int histogram[NUM_BUCKETS] = { 0 };

	size_t chunk_size = num_elements / NUM_THREADS;

	// Computation

	// TODO: divide up the work
	size_t leftover = num_elements % NUM_THREADS;

	Worker * thread_work = malloc(sizeof(Worker));
	pthread_mutex_init(&thread_work->lock, NULL);

	pthread_t threads[NUM_THREADS];
	Worker workers[NUM_THREADS];

	// TODO: create threads
	for (size_t i = 0; i < NUM_THREADS; i++) {
		workers[i] = * thread_work;
		workers[i].start = i * chunk_size;
		workers[i].end = workers[i].start + chunk_size;
		workers[i].data = arr;
		workers[i].hist = histogram;

		if (i == NUM_THREADS - 1) {
			workers[i].end += leftover;
		}
		pthread_create(&threads[i], NULL, fill_buckets, &workers[i]);
	}

	// TODO: wait for threads to finish
	for (int j = 0; j < NUM_THREADS; j++) {
		pthread_join(threads[j], NULL);
	}

	// TODO: combine results (if necessary)

	// Print histogram
	for (int i = 0; i < NUM_BUCKETS; i++) {
		printf("%5i..%5i: %i\n", i*BUCKET_SIZE+1, (i+1)*BUCKET_SIZE, histogram[i]);
	}

	// Unmap the file
	unmap_file_region(&region);

	return 0;
}
