# ECS-150-Project2

# Phase 1: Queue API
We implemented a linked list as the data structure to handle our queue. We
created a struct called queue to handle the head and tail pointers as well as
the length of the queue. Another struct called node was created to populate the
aforementioned queue with the given data. By rearranging and/or reassigning the
values of the pointers to a node's previous and next nodes, we
implemented queue_create, queue_destroy, queue_enqueue, queue_dequeue, and
queue_delete as functions of the queue.
