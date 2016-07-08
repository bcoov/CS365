#include <stdio.h>
#include <stdlib.h>
#include "mtqueue.h"

MTQueue *mtqueue_create(void)
{
	MTQueue *q = malloc(sizeof(MTQueue));

	q->head = q->tail = NULL;

	// TODO: initialize mutex and condition variable(s)

	return q;
}

void mtqueue_enqueue(MTQueue *q, void *item)
{
	// TODO: implement
}

void *mtqueue_dequeue(MTQueue *q)
{
	// TODO: implement
}

void mtqueue_wait_until_empty(MTQueue *q)
{
	// TODO: implement
}
