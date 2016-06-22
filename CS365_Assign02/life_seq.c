// Conway's Game of Life - sequential version

#include <stdio.h>
#include <stdlib.h>
#include "grid.h"
#include "life.h"

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: life_seq <filename> <numgens>\n");
		exit(1);
	}

	const char *filename = argv[1];
	int numgens = atoi(argv[2]);

	// read file
	FILE *fp = fopen(filename, "r");
	Grid *grid = life_load_board(fp);
	fclose(fp);

	// simulation
	for (int i = 0; i < numgens; i++) {
		life_compute_next_gen(grid, 0, 0);
		grid_flip(grid);
	}

	// print result
	life_save_board(stdout, grid);

	return 0;
}
