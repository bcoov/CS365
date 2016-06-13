// Conway's Game of Life - parallel version

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "grid.h"
#include "life.h"

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc != 5) {
		fprintf(stderr, "Usage: ./runpar <filename> <numgens> <N> <M>\n");
		exit(1);
	}

	const char *filename = argv[1];
	int numgens = atoi(argv[2]);
	int N = atoi(argv[3]);  // number of rows of processes
	int M = atoi(argv[4]);  // number of columns of processes

	printf("filename=%s, numgens=%i, N=%i, M=%i\n", filename, numgens, N, M);

	// read file
	FILE * fp = fopen(filename, "r");
	Grid * grid = life_load_board(fp);
	fclose(fp);

	// Split per process
	int rank_col = rank % M;
	int rank_row = rank / M;

	// Create local copy
	Grid * local = grid_alloc((N / grid->rows) + 2, (M / grid->cols) + 2);
	for (int i = 0; i < local->rows; ++i) {
		for (int j = 0; j < local->cols; ++j) {
			int pos_i = (i - 1) + (local->rows * rank);
			int pos_j = (j - 1) + (local->cols * rank);
			uint8_t val = grid_get_current(grid, pos_i, pos_j);
			grid_set_current(local, i, j, val);
		}
	}

	// Local computation
	life_compute_next_gen(local);

	MPI_Finalize();

	return 0;
}
