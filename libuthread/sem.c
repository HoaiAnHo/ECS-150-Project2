#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

#include "uthread.h"

struct semaphore {
	// queue_t sem_queue;
	int lock;
	size_t count;
};

sem_t sem_create(size_t count)
{
	// queue_t sem_queue = queue_create();
	sem_t sem = (struct semaphore *) malloc(sizeof(struct semaphore));
	sem->lock = 0;
	sem->count = count;
	return sem;
}

int sem_destroy(sem_t sem)
{
	// if locks are still in use, return -1
	if (sem == NULL || sem->count != 0 || sem->lock) 
		return -1;
	
	free(sem);
	return 0;
}

int sem_down(sem_t sem)
{
	if (sem == NULL)
		return -1;

	// spinlock_lock(sem->lock);
	sem->lock = 1;
	while (sem->count == 0) {
		/* Block self */
	}

	sem->count -= 1;
	// spinlock_unlock(sem->lock);
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL)
		return -1;

	// spinlock_lock(sem->lock);
	sem->count += 1;

	/* wake up first thread in line, if there are any*/

	// spinlock_unlock(sem->lock);
	sem->lock = 0;
	return 0;
}
