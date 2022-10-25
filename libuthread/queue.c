#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define FULL 100

struct node {
	struct node* prev;
	struct node* next;
	int node_data;
};

struct queue {
    struct node* front;
    struct node* back;
	int queue_len;
};

queue_t queue_create(void)
{
	queue_t init_queue = NULL;
	init_queue->front = NULL;
	init_queue->back = NULL;
	init_queue->queue_len = 0;
	return init_queue;
}

int queue_destroy(queue_t queue)
{
	if (queue->queue_len != 0 || queue == NULL){
		return -1;
	}
	queue->front = NULL;
	queue->back = NULL;
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}
	// if the queue can be added to
	if (queue->front == NULL) {
		struct node new = {NULL, NULL, *(int*) data};
		*queue->front = new;
	}
	else if (queue->front != NULL && queue->back == NULL){
		struct node new = {queue->front, NULL, *(int*) data};
		*queue->back = new;
	}
	else{
		struct node new = {queue->back, NULL, *(int*) data};
		*queue->back->next = new;
		*queue->back = new;
	}
	queue->queue_len += 1;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	else{
		data = (void *) &(queue->front->node_data);
		*queue->front = *queue->front->next;
		queue->queue_len -= 1;
	}
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue->queue_len == -1){
		return -2;
	}
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	struct node* check;
	check = queue->front;
	while(check){
		if (check->node_data == *(int*) data){
			struct node* temp_node;
			temp_node = check->next;
			// change check's prev's next to check's next
			check->prev->next = check->next;
			// change check's next's prev to check's prev
			check->next->prev = check->prev;
			check = temp_node;
			queue->queue_len -= 1;
			return 0;
		}
		check = check->next;
	}
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue->queue_len == 0 || queue == NULL || func == NULL){
		return -1;
	}
	struct node* check;
	check = queue->front;
	// queue_t test;
	// test->queue_len = -1;
	while(check){
		// int deletion_check = func(test, check);
		// if (func(test, check) == -2){
		// 	check = check->next;
		// 	continue;
		// }
		func(queue, check);
		check = check->next;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_len;
}
