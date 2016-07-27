#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "tctest.h"
#include "grid.h"

typedef struct {
	Grid *small;
} TestObjs;

static TestObjs *setup() {
	TestObjs *t = malloc(sizeof(TestObjs));

	t->small = grid_alloc(10, 20);

	return t;
}

static void cleanup(TestObjs *t) {
	grid_destroy(t->small);
	free(t);
}

static void test_get_size(TestObjs *t) {
	Grid *g = t->small;
	ASSERT(g->rows == 10);
	ASSERT(g->cols == 20);
}

static void test_initially_zero(TestObjs *t) {
	Grid *g = t->small;
	for (int j = 0; j < g->rows; j++) {
		for (int i = 0; i < g->cols; i++) {
			ASSERT(grid_get_current(g, j, i) == 0);
		}
	}
}

static void test_initially_zero_after_flip(TestObjs *t) {
	Grid *g = t->small;
	grid_flip(g);
	for (int j = 0; j < g->rows; j++) {
		for (int i = 0; i < g->cols; i++) {
			ASSERT(grid_get_current(g, j, i) == 0);
		}
	}
}

static void test_set_and_get(TestObjs *t) {
	Grid *g = t->small;
	for (int j = 0; j < g->rows; j++) {
		for (int i = 0; i < g->cols; i++) {
			int val = j*20 + i;
			//printf("Setting: j=%i, i=%i, val=%i\n", j, i, val);
			grid_set_next(g, j, i, val);
		}
	}
	
	// Current generation should still be all zeroes
	for (int j = 0; j < g->rows; j++) {
		for (int i = 0; i < g->cols; i++) {
			ASSERT(grid_get_current(g, j, i) == 0);
		}
	}

	// After a flip we should see the updated values
	grid_flip(g);
	for (int j = 0; j < g->rows; j++) {
		for (int i = 0; i < g->cols; i++) {
			int val = grid_get_current(g, j, i);
			//printf("j=%i, i=%i, val=%i\n", j, i, val);
			ASSERT(val == (j*20 + i));
		}
	}
}

int main(void) {
	TEST_INIT();
	TEST(test_get_size);
	TEST(test_initially_zero);
	TEST(test_initially_zero_after_flip);
	TEST(test_set_and_get);
	TEST_FINI();
}
