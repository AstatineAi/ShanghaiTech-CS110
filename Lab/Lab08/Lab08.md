# Lab08

## Exercise 1

### Shift and Add

```c
int mul(int a, int b) {
    int p_prod = 0;
    while (b) {
        if (b & 1)
            p_prod = p_prod + a;
        // else p_prod = p_prod + 0;
        a = a << 1; // left shift multiplicand
        b = b >> 1;
    }
    return p_prod;
}
```

### Highest Clock Frequency

t_comp in single shift_and_add component:

current_sum -> partial_product: 45ns
multiplicand -> partial_product: 65ns
multiplicand -> shifted_multiplicand: 5ns

The critical path in the circuit is:

multiplcand -> partial_product1: 65ns
partial_product1 -> partial_product2: 45ns
partial_product2 -> partial_product3: 45ns
partial_product3 -> partial_product4 (result): 45ns

t_comp = 65ns + 45ns + 45ns + 45ns = 200ns

min clock period >= t_comp + t_setup + t_clocktoq = 200 + 10 + 10 = 220ns

t_clock = 1 / f_clock = 1 / 220ns = 4.545MHz

