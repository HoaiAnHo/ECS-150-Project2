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
#include "context.c" //test
#include "preempt.c" //test

queue_t running_queue;
queue_t ready_queue; 
queue_t exited_queue;
queue_t blocked_queue;

int only_idle = 0;

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
	void * data;
	// if there's something running at the moment, dequeue it
	if (queue_length(running_queue) > 0){
		queue_dequeue(running_queue, (void **)&data);
		return data;
	}
	// if nothing's in the running queue and there's something in the ready queue, dequeue the ready thread
	else if (queue_length(ready_queue) > 0){
		// return (struct uthread_tcb *) ready_queue->front->node_data;
		// return ready_queue->front->node_data;
		queue_dequeue(ready_queue, (void **)&data);
		return data;
	}
	else if (queue_length(running_queue) == 0 && queue_length(running_queue) == 0){
		queue_dequeue(exited_queue, (void **)&data);
		return data;
	}

	return NULL;
}

// yield just puts thread back into ready queue
void uthread_yield(void)
{
	// dequeue current running thread into ready
	struct uthread_tcb *yielding = uthread_current();
	queue_enqueue(ready_queue, yielding);
	yielding->u_state = ready;

	// dequeue next ready thread into running queue
	struct uthread_tcb *yield_to = uthread_current();
	yield_to->u_state = running;
	queue_enqueue(running_queue, yield_to);
	// struct uthread_tcb * copy_yield = (struct uthread_tcb *) *yield_to;
	uthread_ctx_switch(yielding->context, yield_to->context);
	
	uthread_exit();
}

void uthread_exit(void)
{
	// function hasn't actually ended, it's just in a zombie state to be put into queue later

	// get current running thread and move to exited
	struct uthread_tcb * exit_uthread = uthread_current();
	// uthread_ctx_destroy_stack(exit_uthread->stack);
	queue_enqueue(exited_queue, exit_uthread);
	exit_uthread->u_state = exited;
	
	// put the next ready thread into the running queue
	struct uthread_tcb * next_uthread = uthread_current();
	queue_enqueue(running_queue, next_uthread);
	uthread_ctx_switch(exit_uthread->context, next_uthread->context);

	// if only the idle thread is left in running, change global var
	if (queue_length(ready_queue) == 0) {
		only_idle = 1;
		exit_uthread = uthread_current();
		queue_enqueue(exited_queue, exit_uthread);
	}
}

int uthread_create(uthread_func_t func, void *arg) //done
{
	if (arg == NULL || func == NULL)
		return -1;
	
	// create new thread
	uthread_ctx_t new_context;
	void * new_stack;
	new_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * new_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	uthread_ctx_init(&new_context, new_stack, func, arg);
	new_thread->context = &new_context;
	new_thread->stack = new_stack;
	new_thread->u_state = ready;

	// add new thread to ready queue
	queue_enqueue(ready_queue, new_thread);
	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	// create running, ready, blocked, and exited queues
	running_queue = queue_create();
	ready_queue = queue_create();
	blocked_queue = queue_create();
	exited_queue = queue_create();


	//phase 4 - preemption
	if (preempt){
		// preemptive scheduling is enabled
		// preempt_enable();
	}

	// create idle thread
	uthread_ctx_t init_context;
	void * init_stack;
	init_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb * idle_thread = (struct uthread_tcb *) malloc(sizeof (struct uthread_tcb));
	uthread_ctx_init(&init_context, init_stack, NULL, NULL);
	idle_thread->context = &init_context;
	idle_thread->stack = init_stack;
	idle_thread->u_state = running;

	// enqueue idle thread as the running thread
	queue_enqueue(running_queue, idle_thread);
	
	// create initial thread (which is added to the ready queue)
	uthread_create(func, &arg);

	// start idle thread (which only yields until all other threads are exited)
	while(1){
		if (only_idle == 1) break;
		uthread_yield();
	}

	// if no more threads, return 0
	queue_destroy(running_queue);
	queue_destroy(ready_queue);
	queue_destroy(blocked_queue);
	// run a loop to delete everything in blocked_queue
	struct uthread_tcb *last_uthread;
	while (queue_length(exited_queue) > 0){
		last_uthread = uthread_current();
		uthread_ctx_destroy_stack((void *)&last_uthread->context);
	}
	queue_destroy(exited_queue);
	return 0;
}

void uthread_block(void)
{
	// dequeue current running thread into the blocked queue
	struct uthread_tcb *block_uthread = uthread_current();
	block_uthread->u_state = blocked;
	queue_enqueue(blocked_queue, block_uthread);

	// dequeue next ready thread into running queue
	struct uthread_tcb *yield_to = uthread_current();
	yield_to->u_state = running;
	queue_enqueue(running_queue, yield_to);
	uthread_ctx_switch(block_uthread->context, yield_to->context);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	// dequeue the front thread of blocked_queue
	struct uthread_tcb * awaken_uthread;
	queue_dequeue(blocked_queue, (void **)&awaken_uthread);

	// loop until you find the thread in the blocked queue that matches uthread
	while (1){
		// if match is found, enqueue that thread into the ready queue
		if (&awaken_uthread == &uthread){
			queue_enqueue(ready_queue, awaken_uthread);
			awaken_uthread->u_state = ready;
			break;
		}
		// else, enqueue thread back into blocked queue and grab the next thread
		else{
			queue_enqueue(blocked_queue, awaken_uthread);
			queue_dequeue(blocked_queue, (void **)&awaken_uthread);
		}
	}
}