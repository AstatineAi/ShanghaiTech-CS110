# ShanghaiTech CS110 Spring 2025 Lab 4

## Exercies 1

- What causes the errors in `simple_fn`, `naive_mod`, and `mul_arr` that were reported by the Venus CC checker?

`simple_fn` uses unset register `t0`

`naive_mod` uses s0 but it is not saved and restored

`mul_arr` uses s0, s1 but they are not saved and restored

`mul_arr` calls `helper_fn` but `ra`, `t0` is not saved and restored before and after the call

- In RISC-V, we invoke functions by jumping to them and storing the return address in the ra register. Does calling convention apply to the jumps to the `naive_mod_loop` or `naive_mod_end` labels?

`naive_mod_loop` and `naive_mod_end` do not jump to a function, so calling convention does not apply to them.

- No, they are simple jumps for loop control using `j` and B-type instructions.
  
`mul_arr` calls `helper_fn`, so it needs to store `ra` in the prologue and restore it in the epilogue. Other functions do not call any other functions, so they do not need to store `ra`.

## Exercise 2

```c
int gcd(int a, int b) { return b == 0 ? a : gcd(b, a % b); }
```

Since the function above is tail recursive, we can convert it to an iterative version and just forget the calling convention.
