// Compute the sum of a file full of binary 16-bit integer data,
// sequential version.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "common.h"

int main(int argc, char **argv)
{
	uint16_t *data;
	size_t i, size, num_elements;
	uint64_t sum;
	struct mmap_region region;

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

	// Map the file data into memory
	map_file_region(argv[1], size, &region);
	data = region.addr;

	// Compute the sum
	sum = 0ULL;
	for (i = 0; i < num_elements; i++) {
		sum += data[i];
	}

	printf("Sum is %lu\n", sum);

	unmap_file_region(&region);

	return 0;
}

