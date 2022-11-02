#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

////////////////////////////////////////////////////////////////////// for testing
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
			return -1;
			//printf("seg fault\n");
		void * initData = check->node_data;
		//printf("Going to run func on data %d in queue\n", *(int *) check->node_data);
		func(queue, check->node_data);
		//printf("I ran func and got %d\n\n", *(int *) check->node_data);
		if (check->node_data != initData)
			return -1;
			//printf("I deleted the data here\n");
		check = check->next;
	}

	is_iterating = 0;
	return 0;
}

int queue_length(queue_t queue)
{
	return queue->queue_len;
}

////////////////////////////////////////////////////////////////

queue_t ready_queue; 
queue_t blocked_queue; // push running thread to this queue
//front of queue will always be running

enum state {
	running = 0,
	ready = 1,
	blocked = 2,
	exited = 3
};

struct uthread_tcb {
	uthread_ctx_t * context;
	void * stack;
	int u_state;
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	if (queue_length(ready_queue) > 0){
		// return (struct uthread_tcb *) ready_queue->front->node_data;
		return ready_queue->front->node_data;
	}
	return blocked_queue->front->node_data;
}

void uthread_yield(void)
{
	// yield just puts thread back into queue
	void ** yield_to;
	queue_dequeue(ready_queue, yield_to);
	// queue_enqueue(blocked_queue, uthread_current());
	queue_enqueue(ready_queue, uthread_current());
	uthread_current()->u_state = ready;
	struct uthread_tcb * copy_yield = (struct uthread_tcb *) *yield_to;
	uthread_ctx_switch(uthread_current()->context, copy_yield->context);
	copy_yield->u_state = running;

	// don't forget state checking and changing before ctx switch
	//update state
}

void uthread_exit(void)
{
	// function hasn't actually ended, it's just in a zombie state to be put into queue later
		// clear in while loop for uthread_run
	// get current running thread
	// uthread_ctx_destroy_stack(&uthread_current()->stack);
	// uthread_current()->u_state = exited;
	// update state
	// if (queue_length(ready_queue) > 0){
	//     queue_delete(ready_queue->front->node_data);	
	// }
	// else{
	//     	queue_delete(blocked_queue->front->node_data);	
	// }
}

int uthread_create(uthread_func_t func, void *arg) //done
{
	if (arg == NULL || func == NULL)
		return -1;
	
	// create initial thread
	uthread_ctx_t * new_context;
	void * new_stack;
	new_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * new_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	new_thread->context = new_context;
	new_thread->stack = new_stack;
	new_thread->u_state = ready;
	uthread_ctx_init(new_thread->context, new_thread->stack, func, &arg);

	// add initial thread to queue
	queue_enqueue(ready_queue, &new_thread);

	// uthread_run(&new_thread, func(arg));


	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	ready_queue = (struct queue *) malloc(sizeof (struct queue )); //fix this
	ready_queue = queue_create();
	blocked_queue = (struct queue *) malloc(sizeof (struct queue)); //fix this
	blocked_queue = queue_create();
	//phase 4 - preemption
	if (preempt){
		// preemptive scheduling is enabled
		// preempt_enable();
	}
	// register so-far single execution flow of app as the idle thread

	// create idle thread
	uthread_ctx_t * init_context;
	void * init_stack;
	init_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * idle_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	idle_thread->context = init_context;
	idle_thread->stack = init_stack;
	idle_thread->u_state = running;
	uthread_ctx_init(idle_thread->context, idle_thread->stack, NULL, NULL);

	queue_enqueue(blocked_queue, idle_thread);
	
	// add initial thread
	uthread_create(func, &arg);

	// uthread_current should point to new_thread
	struct uthread_tcb *current;
	current = uthread_current();

	while(current != NULL && queue_length(ready_queue) == 0){
		// run through availible thread
		uthread_yield();
		// after function is done, remove from queue
		queue_delete(ready_queue, current);
		// get another thread from queue? current?
		if (queue_length(ready_queue) == 0){
			current = uthread_current();
			queue_delete(blocked_queue, current);
		}
	}
	// if no more threads, return 0
	queue_destroy(ready_queue);
	queue_destroy(blocked_queue);
	return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	void ** uthread_copy;
	queue_dequeue(ready_queue, uthread_copy);
}