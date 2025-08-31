CC := gcc
CFLAGS := -Wall -O0 -fopenmp

SRCS := $(filter-out chacha.c,$(wildcard *.c))
OBJS := $(SRCS:%.c=%.o)

.PHONY: all clean

all:: $(OBJS) extract_lib

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

extract_lib:
	-ar x libp3lib.a
	
clean::
	rm -f *.o