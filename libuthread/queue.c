#include <stdint.h>
#include <stdlib.h>
//#include <stdio.h> //for debugging, remove later
#include <string.h>

#include "queue.h"

#define FULL 100

struct node {
	struct node* prev; //node infront of current
	struct node* next; //node behind current
	void * node_data; //changing void * int *
};

/*
 - context init (given for uthread create)
*/

struct queue {
    struct node* front;
    struct node* back;
	int queue_len;
};

queue_t queue_create(void)
{
	//struct queue q = malloc(sizeof (struct queue));// = {NULL, NULL, 0};
	queue_t init_queue = (struct queue *) malloc(sizeof (struct queue));
	init_queue->back = NULL;
	init_queue->front = NULL;
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
	queue->queue_len = 0;
	return 0;
}

int queue_enqueue(queue_t queue, void *data) 
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	if (queue->queue_len == 0) {
		//printf("QUEUE IS EMPTY\n");
		struct node* newNode = (struct node*) malloc(sizeof (struct node));
		newNode->prev = NULL;
		newNode->next = NULL;
		newNode->node_data = data;
		//printf("node data: %p\n", (int *) newNode->node_data);
		queue->front = newNode;
		queue->back = NULL;
	}
	else {
		//printf("QUEUE IS NOT EMPTY\n");
		struct node * copy_back = queue->back;
		queue->back = &(struct node) {copy_back, NULL, data};
		if (copy_back != NULL)
			copy_back->next = queue->back;
	}

	queue->queue_len += 1;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue->queue_len == 0 || queue == NULL || data == NULL || queue->front == NULL){
		return -1;
	}
	*data = queue->front->node_data;
	//printf("data: %p\n", queue->front->node_data);
	queue->front = queue->front->next;
	queue->queue_len -= 1;
	
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	// if (queue->queue_len == -1){
	// 	return -2;
	// }
	// null cases
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	// create a pointer to the front node
	struct node* check;
	check = queue->front;
	while(check){
		if (check->node_data == data){
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
