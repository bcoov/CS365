#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "grid.h"

static inline int compute_index(Grid *g, int row, int col) {
	return row * g->cols + col;
}

Grid *grid_alloc(int rows, int cols)
{
	Grid *g = malloc(sizeof(Grid));
	g->rows = rows;
	g->cols = cols;
	size_t size = rows*cols*sizeof(uint8_t);
	g->buf1 = malloc(size);
	g->buf2 = malloc(size);
	memset(g->buf1, 0, size);
	memset(g->buf2, 0, size);
	return g;
}

void grid_destroy(Grid *g)
{
	free(g->buf2);
	free(g->buf1);
	free(g);
}

void grid_flip(Grid *g)
{
	uint8_t *tmp = g->buf1;
	g->buf1 = g->buf2;
	g->buf2 = tmp;
}

uint8_t grid_get_current(Grid *g, int row, int col)
{
	if (row < 0 || row >= g->rows || col < 0 || col >= g->cols) {
		return 0;
	}
	uint8_t value = g->buf1[compute_index(g, row, col)];
	return value;
}

void grid_set_next(Grid *g, int row, int col, uint8_t val)
{
	g->buf2[compute_index(g, row, col)] = val;
}


Grid *grid_load(FILE *fp)
{
	int rows, cols;
	fscanf(fp, "%i %i", &rows, &cols);
	Grid *g = grid_alloc(rows, cols);
	for (int j = 0; j < rows; j++) {
		for (int i = 0; i < cols; i++) {
			int val;
			fscanf(fp, "%i", &val);
			grid_set_next(g, j, i, (uint8_t)val);
		}
	}

	grid_flip(g); // make next generation the current one

	return g;
}

// vim:ts=2:
