# CS110 Lab 2 Exercise 3: Catch those bugs!

1. How do you pass command line arguments to a program when using gdb?

Add arguments after `run` command.

2. How do you set a breakpoint which only occurs when a set of conditions is true (e.g. when certain variables are a certain value)?

Use `break` command with condition: `break <breakpoint> if <condition>`

3. How do you execute the next line of C code in the program after stopping at a breakpoint?

Use command `next` / `n`

4. If the next line of code is a function call, you'll execute the whole function call at once if you use your answer to #3. (If not, consider a different command for #3!) How do you tell GDB that you want to debug the code inside the function instead? (If you changed your answer to #3, then that answer is most likely now applicable here.)

Use command `step` / `s` to step into the function.

5. How do you resume the program after stopping at a breakpoint?

Use command `continue` / `c`

6. How can you see the value of a variable (or even an expression like 1+2) in gdb?

Use command `print <variable>` / `p <variable>`

7. How do you configure gdb so it prints the value of a variable after every step?

Use command `display <variable>`

8. How do you print a list of all variables and their values in the current function?

Use command `info locals` and `info args`

9. How do you exit out of gdb?

Use command `quit` / `q`
