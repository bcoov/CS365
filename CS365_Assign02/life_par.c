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
	for (int i = 1, pos_i = v_chunk_size*rank_row;
		 i < local->rows-1; ++i, ++pos_i) {
		for (int j = 1, pos_j = h_chunk_size*rank_col;
			 j < local->cols-1; ++j, ++pos_j) {
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
			send_col(local, local->cols - 2, right_n);

			// receive from right
			recv_col(local, local->cols - 1, right_n);
		}
		// Top Neighbour
		if (rank_row != 0) {
			// send row up
			int top_n = rank - M;
			send_row(local, 1, top_n);

			// receive from top
			recv_row(local, 0, top_n);
		}
		// Bottom Neighbour
		if (rank_row != N - 1) {
			// send row down
			int bot_n = rank + M;
			send_row(local, local->rows - 2, bot_n);

			// receive from bottom
			recv_row(local, local->rows - 1, bot_n);
		}
		// Corner Comms
		// Top-Left Corner
		if (rank_row != 0 && rank_col != 0) {
			// Send corner cell
			uint8_t to_send = grid_get_current(local, 1, 1);
			int corner = rank - M - 1;
			MPI_Send(&to_send, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD);

			// Receive corner cell
			uint8_t val;
			MPI_Recv(&val, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD, NULL);
			grid_set_current(local, 0, 0, val);
		}
		// Top-Right Corner
		if (rank_row != 0 && rank_col != M - 1) {
			uint8_t to_send = grid_get_current(local, 1, local->cols - 2);
			int corner = rank - M + 1;
			MPI_Send(&to_send, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD);

			uint8_t val;
			MPI_Recv(&val, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD, NULL);
			grid_set_current(local, 0, local->cols - 1, val);
		}
		// Bottom-Left Corner
		if (rank_row != N - 1 && rank_col != 0) {
			uint8_t to_send = grid_get_current(local, local->rows - 2, 0);
			int corner = rank + M - 1;
			MPI_Send(&to_send, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD);

			uint8_t val;
			MPI_Recv(&val, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD, NULL);
			grid_set_current(local, local->rows - 1, 0, val);
		}
		// Bottom-Right Corner
		if (rank_row != N - 1 && rank_col != M - 1) {
			uint8_t to_send = grid_get_current(local,
											   local->rows - 2,
											   local->cols - 2);
			int corner = rank + M + 1;
			MPI_Send(&to_send, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD);

			uint8_t val;
			MPI_Recv(&val, 1, MPI_CHAR, corner, 0, MPI_COMM_WORLD, NULL);
			grid_set_current(local, local->rows - 1, local->cols - 1, val);
		}

		// Local computation
		life_compute_next_gen(local, 1, 1);

		grid_flip(local);
	}
	sleep(rank);
	printf("Rank: %d (After)\n", rank);
	life_save_board(stdout, local);
	fflush(stdout);

	// Non-root processes: let the root process know how many
	// data values will be sent
	if (rank != 0) {
		int cells_to_send = (local->rows-2) * (local->cols-2);
		MPI_Send(&cells_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	// Global reconstruction
	for (int i = 1, pos_i = v_chunk_size*rank_row;
		 i < local->rows-1; ++i, ++pos_i) {
		for (int j = 1, pos_j = h_chunk_size*rank_col;
			 j < local->cols-1; ++j, ++pos_j) {
			//printf("local[%d,%d] -> grid[%d,%d] (rank: %d)\n", i, j, pos_i, pos_j, rank);
			//fflush(stdout);
			uint8_t val = grid_get_current(local, i, j);
			if (rank == 0) {
				grid_set_current(grid, pos_i, pos_j, val);
			} else {
				// Send to root process
				MPI_Send(&pos_i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(&pos_j, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(&val, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			}
		}
	}

	// Root process: receive data from other processes
	if (rank == 0) {
		for (int proc = 1; proc < size; proc++) {
			int cells_to_recv;
			// call to MPI_Recv
			for (int i = 0; i < cells_to_recv; i++) {
				// three calls to MPI_Recv, one call to grid_set_current
			}
		}
	}

	MPI_Finalize();

	if (rank == 0){
		printf("Final state:\n");
		life_save_board(stdout, grid);
		fflush(stdout);
	}

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
