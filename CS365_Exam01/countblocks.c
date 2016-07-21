// CS 365 - Spring 2015 - Exam 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <mpi.h>
#include "grid.h"

// Use this to help divide up the work if you want to
void divide_work(int n, int num_chunks, int chunk_index, int *start_index, int *end_index)
{
	assert(n > 0);
	assert(num_chunks < n);

	int chunk_size = n / num_chunks;
	int r = n % num_chunks;

	*start_index = (chunk_index * chunk_size);

	if (chunk_index < r) {
		*start_index += chunk_index;
	} else {
		*start_index += r;
	}

	*end_index = (*start_index) + chunk_size;
	if (chunk_index < r) {
		(*end_index)++;
	}
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (argc != 5) {
		fprintf(stderr, "Usage: ./runpar <filename> <q> <N> <M>\n");
		exit(1);
	}

	const char *filename = argv[1];
	int q = atoi(argv[2]);  // the integer q
	int N = atoi(argv[3]);  // number of rows of processes
	int M = atoi(argv[4]);  // number of columns of processes

	printf("filename=%s, N=%i, M=%i\n", filename, N, M);

	// Load the overall (global) Grid
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Couldn't open %s: %s\n", filename, strerror(errno));
		exit(1);
	}
	Grid *global = grid_load(fp);
	fclose(fp);

	int global_num_rows = global->rows;
	int global_num_cols = global->cols;
	uint8_t global_count;

	// TODO: have this process determine its region of the global Grid
	int proc_row = rank / M; // which row of processes is this process in
	int proc_col = rank % M; // which column of processes is this process in

	int p_rows = global_num_rows / N;
	int p_cols = global_num_cols / M;

	int pr_start = 0 + (p_rows * proc_row);
	int pc_start = 0 + (p_cols * proc_col);

	// Add excess once starting point is found
	if (proc_row == N - 1) {
		p_rows += global_num_rows % N;
	}
	if (proc_col == M - 1) {
		p_cols += global_num_cols % M;
	}

	int pr_end = pr_start + p_rows;
	int pc_end = pc_start + p_cols;

	//printf("Start: %d, End: %d; (Rank: %d)\n", pr_start, pr_end, rank);
	//printf("Start: %d, End: %d; (Rank: %d)\n", pc_start, pc_end, rank);

	// TODO: local computation: count how many 3x3 blocks in this
	// process's region sum to q
	uint8_t p_count = 0;

	for (int row = pr_start; row < pr_end - 2; row++) {
		for (int col = pc_start; col < pc_end - 2; col++) {
			uint8_t UL = grid_get_current(global, row, col);
			uint8_t UM = grid_get_current(global, row, col + 1);
			uint8_t UR = grid_get_current(global, row, col + 2);
			uint8_t ML = grid_get_current(global, row + 1, col);
			uint8_t MM = grid_get_current(global, row + 1, col + 1);
			uint8_t MR = grid_get_current(global, row + 1, col + 2);
			uint8_t BL = grid_get_current(global, row + 2, col);
			uint8_t BM = grid_get_current(global, row + 2, col + 1);
			uint8_t BR = grid_get_current(global, row + 2, col + 2);

			uint8_t total = (UL + UM + UR +
							 ML + MM + MR +
							 BL + BM + BR);
			//printf("%d %d %d %d %d %d %d %d %d => %d\n", UL, UM, UR, ML, MM, MR, BL, BM, BR, total);
			if (total == q) {
				p_count++;
			}
		}
	}
	//printf("count for rank %d: %d\n", rank, p_count);

	// TODO: reduce all of the local results to a single
	// overall result, report it (hint: MPI_Reduce)
	MPI_Reduce(&p_count, &global_count, 1, MPI_CHAR, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Result is %d\n", global_count);
	}

	MPI_Finalize();

	return 0;
}
