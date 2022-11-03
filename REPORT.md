# ECS-150-Project2

# Phase 1: Queue API
We implemented a linked list as the data structure to handle our queue. We
created a struct called queue to handle the head and tail pointers as well as
the length of the queue. Another struct called node was created to populate the
aforementioned queue with the given data. By rearranging and/or reassigning the
values of the pointers to a node's previous and next nodes, we
implemented queue_create, queue_destroy, queue_enqueue, queue_dequeue, and
queue_delete as functions of the queue.

To test our code, we ran unit tests through the queue_tester.c file. We
tried to create a test case for each function we implemented along with
some little edge cases we noticed.

# Phase 2: Uthread API
The uthread API uitlized our linked list queue from phase 1, plus the context
functions from context.c that were given to us. We primarily worked in the
uthread.c file for this, and added to the Makefile accordingly.

The uthread TCB structure has 3 main members: 2 of which were initialized by the
context functions and 1 of which is more of a label to keep track of their
current status. The first member of the struct is a context type provided by
context.c and private.h, and how that context is modified is dependent on the
context switch functions. The second member is a pointer to an allocated stack
(and that allocation is dependent on a context.c function, created in either
uthread_run() or uthread_create()). The third member is an integer value that
related to an enum that we created, that informs what state the thread is in.

Through trial and error, our running implementation relies on 3 globalqueues 
for the scheduling process: the running queue, the ready queue, and the exited
queue. After we create the idle thread (which we don't utilize uthread_create()
for to avoid using other uthread functions before the next thread), we first 
enqueue it to the running queue (which will usually only contain at most one 
thread).  New threads are added to the ready queue through uthread_create(), 
which are then to be cycled through the ready queue and running queue whenever 
uthread_yield() is called. Once the idle and initial threads are created, we
then enter an infinite loop that only calls uthread_yield(). This is the main 
function of the idle thread, so that it yields to the initial and subsquently
created threads if it's pushed to the running queue. There is also an "if" 
statement that will check before the idle thread yields if it's the last thread
left, in which case it will break the while loop. We will then remove the last
thread, free all the allocated spaces from the queues and other pointers, and
end the run.

The uthread_current() function utilized the dequeue function from queue through
different "if" conditions to dequeue the current thread. If there's a currently 
running thread, then we dequeue and return that thread. If not but there's still
some ready threads left, then we dequeue from the ready queue instead.

The implementations for uthread_yield() and uthread_exit() are similar in their
construction, but work with different queues and level of finality. What happens
for both at the start is dequeuing running queue with uthread_current(). For 
yield, that thread is enqueued to the ready queue, while in exit, that thread is
moved to the blocked queue. The ready queue is then dequeued again with the same
uthread_current() function, and is then enqueued to the currently empty running
queue. We then switch the contexts using the given context switch function from
context.c's file. If there are no more threads left in ready queue at this point
(and none in blocked queue if we're working with semaphores), that means every 
thread besides the idle thread has been moved to the exited queue. We can then 
break the idle thread out of the infinite loop in uthread_run() by assigning a 
value to the global variable that keeps track of whether the idle thread is the 
last one left (dubbed "only_idle"). For most of the run, it'll be set to 0, but 
uthread_exit() will do the check after its main work through an "if" statement.
It will set "only_idle" to 1 if the "if" statement is accessed.

# Phase 3: Semaphore API
The skeleton code for this phase was first provided by Porquet in the slides.
This utilizes lock implementations in order to prevent starvation of resources.
Most of the work for this was done in sem.c, but we also went back to uthread.c
to modify the block functions for this phase.

(Not actually implemented but here's an idea: if a thread is considered to have
the possibility of starving the process due to the resources they'd hoard,
they're placed in the blocked queue with the blocked status. The blocked queue
is checked periodically to see if the front thread can come back to the running
queue as they're no longer a threat to the safe state. This would be implemented
through the uthread_block and uthread_unblock functions in uthread.c).

# Phase 4: Preemption
There is an "if" statement in uthread_run() that checks whether or not preempt
is true or false. If true, then we use a function from preempt.c to activate 
all the other functions from that file.

Out of all the phases, we had the least amount of time to work on this, as we 
prioritized the quality of the queue and uthread implementations.

# Makefile
For each phase, we included the compilation lines for the associated files
accordingly. We created libuthread.a accordingly at the start. We also tried
including the tester files in the make so that those files could also access 
"undefined" functions like in the context.c file.

# Project Issues
Before settling on a linked list implementation for queue.c, the team considered
using an array (like a circular array), but the fixed length nature of an array 
in C made the prospect difficult. Once we got the "OK" to create more structs 
beyond the ones given to us in the skeleton code, we were good to go on the 
linked lists.

Some initial versions of the uthread.c code ran into trouble whenever we tried 
to access a TCB in the queue without dequeueing, as our thread queues model
considered the front thread of the ready queue as "running". So at some point, we 
copied over the entire code of queue.c to access those directly from the queue.
However, we realized that dequeuing through uthread_current() to access a current
TCB and using running queue to hold the current running thread could fix our 
issues without having to copy over the queue.c code, so that's what we went with.

Another previous issue in uthread.c was the gcc compiler complaining about the 
context variables being used "unitialized" whenever we used the context_init 
function. We got around this by initializing the context variable as a normal 
variable, then inputting that context as a pointer parameter with & in the 
context_init function.
