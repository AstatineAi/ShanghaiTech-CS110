#let sol = content => {
  box([*Solution: * #content], stroke: 1pt, inset: 10pt, width: 100%)
}

// Solution Example:
// #let t1 = (
//   [Yousr Ans],
//   [Yousr Ans],
//   [Yousr Ans],
//   [Yousr Ans],
// )
//
// #let t2-3 = sol[
//   Your Ans
// ]
//
// $ 1 + 1 = 2 $ for math equation

#let t1 = (
  [F],
  [F],
  [F],
  [F],
)

#let t2-1 = (
  [32 btyes],
  [256 bytes],
  [2-way set associative],
  [8],
  [4],
  [5, 2, 5],
)

#let t2-2 = (
  [Miss],
  [Miss],
  [Miss],
  [Replacement],
  [Replacement],
  [Miss],
  [Replacement],
  [Hit],
  [Replacement],
  [Replacement],
  [Hit],
  [Hit],
  [Replacement],
  [Hit],
)

#let t2-3 = sol[
  Change the cache associativity to fully associative.
]

#let t3-1 = sol[
  $8% times 35% times 10% = 0.28%$
]

#let t3-2 = sol[
  $
  "AMAT" = 2 "cycle" + 8% times (12 "cycle" + 35% times (44 "cycle" + 10% times (80 "ns")))
  $

  Since the CPU runs at 4GHz, a cycle takes 0.25ns.

  $
  "AMAT" = 0.5 "ns" + 0.08 times (3 "ns" + 0.35 times (11 "ns" + 0.10 times (80 "ns"))) = 1.272 "ns"
  $
]

#let t3-3 = sol[
  $
  "AMAT" = 0.8 "ns" + 0.06 times (3 "ns" + 0.35 times (11 "ns" + 0.10 times (80 "ns"))) = 1.379 "ns"
  $
]

#let t4-1 = sol[
  The size of `struct body` is 16 bytes, which is exactly the size of a cache line.

  $
  "Total accesses" = sum_(i = 0)^(63) 2 times (64 - i - 1) = 4032
  $

  + For outer loop iterations where $i <= 54$
    - Each time $i$ reaches an even number (0, 2, 4, ..., 54), we'll have 1 miss
    - This gives us 28 misses from accessing `body[i]`
    - For the inner loop, we'll have a miss for every other element due to our 2-element-per-block alignment
    - Since i and i+1 loops have the same miss pattern, the total inner loop misses are 972
  + For outer loop iterations where $55 <= i <= 56$
    - Total 7 misses.
    - After this two iterations, all elements from `body[57]` to `body[63]` are in the cache.
  + For outer loop iterations where $i >= 57$
    - Total 16 cache lines.
    - when $i >= 57$, the last 16 elements of `body` are all in the cache and all accesses are hits.
  $
  "Hit Rate" = 1 - (28 + 972 + 7) / 4032 = 3025/4032 = 75.0248%
  $

]

#let t4-2 = sol[
  ```c
  #define tile_size 8
  int check_collision() {
    int count = 0;
    
    for (int ii = 0; ii < 64; ii += tile_size) {
        for (int jj = ii; jj < 64; jj += tile_size) {
            for (int i = ii; i < ii + tile_size && i < 64; i++) {
                for (int j = (i < jj ? jj : i + 1); j < jj + tile_size && j < 64; j++) {
                    if (is_collide(bodies[i], bodies[j])) {
                        count++;
                    }
                }
            }
        }
    }
    return count;
  }
```

Use a smaller tile to make sure that the inner loop elements are always in the cache.

]
