#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	double xmin, xmax, total;
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
	MPI_Bcast(&xmin, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&xmax, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&nrects, 1, MPI_INT, 0, MPI_COMM_WORLD);

	// Divide up work
	int chunk_size = nrects / size;
	int extra = nrects % size;
	double step = (xmax - xmin) / (double) nrects;
	int start = chunk_size * rank;
	int end = chunk_size * (rank + 1);
	if (rank == (size - 1)) {
		end += extra;
	}
	//printf("Step width: %f\n", step);
	//printf("Rank %d: Start at %d, end at %d\n", rank, start, end);
	//fflush(stdout);

	// Local computation
	double x = xmin + (start * step) + (step / 2.0);
	double y = 0.00;
	for (int i = start; i < end; i++, x += step) {
		// y = sin(x) + x^1.3 cos(x) + x ln(x)
		if (x != 0) {
			y += (sin(x) + (pow(x, 1.3) * cos(x)) + (x * log(x))) * step;
		}
	}

	//printf("Rank %d's y: %f\n", rank, y);
	//fflush(stdout);
	// Reduce local partial results to produce one global result
	MPI_Reduce(&y, &total, 1, MPI_DOUBLE,
               MPI_SUM, 0, MPI_COMM_WORLD);

	// Print the global result (process 0 only)
	if (rank == 0) {
		printf("Global total: %f\n", total);
	}

	MPI_Finalize();

	return 0;
}
