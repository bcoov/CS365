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

void life_compute_next_gen(Grid *grid, int offset_start, int offset_end)
{
	for (int i = 0 + offset_start; i < grid->rows - offset_end; ++i) {
		for (int j = 0 + offset_start; j < grid->cols - offset_end; ++j) {
			// Get values for all eight neighbors
			uint8_t UL = grid_get_current(grid, i-1, j-1);
			uint8_t UM = grid_get_current(grid, i-1, j);
			uint8_t UR = grid_get_current(grid, i-1, j+1);
			uint8_t ML = grid_get_current(grid, i, j-1);
			uint8_t MR = grid_get_current(grid, i, j+1);
			uint8_t BL = grid_get_current(grid, i+1, j-1);
			uint8_t BM = grid_get_current(grid, i+1, j);
			uint8_t BR = grid_get_current(grid, i+1, j+1);
			uint8_t self = grid_get_current(grid, i, j);

			// Find total living neighbors
			uint8_t neighbors = (UL + UM + UR + ML + MR + BL + BM + BR);
			// 1 -> 0 if < 2 neighbors
			if (self == 1 && neighbors < 2) {
				grid_set_next(grid, i, j, 0);
			}
			// 1 -> 1 if 2 or 3 neighbors
			else if (self == 1 && (neighbors == 2 || neighbors == 3)) {
				grid_set_next(grid, i, j, 1);
			}
			// 1 -> 0 if > 3 neighbors
			else if (self == 1 && neighbors > 3) {
				grid_set_next(grid, i, j, 0);
			}
			// 0 -> 1 if 3 neighbors
			else if (self == 0 && neighbors == 3) {
				grid_set_next(grid, i, j, 1);
			}
			else {
				grid_set_next(grid, i, j, self);
			}
		}
	}
}

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
