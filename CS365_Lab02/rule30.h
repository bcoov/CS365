#ifndef RULE30_H
#define RULE30_H

#include <stdint.h>

typedef struct {
	int num_cells;
	uint8_t *cur;
	uint8_t *next;
} State;

//
// Read initial state from given file handle.
//
State *rule30_load(FILE *fp);

//
// Flip (swap) current and next generations.
//
void rule30_flip(State *state);

//
// Based on the current generation, compute the state of the
// next generation.
//
void rule30_compute_next(const uint8_t *current, uint8_t *next, int num_cells);

uint8_t compute_rule30(uint8_t left, uint8_t self, uint8_t right);

#endif // RULE30_H
