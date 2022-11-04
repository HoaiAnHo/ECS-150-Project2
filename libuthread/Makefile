# # Default rule
# all: VIFiles

# VIFiles: libuthread.a
# 	gcc -o VIFiles $^

# # Don't print the commands unless explicitly requested with `make V=1`
# ifneq ($(V),1)
# Q = @
# V = 0
# endif

# # Define compilation toolchain
# CC	= gcc

# # General gcc options
# CFLAGS	:= -Wall -Wextra -Werror
# CFLAGS	+= -pipe
# ## Debug flag
# ifneq ($(D),1)
# CFLAGS	+= -O2
# else
# CFLAGS	+= -g
# endif

# # Doesn't run the linker
# CFLAGS += -c

# # Generic rule for compiling objects
# %.o: %.c
# 	@echo "CC	$@"
# 	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

# # Cleaning rule
# clean: FORCE
# 	@echo "CLEAN	$(CUR_PWD)"
# 	$(Q)$(MAKE) V=$(V) D=$(D) -C $(UTHREADPATH) clean
# 	$(Q)rm -rf $(objs) $(deps) $(programs)

# # Keep object files around
# .PRECIOUS: %.o
# .PHONY: FORCE
# FORCE:


# # Target library
lib    := libuthread.a
CC     := gcc
CFLAGS := -Wall -Wextra -Werror

all: $(lib)

## TODO: Phase 1
$(lib): queue.o uthread.o
	@ar rcs $(lib) queue.o uthread.o

context.o: context.c private.h
	$(CC) $(CFLAGS) -c -o $@ context.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c -o queue.o queue.c

uthread.o: uthread.c uthread.h
	$(CC) $(CFLAGS) -c -o uthread.o uthread.c

sem.o: sem.c sem.h
	$(CC) $(CFLAGS) -c -o $@ sem.c

clean:
	rm -f $(lib) queue.o


