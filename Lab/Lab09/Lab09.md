# Lab09

## Exercise 1

### Scenario 1

> Cache Levels: 1
> Block Size (Bytes): 8
> Number of blocks: 4
> Associativity: 1 (Venus won't let you change this, why?)
> Cache Size (Bytes): 32 (Why?)
> Placement Policy: Direct Mapping
> Block Replacement Policy: LRU
> Enable current selected level of the cache.
>
> Array Size: 128
> Step Size: 8
> Rep Count: 4
> Option: 0

The associativity is 1 because the cache is direct-mapped. In a direct-mapped cache, each block of memory maps to exactly one cache line. This means that for each memory address, there is only one possible location in the cache where it can be stored.

Cache size is 32 bytes because size = number of blocks * block size = 4 * 8 = 32 bytes.

-  What combination of parameters is producing the hit rate you observe? (Hint: Your answer should be the process of your calculation.)

Step size 8. Since the cache is direct-mapped, addresses with 8 bytes difference will map to the same cache line. This means only 1 cache line will be used in this scenario.

Hit rate 0 because every access evicts the same cache line.

- What is our hit rate if we increase Rep Count arbitrarily? Why?

0. All accesses will still map to the same cache line, so the hit rate will remain 0.

- How could we modify our program parameters to maximize our hit rate?

Change the step size to 32.

### Scenario 2

> Cache Levels: 1
> Block Size (Bytes): 16
> Number of blocks: 16
> Associativity: 4
> Cache Size (Bytes): 256
> Placement Policy: N-Way Set Associative
> Block Replacement Policy: LRU
> Enable current selected level of the cache.
>
> Array Size: 256
> Step Size: 2
> Rep Count: 1
> Option: 1

- What combination of parameters is producing the hit rate you observe? (Hint: Your answer should be the process of your calculation.)

1. Total accesses: 256 / 4 = 64 words, step size 2 -> 64 / 2 = 32 operations, option 1 -> each operation invloves 1 read 1 write -> 64 accesses in total
2. Address: 32 bit, [31 : 5] tag, [4 : 3] set index, [2 : 0] offset
3. Every write after read will hit the cache, and there are 32 writes in total
4. the first cache miss will also load the next word into the cache block, so the next read will hit the cache

Therefore, only 16 accesses will miss the cache, then all contents will be in the cache.

Cache hit rate = (total accesses - cache misses) / total accesses = (64 - 16) / 64 = 48 / 64 = 0.75

- What happens to our hit rate as Rep Count goes to infinity? Why?

Since all contents will be in the cache after 16 accesses, the hit rate will be 1.0.

- Suppose we have a program that uses a very large array and during each Rep, we apply a different operator to the elements of our array (e.g. if Rep Count = 1024, we apply 1024 different operations to each of the array elements). How can we restructure our program to achieve a hit rate like that achieved in this scenario? (Assume that the number of operations we apply to each element is very large and that the result for each element can be computed independently of the other elements.) What is this technique called? (Hint)

Switch the inner and outer loops.

Cache blocking.

### Scenario 3

> Cache Levels: 1
> Block Size (Bytes): 16
> Number of blocks: 16
> Associativity: 4
> Cache Size (Bytes): 256
> Placement Policy: N-Way Set Associative
> Block Replacement Policy: Random
> Enable current selected level of the cache.
>
> Array Size: 256
> Step Size: 8
> Rep Count: 2
> Option: 0

- Run the simulation a few times. Every time, set a different seed value (bottom of the cache window). Note that the hit rate is non-deterministic. What is the range of its hit rate? Why is this the case? ("The cache eviction is random" is not a sufficient answer)

1. Total accesses: 256 / 4 = 64 words, step size 8 -> 64 / 8 = 8 operations, option 0 -> each operation invloves 1 read 1 write, 2 rounds -> 16 accesses in total
2. Only the first and the thrid set will be used since the step size is 8

Best case: only one cache miss for each address, no eviction. Hit rate = (16 - 8) / 16 = 0.5
Worst case: always evict the same cache line. Hit rate: 0

- Which Cache parameter can you modify in order to get a constant hit rate? Record the parameter and its value (and be prepared to show your TA a few runs of the simulation). How does this parameter allow us to get a constant hit rate? And explain why the constant hit rate value is that value.

1. Use non-random replacement policy (change to LRU), hit rate 0.5
2. Use direct-mapped cache, hit rate 0.5
3. Larger cache block size, e.g. 64 bytes cache block hit rate 0.75
4. More cache blocks, e.g. 64 blocks hit rate 0.5

## Exercise 2

- Why there is a gap between gb_v and gb_h?

Key differnece:

In `gb_v`

```c
sum += gv.data[i] /gv.sum[ext - deta] * (float)get_pixel(a, x, y + offset)[channel];
```

In `gb_h`

```c
sum += gv.data[i]/gv.sum[ext - deta] * (float)get_pixel(a, x + offset, y)[channel];
```

Therefore, `gb_h` has better locality.

- Why the changed execution order will achieve a better performance even if we do more things(transpose)?

The cost difference between a single memory access and a cache access is several orders of magnitude.

## Exercise 3

```c
struct log_entry{
    int status;
    long reference_time;
    int src_ip;
    char URL[128];
    char bro[128];
}logs[NUM_ENTRIES];
```

Move `status`, `reference_time`, `src_ip` to the first three fields of the struct.

When accessing `status`, `reference_time`, and `src_ip` will be more likely to be in the same cache line (not seperated by 128 bytes arrays), which will reduce cache misses.