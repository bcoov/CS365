#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double xmin, xmax;
	int nrects;

	if (rank == 0) {
		printf("xmin: ");
		fflush(stdout);
		scanf("%lf", &xmin);
	
		printf("xmax: ");
		fflush(stdout);
		scanf("%lf", &xmax);
	
		printf("nrects: ");
		fflush(stdout);
		scanf("%i", &nrects);
	}

	// Broadcast input values
	if (rank == 0) {
		MPI_Bcast(&xmin, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(&xmax, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		MPI_Bcast(nrects, 1, MPI_INT, 0, MPI_COMM_WORLD);
	} else {
		MPI_Recv(&xmin, 1, MPI_DOUBLE, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&xmax, 1, MPI_DOUBLE, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&nrects, 1, MPI_DOUBLE, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	// Divide up work
	// TODO

	// Local computation
	// TODO

	// Reduce local partial results to produce one global result
	// TODO

	// Print the global result (process 0 only)
	// TODO

	MPI_Finalize();

	return 0;
}
