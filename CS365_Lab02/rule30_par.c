#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rule30.h"

static void barf(const char *msg)
{
	printf("%s: %s\n", msg, strerror(errno));
	MPI_Finalize();
	exit(1);
}

static void print(State *state)
{
	for (int i = 0; i < state->num_cells; i++) {
		printf("%c", state->cur[i] != 0 ? 'x' : '.');
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// Read initial state (every process can do this)
	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		barf("Couldn't load initial state");
	}
	State *state = rule30_load(fp);
	fclose(fp);

	// Determine what work this processor should do
	int chunk_size = state->num_cells / size;
	int start = rank * chunk_size;
	if (rank == size - 1) {
		chunk_size += (state->num_cells % size);
	}
	int end = start + chunk_size;
    int last = chunk_size + 1;

	// Get ready for local computation: for example, copy part of the
	// global data (from state->cur) into a separate array
	State * local = malloc(sizeof(State));
	local->num_cells = chunk_size;
	local->cur = malloc(sizeof(uint8_t *) * chunk_size + (2 * sizeof(uint8_t)));
	local->next = malloc(sizeof(uint8_t *) * chunk_size + (2 * sizeof(uint8_t)));
	for (int i = start; i < end; ++i) {
		int pos = (i - start) + 1;
		local->cur[pos] = state->cur[i];
	}

	// How many generations to simulate
	int num_gens = atoi(argv[2]);
    //printf("Simulating %d generations\n", num_gens);
    //fflush(stdout);

    /** Use new State object for local computations (can then use the
     * "computenext" function instead of re-writing here). On zero
     * generations, result never gets initiatlized.
     */
	for (int i = 1; i <= num_gens; i++) {
        local->cur[0] = 0;
        local->cur[last] = 0;

		// send left?
		if (rank != 0) {
            //printf("sending left from process %d\n", rank);
			MPI_Send(&local->cur[1], 1, MPI_CHAR, rank - 1, 0, MPI_COMM_WORLD);
		}

		// receive from right?
		if (rank != (size - 1)) {
            //printf("receiving right from process %d\n", rank);
			MPI_Recv(&local->cur[last], 1, MPI_CHAR, rank + 1, MPI_ANY_TAG,
					 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// send right?
		if (rank != (size - 1)) {
            //printf("sending right from process %d\n", rank);
			MPI_Send(&local->cur[last - 1], 1, MPI_CHAR, rank + 1, 0, MPI_COMM_WORLD);
		}

		// receive from left?
		if (rank != 0) {
            //printf("receiving left from process %d\n", rank);
			MPI_Recv(&local->cur[0], 1, MPI_CHAR, rank - 1, MPI_ANY_TAG,
					 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
        
		// Compute values for inner "local" portion
		// (Ignoring edges for now)
		/*for (int i = 1; i < chunk_size - 1; ++i) {
			uint8_t left = local->cur[i-1];
			uint8_t right = local->cur[i+1];
			uint8_t self = local->cur[i];

			local->next[i] = compute_rule30(left, self, right);
		}
		// Compute edges
		local->next[0] = compute_rule30(left_bound,
										local->cur[0],
										local->cur[1]);
		local->next[last] = compute_rule30(local->cur[last-1],
										   local->cur[last],
										   right_bound);*/
        rule30_compute_next(local->cur, local->next, chunk_size + 2);

		// flip current and next generation
		rule30_flip(local);
	}

	// combine solutions
	if (rank == 0) {
		// copy local data
		for (int i = 1; i < chunk_size; i++) {
            state->cur[i - 1] = local->cur[i];
        }
		// receive data from other processes
		for (int i = 1; i < size; i++) {
            int in_size;
            int index;
            //uint8_t * elems;
            
            MPI_Recv(&in_size, 1, MPI_INT, i, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&index, 1, MPI_INT, i, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&state->cur[index], in_size, MPI_CHAR, i, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        print(state);
	} else {
		// Send local data to process 0
		MPI_Send(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&local->cur[1], chunk_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
