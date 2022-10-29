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
}

void uthread_yield(void)
{
	// uthread_ctx_switch();
}

void uthread_exit(void)
{
	// uthread_ctx_destroy_stack(void *top_of_stack)
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
		// preemptive scheduling is enabled
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
	queue_enqueue(uthread_queue, *idle_thread);

	// uthread_current should point to new_thread
	struct uthread_tcb *current;
	current = uthread_current();

	while(current){
		// run through availible threads
		if (current) {
			// run thread
		}
		// if no more threads, return 0
		// uthread_yield()
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

