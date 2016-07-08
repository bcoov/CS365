// Compute the sum of a file full of binary 16-bit integer data,
// parallel version.

// TODO: parallelize this program!

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include "common.h"

struct t_parts {
	uint16_t * t_data;
	size_t start;
	size_t end;
	uint64_t t_sum;
};

// struct t_parts some_threads[1];

void * thread_sum(void * t_arg)
{
	printf("Starting thread_sum\n");

	struct t_parts * thread_data = (struct t_parts *) t_arg;
	for (size_t i = thread_data->start; i < thread_data->end; ++i) {
		thread_data->t_sum += thread_data->t_data[i];
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	uint16_t *data;
	size_t i, size, num_elements;
	uint64_t sum;
	struct mmap_region region;

	struct t_parts t0_data;
	pthread_t thread0;
	size_t chunk;

	if (argc != 2) {
		fprintf(stderr, "Usage: sumu16 <input file>\n");
		exit(1);
	}

	// Find out how large the file is.
	size = get_file_size(argv[1]);
	printf("File is %lu bytes in size\n", size);

	// Each element (datum) is 16 bits.
	// Find out how many elements there are overall.
	num_elements = size / sizeof(uint16_t);
	chunk = num_elements / 2;

	// Map the file data into memory
	map_file_region(argv[1], size, &region);
	data = region.addr;

	// Compute the sum
	sum = 0ULL;

	// some_threads[0].t_data = data;
	// some_threads[0].start = chunk + (size_t) 1;
	// some_threads[0].end = some_threads[0].start + chunk;
	// some_threads[0].t_sum = sum;

	t0_data.t_data = data;
	t0_data.start = chunk + (size_t) 1;
	t0_data.end = t0_data.start + chunk;
	t0_data.t_sum = sum;

	// pthread_create(&thread0, NULL, thread_sum, (void *) some_threads[0]);
	pthread_create(&thread0, NULL, thread_sum, &t0_data);

	// for (i = 0; i < num_elements; i++) {
	for (i = 0; i < chunk; i++) {
		sum += data[i];
	}

	pthread_join(&thread0, NULL);

	// sum += some_threads[0].t_sum;
	sum += t0_data.t_sum;

	printf("Sum is %lu\n", sum);

	unmap_file_region(&region);

	return 0;
}

