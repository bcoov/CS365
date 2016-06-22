// Conway's Game of Life - parallel version

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>
#include "grid.h"
#include "life.h"

static void send_col(Grid * grid, int col, int dest);
static void recv_col(Grid * grid, int col, int src);
static void send_row(Grid * grid, int row, int dest);
static void recv_row(Grid * grid, int row, int src);

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

	int h_chunk_size = grid->cols / M;
	int v_chunk_size = grid->rows / N;

	int width = h_chunk_size + 2;
	int height = v_chunk_size + 2;

	if (rank_col == M - 1) {
		// In right column of processes, allocate excess grid columns
		width += grid->cols % M;
	}
	if (rank_row == N - 1) {
		// In bottom row of processes, allocate excess grid rows
		height += grid->rows % N;
	}

	// Create local copy
	Grid * local = grid_alloc(height, width);
	// for (int i = 0; i < local->rows; ++i) {
	// 	for (int j = 0; j < local->cols; ++j) {
	for (int i = 1, pos_i = v_chunk_size*rank_row;
		 i < local->rows-1; ++i, ++pos_i) {
		for (int j = 1, pos_j = h_chunk_size*rank_col;
			 j < local->cols-1; ++j, ++pos_j) {
			// int pos_i = (i - 1) + (local->rows * rank);
			// int pos_j = (j - 1) + (local->cols * rank);
			uint8_t val = grid_get_current(grid, pos_i, pos_j);
			grid_set_current(local, i, j, val);
		}
	}

	sleep(rank);
	printf("Rank: %d\n", rank);
	life_save_board(stdout, local);
	fflush(stdout);
	for (int i = 0; i < numgens; ++i) {
		// Communications

		// Column sending appears to be working properly

		// Left Neighbour
		if (rank_col != 0) {
			// send column left
			int left_n = rank - 1;
			send_col(local, 1, left_n);

			// receive from left
			recv_col(local, 0, left_n);
		}
		// Right Neighbour
		if (rank_col != M - 1) {
			// send column right
			int right_n = rank + 1;
			send_col(local, local->rows - 2, right_n);

			// receive from right
			recv_col(local, local->rows - 1, right_n);
		}

		// Row sending is acting strangely. A row appears to be "stolen"
		// from the previous process and given to the next
		// ex:
		/*
		Rank: 0
		4 12
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 1 0 0 0 0 0 0 0 0
		0 0 0 0 1 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		Rank: 0 (After)
		4 12
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		Rank: 1
		5 12
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 1 1 1 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		Rank: 1 (After)
		5 12
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		0 0 0 0 0 0 0 0 0 0 0 0
		*/

		// Top Neighbour
		if (rank_row != 0) {
			// send row up
			int top_n = rank - M;
			send_row(local, 1, top_n);

			// receive from top
			recv_col(local, 0, top_n);
		}
		// Bottom Neighbour
		if (rank_row != N - 1) {
			// send row down
			int bot_n = rank + M;
			send_row(local, local->cols - 2, bot_n);

			// receive from bottom
			recv_row(local, local->cols - 1, bot_n);
		}

		// Local computation
		life_compute_next_gen(local, 1, 1);

		grid_flip(local);
	}
	sleep(rank);
	printf("Rank: %d (After)\n", rank);
	life_save_board(stdout, local);
	fflush(stdout);

	MPI_Finalize();

	return 0;
}

static void send_col(Grid * grid, int col, int dest) {
	for (int i = 1; i < grid->rows - 1; ++i) {
		uint8_t cell = grid_get_current(grid, i, col);
		MPI_Send(&cell, 1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
	}
}

static void recv_col(Grid * grid, int col, int src) {
	for (int i = 1; i < grid->rows - 1; ++i) {
		uint8_t cell;
		MPI_Recv(&cell, 1, MPI_CHAR, src, 0, MPI_COMM_WORLD, NULL);
		grid_set_current(grid, i, col, cell);
	}
}

static void send_row(Grid * grid, int row, int dest) {
	for (int i = 1; i < grid->cols - 1; ++i) {
		uint8_t cell = grid_get_current(grid, row, i);
		MPI_Send(&cell, 1, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
	}
}

static void recv_row(Grid * grid, int row, int src) {
	for (int i = 1; i < grid->cols - 1; ++i) {
		uint8_t cell;
		MPI_Recv(&cell, 1, MPI_CHAR, src, 0, MPI_COMM_WORLD, NULL);
		grid_set_current(grid, row, i, cell);
	}
}
