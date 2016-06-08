#include <stdio.h>
#include <stdlib.h>
#include "rule30.h"

State *rule30_load(FILE *fp)
{
	int num_cells;
	fscanf(fp, "%i", &num_cells);

	State *state = malloc(sizeof(State));
	state->num_cells = num_cells;

	state->cur = (uint8_t *) malloc(sizeof(uint8_t) * num_cells);
	for (int i = 0; i < num_cells; i++) {
		int value;
		fscanf(fp, "%i", &value);
		state->cur[i] = (uint8_t) value;
	}

	state->next = (uint8_t *) malloc(sizeof(uint8_t) * num_cells);

	return state;
}

void rule30_flip(State *state)
{
	uint8_t *tmp = state->cur;
	state->cur = state->next;
	state->next = tmp;
}

void rule30_compute_next(const uint8_t *current, uint8_t *next, int num_cells)
{
	for (int i = 0; i < num_cells; i++) {
		uint8_t left = 0;
		uint8_t right = 0;
		uint8_t self = current[i];

		// Handle (literal) edge cases
		// No "Left" for first element (or considered as 0)
		if (i != 0) {
			left = current[i - 1];
		}
		// No "Right" for last element (or considered as 0)
		if (i != num_cells - 1) {
			right = current[i + 1];
		}

		next[i] = compute_rule30(left, self, right);		
	}
}

uint8_t compute_rule30(uint8_t left, uint8_t self, uint8_t right) {
	// Determine new value based on neighbors
	// 111 => 0
	if (left == 1 && self == 1 && right == 1) {
		return 0;
	}
	// 110 => 0
	else if (left == 1 && self == 1 && right == 0) {
		return 0;
	}
	// 101 => 0
	else if (left == 1 && self == 0 && right == 1) {
		return 0;
	}
	// 100 => 1
	else if (left == 1 && self == 0 && right == 0) {
		return 1;
	}
	// 011 => 1
	else if (left == 0 && self == 1 && right == 1) {
		return 1;
	}
	// 010 => 1
	else if (left == 0 && self == 1 && right == 0) {
		return 1;
	}
	// 001 => 1
	else if (left == 0 && self == 0 && right == 1) {
		return 1;
	}
	// 000 => 0
	else {
		return 0;
	}
}
