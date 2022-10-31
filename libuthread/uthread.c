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

queue_t ready_queue = queue_create();
queue_t running_queue = queue_create();

enum state {
	running = 0,
	ready = 1,
	exited = 2
}

struct uthread_tcb {
	uthread_ctx_t context;
	void stack;
	int u_state;
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	// return either front of ready or front of running
	// return running_queue->front;
	// why won't the queue point to any of its members???
}

void uthread_yield(void)
{
	// get running thread and front of ready queue
	// uthread_ctx_switch(pointer of running context, pointer of front context);
	// switch threads from running to ready and vice versa
}

void uthread_exit(void)
{
	// first get thread
	// remove thread from queue
	// uthread_ctx_destroy_stack(void *top_of_stack)
	// maybe destroy context?
	// queue_delete(ready_queue, thread)
}

int uthread_create(uthread_func_t func, void *arg)
{
	uthread_ctx_t new_context;
	void new_stack;
	*new_stack = uthread_ctx_alloc_stack();
	uthread_ctx_init(&new_context, &new_stack, func, &arg);
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if (preempt){
		// preemptive scheduling is enabled (phase 4)
		// preempt_enable();
	}
	// register so-far single execution flow of app as the idle thread

	// create initial thread
	uthread_ctx_t init_context;
	void init_stack;
	*init_stack = uthread_ctx_alloc_stack();
	struct uthread_tcb idle_thread = {init_context, init_stack, ready};
	uthread_ctx_init(idle_thread->context, idle_thread->stack, func, *arg);

	// add initial thread to queue
	queue_enqueue(ready_queue, *idle_thread);
	// question, where do we get the other threads???
	// create threads??? uthread_create(func, arg)
	// is this file assuming we're only running one thread???

	// uthread_current should point to new_thread
	struct uthread_tcb *current;
	current = uthread_current();

	while(current){
		// run through availible threads
		if (current) {
			// run thread
		}
		// uthread_yield()
		// if no more threads, return 0
			// destroy ready_queue and running_queue
		return 0;
	}
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

