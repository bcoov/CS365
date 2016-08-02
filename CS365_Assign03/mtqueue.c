#include <stdio.h>
#include <stdlib.h>
#include "mtqueue.h"

MTQueue *mtqueue_create(void)
{
	MTQueue *q = malloc(sizeof(MTQueue));

	q->head = q->tail = NULL;
    //q->head->next = q->tail;

	pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);

	return q;
}

void mtqueue_enqueue(MTQueue *q, void *item)
{
	MTQueueNode * new_node = malloc(sizeof(MTQueueNode));
    new_node->item = item;
    new_node->next = NULL;

    pthread_mutex_lock(&q->lock);

	if (q->head == NULL) {
		q->head = new_node;
		q->tail = new_node;
	}
	else {
	    q->tail->next = new_node;
    	q->tail = new_node;
	}
    pthread_cond_broadcast(&q->cond);

    pthread_mutex_unlock(&q->lock);
}

void *mtqueue_dequeue(MTQueue *q)
{
	//mtqueue_wait_until_empty(q);

    pthread_mutex_lock(&q->lock);

	// wait for queue to become non-empty
	while (q->head == NULL) {
		pthread_cond_wait(&q->cond, &q->lock);
	}

    MTQueueNode * to_remove = q->head;
    q->head = q->head->next;
    //free(q->head);

	// if head became null, make tail null as well
	if (q->head == NULL) {
		q->tail = NULL;
	}
	pthread_cond_broadcast(&q->cond);

    pthread_mutex_unlock(&q->lock);

    //return to_remove->item;
	void *item = to_remove->item;
	free(to_remove);
	return item;
}

void mtqueue_wait_until_empty(MTQueue *q)
{
	pthread_mutex_lock(&q->lock);

    while (q->head != NULL) {
        pthread_cond_wait(&q->cond, &q->lock);
    }

    pthread_mutex_unlock(&q->lock);
}
