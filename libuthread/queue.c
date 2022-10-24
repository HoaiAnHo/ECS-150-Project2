#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define FULL 100

struct queue {
    /* TODO Phase 1 */
	int* arr;
    int front;
    int back;
	int queue_len;
	//we're queueing and dequeing addresses
};

// queue_t should point to an address
// that address should contain the data?

queue_t queue_create(void)
{
	queue_t init_queue;
	int queue_arr[FULL];
	init_queue->arr = queue_arr;
	init_queue->queue_len = 0;
	init_queue->front = -1;
	init_queue->back = -1;
	return init_queue;
}

int queue_destroy(queue_t queue)
{
	if (queue->queue_len != 0 || queue == NULL){
		return -1;
	}
	// Deallocate the memory associated to the queue object pointed by @queue
	// for every queue->arr[i], free(queue->arr[i])
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}
	// if the queue can be added to
	if (queue->back < FULL - 1){
		if (queue->front == -1) queue->front = 0;
		queue->back += 1;
		queue->arr[queue->back] = &data;
	}
	// if queue is full
	else{
		return -1;
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
		*data = queue->arr[queue->front];
		queue->front += 1;
	}
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	int i = queue->front;
	while(1){
		if (i > queue->back) return -1;
		if (queue->arr[i] == data){
			// delete item in queue->arr
			return 0;
		}
	}
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue->queue_len == 0 || queue == NULL || func == NULL){
		return -1;
	}
	int i = queue->front;
	while(i <= queue->back){
		if (queue->arr[i] == NULL)
		{
			i++;
			continue;
		}
		func(queue, queue->arr[i]);
		i++;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_len;
}
