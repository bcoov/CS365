#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mapfile.h"

// No data value will be larger than this
#define MAX_ELEMENT_VALUE 40000

// Each bucket counts occurrences of this many values
// (in a contiguous range)
#define BUCKET_SIZE       4000

// Number of buckets in the histogram
#define NUM_BUCKETS       (MAX_ELEMENT_VALUE/BUCKET_SIZE)

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
	// etc.
	int histogram[NUM_BUCKETS] = { 0 };

	// Sequential computation
	for (size_t i = 0; i < num_elements; i++) {
		uint16_t val = arr[i];
		int bucket = (val-1)/BUCKET_SIZE;
		if (bucket < 0) {
			bucket = 0;
		} else if (bucket >= NUM_BUCKETS) {
			bucket = NUM_BUCKETS-1;
		}
		histogram[bucket]++;
	}

	// Print histogram
	for (int i = 0; i < NUM_BUCKETS; i++) {
		printf("%5i..%5i: %i\n", i*BUCKET_SIZE+1, (i+1)*BUCKET_SIZE, histogram[i]);
	}

	return 0;
}
