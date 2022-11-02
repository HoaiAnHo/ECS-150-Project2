#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
	//queue_t sem_queue;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	//queue_t sem_queue = queue_create();
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
}
