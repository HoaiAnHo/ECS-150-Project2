#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> //for debugging, remove later
#include <string.h>

#include "queue.h"

#define FULL 100

int is_iterating = 0; //boolean to check if in queue iterate

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
	queue_t init_queue = (struct queue *) malloc(sizeof (struct queue));
	init_queue->back = NULL;
	init_queue->front = NULL;
	init_queue->queue_len = 0;
	return init_queue;
}

int queue_destroy(queue_t queue) //just need to implement freeing parts
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
	else if (queue->queue_len == 1) {
		//printf("queue->back is NULL\n");
		struct node* newNode = (struct node*) malloc(sizeof (struct node));
		newNode->prev = queue->front;
		newNode->next = NULL;
		newNode->node_data = data;
		//printf("node data: %p\n", (int *) newNode->node_data);
		queue->back = newNode;
		queue->front->next = queue->back;
	}
	else {
		//printf("QUEUE IS NOT EMPTY\n");
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
	*data = queue->front->node_data; //->node_data; <- can't do this
	//printf("data: %p\n", queue->front->node_data);
	queue->front = queue->front->next;
	queue->queue_len -= 1;
	
	return 0;
}

int queue_delete(queue_t queue, void *data) //need to free data whenever we "delete" it
{
	// if (queue->queue_len == -1){
	// 	return -2;
	// }
	// null cases
	if (queue->queue_len == 0 || queue == NULL || data == NULL){
		return -1;
	}
	if (is_iterating)
		return 0;
	// create a pointer to the front node
	struct node* check;
	check = queue->front;
	while(check != NULL){
		//printf("Hi 1! node data: %p data: %p\n", check->node_data, data);
		if (check->node_data == data){
			//printf("Hi!\n");
			if (check == queue->front) {
				queue->front = queue->front->next;
				if (queue->front == queue->back)
					queue->back = NULL;
				else if (queue->front != NULL)
					queue->front->prev = NULL;
				free(check); //edit - not in csif yet
			}
			else if (check->next == NULL) {
				check->prev->next = NULL;
				free(check); //edit - not in csif yet
			}
			else {
				struct node* temp_node;
				temp_node = check->next;
				// change check's prev's next to check's next
				check->prev->next = check->next;
				// change check's next's prev to check's prev
				check->next->prev = check->prev;
				free(check); //edit - not in csif yet
				check = temp_node;
			}
			queue->queue_len -= 1;
			//free(check);
			//printf("Bye!\n");
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
	printf("first val in queue: %d\n", *(int *) check->node_data);
	// queue_t test;
	// test->queue_len = -1;
	while(check != NULL){
		// int deletion_check = func(test, check);
		// if (func(test, check) == -2){
		// 	check = check->next;
		// 	continue;
		// }
		if (check->node_data == NULL)
			printf("seg fault\n");
		void * initData = check->node_data;
		printf("Going to run func on data %d in queue\n", *(int *) check->node_data);
		func(queue, check->node_data);
		printf("I ran func and got %d\n\n", *(int *) check->node_data);
		if (check->node_data != initData)
			printf("I deleted the data here\n");
		check = check->next;
	}

	is_iterating = 0;
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_len;
}
