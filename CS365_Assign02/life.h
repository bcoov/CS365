#ifndef LIFE_H
#define LIFE_H

#include "grid.h"

//
// Load the game board from given file handle.
// The file consists of a series of integers.
// The first two integers specify the number of rows
// and columns, respectively.
// The remaining integers specify cell values (either 0 or 1)
// for all of the cells on the game board, in row-major order.
//
// The Grid returned by the function should have the contents of
// the file as the current generation.
//
Grid *life_load_board(FILE *fp);

//
// In given Grid, compute the next generation based on the
// current generation.
//
void life_compute_next_gen(Grid *grid);

//
// Save the contents of the current generation of the given Grid
// by writing them to the given file handle.
// The file format should be the same as the one expected
// by life_load_board().
//
void life_save_board(FILE *fp, Grid *grid);

#endif // LIFE_H
