#include <immintrin.h> /* header file for the SSE intrinsics */
#include <assert.h>
#include <stdio.h>

int **a;
int **b;
int **c;


void naive_shift_right(void){
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            c[i][j] = a[i][j] >> b[i][j];
        }
    }
}

void simd_shift_right(void) {
    for (int i = 0; i < 1000; ++i) {
        int j = 0;
        for (; j + 3 < 40; j += 4) {
            __m128i a_vec = _mm_load_si128((__m128i*)&a[i][j]);
            __m128i b_vec = _mm_load_si128((__m128i*)&b[i][j]);
            __m128i result = _mm_srlv_epi32(a_vec, b_vec);
            _mm_storeu_si128((__m128i*)&c[i][j], result);
        }
        for (; j < 40; ++j) {
            c[i][j] = a[i][j] >> b[i][j];
        }
    }
}


int main(void) {
    a = malloc(1000 * sizeof(int *));
    b = malloc(1000 * sizeof(int *));
    c = malloc(1000 * sizeof(int *));
    for (int i = 0; i < 1000; ++i) {
        a[i] = malloc(40 * sizeof(int));
        b[i] = malloc(40 * sizeof(int));
        c[i] = malloc(40 * sizeof(int));
    }

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            a[i][j] = i * 40 + j;
            b[i][j] = i * 11 % 8;
        }
    }

    simd_shift_right();

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            assert(c[i][j] == a[i][j] >> b[i][j]);
        }
    }
    printf("PASS\n");
    return 0;
}
