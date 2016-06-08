#include <stdlib.h>
#include <string.h>
#include "grid.h"

Grid *grid_alloc(int rows, int cols)
{
	Grid * grid = malloc(sizeof(Grid));
	grid->rows = rows;
	grid->cols = cols;
	int grid_size = rows * cols;

	grid->curr_gen = (uint8_t *) malloc(sizeof(uint8_t *) * grid_size);
	grid->next_gen = (uint8_t *) malloc(sizeof(uint8_t *) * grid_size);

	// Initialize current to all zero
	for (int i = 0; i < grid_size; ++i) {
		grid->curr_gen[i] = 0;
	}

	return grid;
}

void grid_destroy(Grid *grid)
{
	free(grid->curr_gen);
	free(grid->next_gen);
}

void grid_flip(Grid *grid)
{
	uint8_t * flip = grid->curr_gen;
	grid->curr_gen = grid->next_gen;
	grid->next_gen = flip;
}

uint8_t grid_get_current(Grid *grid, int row, int col)
{
	int loc = (row * grid->cols) + col;
	return grid->curr_gen[loc];
	// return 0 if out of bounds
}

void grid_set_next(Grid *grid, int row, int col, uint8_t val)
{
	int loc = (row * grid->cols) + col;
	grid->next_gen[loc] = val;
}
