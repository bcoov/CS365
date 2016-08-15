// Produce a file containing unsigned 16 bit values in a normal distribution.
// E.g.:
//
//    ./gendata 20 2000 16777216 normal.dat
//
// to generate 16M values, where each value is the sum of rolling
// 20 2000-sided dice.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

uint16_t roll(int ndice, int sides);

int main(int argc, char **argv) {
	if (argc != 5) {
		fprintf(stderr, "Usage: gendata <ndice> <sides> <nvals> <outfile>\n");
		exit(1);
	}

	int ndice = atoi(argv[1]);
	int sides = atoi(argv[2]);
	int nvals = atoi(argv[3]);
	const char *outfile = argv[4];

	srand(time(NULL));

	FILE *fh = fopen(outfile, "w");
	if (!fh) {
		perror("Could not open output file");
		exit(1);
	}

	for (int i = 0; i < nvals; i++) {
		uint16_t val = roll(ndice, sides);
		fwrite(&val, sizeof(val), 1, fh);
	}

	fclose(fh);
}

uint16_t roll(int ndice, int sides) {
	uint16_t sum = 0;
	for (int i = 0; i < ndice; i++) {
		sum += (uint16_t) ((rand() % sides) + 1);
	}
	return sum;
}
