// CS 365 - Lab 1

// if last process (size - 1), add excess to end of range
// a.k.a. max % size

#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	// TODO: add your code here
	int size;

	MPI_Init(&argc, &argv);

	// Get number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Hello from process %i\n", rank);

	int n;
	if (rank == 0) {
		scanf("%d", &n);
		// Send n to processes 1 through "size"
		for (int i = 1; i < size; ++i) {
			MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	} else {
		// Receive n from process zero
		MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG,
				 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

    int chunk = n / size;
    int excess = n % size;
    int sum = 0;
    int start = (rank * chunk) + 1;
    int end = ((rank + 1) * chunk) + 1;

    if (rank == size - 1) {
        end += excess;
    }
    
    for (int i = start; i < end; ++i) {
        sum += i;
    }
    
    printf("Rank %i: start %d end %d sum %d\n", rank, start, end, sum);
    
	if (rank != 0) {
		// Send computed local sum to process zero
		MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	} else {
		// Receive processes' sums and add into zero's local sum
		int part_sum = 0;
		for (int i = 1; i < size; ++i) {
			MPI_Recv(&part_sum, 1, MPI_INT, i, MPI_ANY_TAG,
					 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sum += part_sum;
		}

		printf("Global sum is %d\n", sum);
	}

	MPI_Finalize();

	return 0;
}

