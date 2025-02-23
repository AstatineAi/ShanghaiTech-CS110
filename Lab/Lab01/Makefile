# Compiler settings
CC = gcc
CFLAGS = -Wpedantic -Wall -Wextra -Wvla -Werror -std=c11
CFLAGS32 = $(CFLAGS) -m32

all:
	@echo "Please specify the exercise number, e.g., make ex1"

# Exercise 1: show my computer is "Real Linux"
ex1:
	@fastfetch
	@uname -a

# Exercise 2: show gradescope account and HW1
ex2:
	@xdg-open https://www.gradescope.com/courses/985840/assignments/5797285/submissions/309536217
	@xdg-open https://www.gradescope.com/account/edit

# Exercise 3: Linux command
ex3:
	@echo -e "cd: change directory\n\
	ls: list directory contents\n\
	pwd: print working directory\n\
	mkdir: make directory\n\
	rm: remove files or directories\n\
	cp: copy files or directories\n\
	mv: move files or directories\n\
	cat: concatenate and display files\n\
	man: manual\n\
	touch: create file"

# Exercise 4: Binary Alphabet
ex4:
	python3 ex4.py

# Exercise 5: Sizes
EX5SRC = ex5.c

size64.o: $(EX5SRC)
	$(CC) $(CFLAGS) -o size64.o ex5.c

size32.o: $(EX5SRC)
	$(CC) $(CFLAGS32) -o size32.o ex5.c

ex5: size64.o size32.o
	@echo -e "\nResult of 64-bit arch:"
	@./size64.o
	@echo -e "\nResult of 64-bit arch:"
	@./size32.o

clean:
	rm -f size64.o size32.o

.PHONY: clean all ex1 ex2 ex3 ex4 ex5