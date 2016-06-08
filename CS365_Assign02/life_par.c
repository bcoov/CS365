// Conway's Game of Life - parallel version

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "grid.h"
#include "life.h"

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	if (argc != 5) {
		fprintf(stderr, "Usage: ./runpar <filename> <numgens> <N> <M>\n");
		exit(1);
	}

	const char *filename = argv[1];
	int numgens = atoi(argv[2]);
	int N = atoi(argv[3]);  // number of rows of processes
	int M = atoi(argv[4]);  // number of columns of processes

	printf("filename=%s, numgens=%i, N=%i, M=%i\n", filename, numgens, N, M);

	// TODO: computation

	MPI_Finalize();

	return 0;
}
