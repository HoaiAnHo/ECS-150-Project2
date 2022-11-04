#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define FULL 100

int is_iterating = 0; // boolean to check if in queue iterate

struct node {
	struct node* prev;
	struct node* next;
	void * node_data;
};

struct queue {
    struct node* front;
    struct node* back;
	int queue_len;
};

queue_t queue_create(void)
{
	// allocate space for queue, then assign values to struct members
	queue_t init_queue = (struct queue *) malloc(sizeof (struct queue));
	init_queue->back = NULL;
	init_queue->front = NULL;
	init_queue->queue_len = 0;
	return init_queue;
}

int queue_destroy(queue_t queue) 
{
	// just need to implement freeing parts
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
		struct node* newNode = (struct node*) malloc(sizeof (struct node));
		newNode->prev = NULL;
		newNode->next = NULL;
		newNode->node_data = data;
		queue->front = newNode;
		queue->back = NULL;
	}
	else if (queue->queue_len == 1) {
		struct node* newNode = (struct node*) malloc(sizeof (struct node));
		newNode->prev = queue->front;
		newNode->next = NULL;
		newNode->node_data = data;
		queue->back = newNode;
		queue->front->next = queue->back;
	}
	else {
		struct node * copy_back = queue->back;
		struct node* newNode = (struct node*) malloc(sizeof (struct node));
		newNode->prev = copy_back;
		newNode->next = NULL;
		newNode->node_data = data;
		queue->back = newNode;
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
	queue->front = queue->front->next;
	queue->queue_len -= 1;
	
	return 0;
}

int queue_delete(queue_t queue, void *data) 
{
	// need to free data whenever we "delete" it
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	if (is_iterating)
		return 0;
	// create a pointer to the front node
	struct node* check;
	check = queue->front;
	while(check != NULL){
		if (check->node_data == data){
			if (check == queue->front) {
				queue->front = queue->front->next;
				if (queue->front == queue->back)
					queue->back = NULL;
				else if (queue->front != NULL)
					queue->front->prev = NULL;
				free(check);
			}
			else if (check->next == NULL) {
				check->prev->next = NULL;
				free(check);
			}
			else {
				struct node* temp_node;
				temp_node = check->next;
				// change check's prev's next to check's next
				check->prev->next = check->next;
				// change check's next's prev to check's prev
				check->next->prev = check->prev;
				free(check);
				check = temp_node;
			}
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
	
	is_iterating = 1;

	struct node* check;
	check = queue->front;
	while(check != NULL){
		if (check->node_data == NULL)
			return -1;
		void * initData = check->node_data;
		func(queue, check->node_data);
		if (check->node_data != initData)
			return -1;
		check = check->next;
	}

	is_iterating = 0;
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_len;
}