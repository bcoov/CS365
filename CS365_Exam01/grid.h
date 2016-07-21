#ifndef GRID_H
#define GRID_H

#include <stdint.h>

typedef struct {
	uint8_t *buf1;
	uint8_t *buf2;
	int rows;
	int cols;
} Grid;

// 
// Create a Grid instance with given number of rows and columns.
// All elements of both the current and next generations should
// be set to 0.
// 
Grid *grid_alloc(int rows, int cols);

// 
// Free a Grid instance previously allocated by grid_alloc.
// 
void grid_destroy(Grid *grid);

// 
// Flip the buffers in given Grid instance, so that the next
// generation becomes the current generation and the current
// generation becomes the next generation.
// 
void grid_flip(Grid *grid);

//
// Get the value stored in given row and column of the current
// generation of the given Grid.
//
uint8_t grid_get_current(Grid *grid, int row, int col);

//
// Set the value stored in given row and column of the next
// generation of the given Grid.
//
void grid_set_next(Grid *grid, int row, int col, uint8_t val);

//
// Load Grid data from a file.
// Returns a pointer to a new Grid object.
//
Grid *grid_load(FILE *fp);

#endif // GRID_H
