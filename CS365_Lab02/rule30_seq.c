#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rule30.h"

static void barf(const char *msg)
{
	printf("%s: %s\n", msg, strerror(errno));
	exit(1);
}

static void print(State *state)
{
	for (int i = 0; i < state->num_cells; i++) {
		printf("%c", state->cur[i] != 0 ? 'x' : ' ');
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: rule30_seq <input file> <numgens>\n");
		exit(1);
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		barf("Couldn't open file");
	}
	State *state = rule30_load(fp);
	fclose(fp);

	int num_gens = atoi(argv[2]);

	print(state);
	for (int i = 1; i <= num_gens; i++) {
		rule30_compute_next(state->cur, state->next, state->num_cells);
		rule30_flip(state);
		print(state);
	}

	return 0;
}
