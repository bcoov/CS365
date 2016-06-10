#include <stdio.h>
#include "life.h"
#include "grid.h"

Grid *life_load_board(FILE *fp)
{
	int num_rows;
	int num_cols;

	fscanf(fp, "%i", &num_rows);
	fscanf(fp, "%i", &num_cols);

	Grid * grid = grid_alloc(num_rows, num_cols);

	for (int i = 0; i < (num_rows * num_cols); ++i) {
		int val;
		fscanf(fp, "%i", &val);
		grid->curr_gen[i] = (uint8_t) val;
	}

	return grid;
}

void life_compute_next_gen(Grid *grid)
{
	/*
	1 -> 1 if 2 or 3 neighbors
	0 -> 1 if 3 neighbors
	**other rules** (not included in assignment?)
	1 -> 0 if < 2 neighbors
	1 -> 0 if > 3 neighbors
	*/

	/*for (int i = 0; i < grid->rows; ++i) {
		for (int j = 0; j < grid->cols; ++j) {

		}
	}*/
}

//uint8_t life_compute_rule()

void life_save_board(FILE *fp, Grid *grid)
{
	fprintf(fp, "%i %i\n", grid->rows, grid->cols);

	int count = 0;
	for (int i = 0; i < (int) (grid->rows * grid->cols); ++i) {
		fprintf(fp, "%i", (int) grid->curr_gen[i]);
		++count;

		if (count == grid->cols) {
			fprintf(fp, "\n");
			count = 0;
		} else {
			fprintf(fp, " ");
		}
	}
}
