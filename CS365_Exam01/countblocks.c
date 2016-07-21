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

	// TODO: have this process determine its region of the global Grid
	int proc_row = rank / M; // which row of processes is this process in
	int proc_col = rank % M; // which column of processes is this process in

	// TODO: local computation: count how many 3x3 blocks in this
	// process's region sum to q

	// TODO: reduce all of the local results to a single
	// overall result, report it (hint: MPI_Reduce)

	MPI_Finalize();

	return 0;
}
