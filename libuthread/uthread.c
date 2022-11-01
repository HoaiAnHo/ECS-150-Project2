#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

queue_t ready_queue; 
queue_t blocked_queue; //push running thread to this queue
//front of queue will always be running

enum state {
	running = 0,
	ready = 1,
	blocked = 2,
	exited = 3
};

struct uthread_tcb {
	uthread_ctx_t context;
	void * stack;
	int u_state;
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return (struct uthread_tcb *) ready_queue->front->node_data;
}

void uthread_yield(void)
{
	struct uthread_tcb *yield_to;
	queue_dequeue(ready_queue, yield_to);
	queue_enqueue(blocked_queue, uthread_current());
	uthread_ctx_switch(uthread_current(), &yield_to);
	// don't forget state checking and changing before ctx switch
	//update state
}

void uthread_exit(void)
{
	// uthread_ctx_destroy_stack(void *top_of_stack)
	//update state
}

int uthread_create(uthread_func_t func, void *arg) //done
{
	if (arg == NULL || func == NULL)
		return -1;
	
	// create initial thread
	uthread_ctx_t new_context;
	void * new_stack;
	new_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * new_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	new_thread->context = new_context;
	new_thread->stack = new_stack;
	new_thread->u_state = ready;
	uthread_ctx_init(&new_context, &new_stack, func, &arg);

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
	uthread_ctx_t init_context;
	void * init_stack;
	init_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * idle_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	idle_thread->context = init_context;
	idle_thread->stack = init_stack;
	idle_thread->u_state = running;
	//uthread_ctx_init(idle_thread, idle_thread->stack, func, arg);
	
	// add initial thread
	uthread_create(func, &arg);

	// uthread_current should point to new_thread
	struct uthread_tcb *current;
	current = uthread_current();

	while(current != NULL){
		// run through availible thread
		uthread_yield();
		// after function is done, remove from queue
		queue_delete(ready_queue, current);
		// get another thread from queue? current?
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
}
