#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_simple_create_and_destroy()
{
	int data = 4;
	queue_t q;
	fprintf(stderr, "*** TEST create_and_destroy ***\n");

	q = queue_create();

	TEST_ASSERT(!queue_destroy(q));
}

void test_queue_delete_end()
{
	int data = 4, data_two = 5;
	queue_t q;
	fprintf(stderr, "*** TEST queue_delete end val***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_two);
	queue_delete(q, &data_two);

	TEST_ASSERT(queue_length(q) == 1);
}

void test_queue_delete_middle()
{
	int data = 4, data_two = 5, data_three = 6;
	queue_t q;
	fprintf(stderr, "*** TEST queue_delete middle val***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_two);
	queue_enqueue(q, &data_three);
	queue_delete(q, &data_two);

	TEST_ASSERT(queue_length(q) == 2);
}

void test_queue_delete_front() //edge case of only 2 vals, need to reset q->back to NULL
{
	int data = 4, data_two = 5;
	queue_t q;
	fprintf(stderr, "*** TEST queue_delete front val***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_enqueue(q, &data_two);
	queue_delete(q, &data);

	TEST_ASSERT(queue_length(q) == 1); //currently has a seg fault
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_simple_create_and_destroy();
	test_queue_delete_end();
	test_queue_delete_middle();
	test_queue_delete_front();

	return 0;
}
