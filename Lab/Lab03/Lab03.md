# ShanghaiTech CS110 Spring 2025 Lab 3

## Exercise 1

1. What do the `.data`, `.word`, `.text` directives mean (i.e. what do you use them for)?

Those are **assembly directives**. They are used to tell the assembler how to interpret the following lines of code.

`.data` and `.text` are in the same category, they are used for selection of the section of the code. `.data` is used to declare data, and `.text` is used to declare code.

`.word` is used for emitting a 32-bit word of data, that is, the integer literal following the directive will be stored in memory as a 32-bit word.

2. What basic instruction(s) are the pseudo instructions like `la`, `mv` and `j` converted to by the assembler?

- `la rd, symbol`: load address to regsiter. Because of the limited number of bits of the immediate field in the instruction, the `la` pseudo-instruction is converted to a `lui` instruction (can manipulate the upper 20 bits of a register) followed by an `addi` instruction (can manipulate the lower 12 bits of a register), for position-independent code, the `la` pseudo-instruction is converted to a `auipc` and `addi`.
- `mv rd, rs`: move value. Converted to `addi rd, rs, 0`
- `j offset`: jump. Converted to `jal x0, offset`

3. Run the program to completion. What number did the program output? What does this number represent?

34, the ninth Fibonacci number.

4. At what address is `n` stored in memory? Hint: Look at the contents of the registers.

`0x10000000`

5. Without using the “Edit” tab, have the program calculate the 13th fib number (0-indexed) by manually modifying the value of a register. You may find it helpful to first step through the code. If you prefer to look at decimal values, change the “Display Settings” option at the bottom.

Set `t3` to 13 (`0x0000000d`) after `lw` instruction.

## Exercise 2



## Exercise 3


