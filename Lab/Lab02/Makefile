CC=gcc
CFLAGS=-Wpedantic -Wall -Wextra -Wvla -Werror -std=c11 -g -O0

MEMCHECK=valgrind --tool=memcheck --leak-check=full --track-origins=yes

OBJ=queue.o main.o
PRODUCT=lab2

all: $(PRODUCT)

$(PRODUCT): $(OBJ)
	$(CC) -o $(PRODUCT) $(OBJ)

# How this rule knows to compile vector.c and main.c?
# If there is another .c file, will this rule compiles that file?

# old-fashioned way
# %.o: %.c
# $< is a special variable that means the first prerequisite
# Every .o file in OBJ is required,
# therefore this rule will be executed for every .o file in OBJ
# Only adding a new .c file to the directory will not be enough

.c.o:
	$(CC) -c $(CFLAGS) $<

# What if we don't write $(PRODUCT) here?

# If we don't write $(PRODUCT) here, the rule will be executed
# but the product will not be created.
# Therefore, there's nothing to check with memcheck, or
# `make memcheck` will not re-compile the product.

memcheck: $(PRODUCT)
	$(MEMCHECK) ./$(PRODUCT)

clean:
	rm -rf $(OBJ) $(PRODUCT)
