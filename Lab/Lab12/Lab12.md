# Lab 12

## Part 1: Vector shifting

> Explain the differences between the loading intrinsics.

- `_mm_load_si128`: Load aligned 128-bit integer vector from memory.
- `_mm_loadu_si128`: Load unaligned 128-bit integer vector from memory.
- `_mm_load_pd`: Load two double-precision floating-point values from memory.
- `_mm_load1_pd`: Load a single double-precision floating-point value and replicate it to both elements of a 128-bit vector.

> Identify the correct header files for each intrinsic.

- `__m128i _mm_abs_epi16 (__m128i a)`: `tmmintrin.h`
- `__m128i _mm_add_epi32 (__m128i a, __m128i b)`: `emmintrin.h`
- `__m128 _mm_ceil_ps (__m128 a)`: `smmintrin.h`
- `_mm_load1_pd`: `emmintrin.h`

## Part 4: Compiler optimization

> Explain why -O3 makes the program much faster.

Autovectorization, loop unwinding, register allocation, dead code elimination, instruction reordering...

Too many.
