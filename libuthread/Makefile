# Target library
lib    := libuthread.a
CC     := gcc
CFLAGS := -Wall -Wextra -Werror

all: $(lib)

## TODO: Phase 1
$(lib): queue.o
	ar rcs $(lib) queue.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o queue.o queue.c

clean:
	rm -f $(lib) queue.o