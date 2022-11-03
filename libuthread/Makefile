# Target library
lib    := libuthread.a
CC     := gcc
CFLAGS := -Wall -Wextra -Werror

all: $(lib)

## TODO: Phase 1
$(lib): queue.o uthread.o
	@ar rcs $(lib) queue.o uthread.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o queue.o queue.c

uthread.o: uthread.c uthread.h
	$(CC) $(CFLAGS) -c -o uthread.o uthread.c

uthread_hello.o: queue.c uthread.c context.c
	$(CC) $(CFLAGS) -c -o $@

clean:
	rm -f $(lib) queue.o
