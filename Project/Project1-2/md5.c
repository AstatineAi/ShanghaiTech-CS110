#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static const uint32_t K[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

static const uint32_t S[] = {7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 7,
                             12, 17, 22, 5,  9,  14, 20, 5,  9,  14, 20, 5,  9,
                             14, 20, 5,  9,  14, 20, 4,  11, 16, 23, 4,  11, 16,
                             23, 4,  11, 16, 23, 4,  11, 16, 23, 6,  10, 15, 21,
                             6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21};

static const uint32_t initial_buffer[4] = {0x67452301, 0xefcdab89, 0x98badcfe,
                                           0x10325476};

static uint32_t A, B, C, D;

/* Rotate a 32-bit integer left by n bits */
static inline uint32_t rotateLeft(uint32_t x, uint32_t n) {
  return (x << n) | (x >> (32 - n));
}

void processHash(const uint8_t *data) {
  uint32_t M[16];
  for (int i = 0; i < 16; i++) {
    M[i] = (uint32_t)data[i * 4] | ((uint32_t)data[i * 4 + 1] << 8) |
           ((uint32_t)data[i * 4 + 2] << 16) |
           ((uint32_t)data[i * 4 + 3] << 24);
  }

  uint32_t a = A, b = B, c = C, d = D;

  for (int i = 0; i < 64; i++) {
    uint32_t F, g;
    if (i < 16) {
      F = (b & c) | ((~b) & d);
      g = i;
    } else if (i < 32) {
      F = (d & b) | ((~d) & c);
      g = (5 * i + 1) % 16;
    } else if (i < 48) {
      F = b ^ c ^ d;
      g = (3 * i + 5) % 16;
    } else {
      F = c ^ (b | ~d);
      g = (7 * i) % 16;
    }

    F += a + K[i] + M[g];
    a = d;
    d = c;
    c = b;
    b += rotateLeft(F, S[i]);
  }

  A += a;
  B += b;
  C += c;
  D += d;
}

void md5(const uint8_t *s, size_t len, uint8_t digest[16]) {
  /* Initialize the MD5 buffer */
  A = initial_buffer[0];
  B = initial_buffer[1];
  C = initial_buffer[2];
  D = initial_buffer[3];

  size_t full_blocks = len / 64;
  size_t remaining_bytes = len % 64;
  /* 56 = 64 - appended 8 bytes for length */
  size_t padding_length = (remaining_bytes < 56) ? (56 - remaining_bytes)
                                                 : (64 + 56 - remaining_bytes);

  /* Process each full block */
  for (size_t i = 0; i < full_blocks; i++) {
    processHash(s + i * 64);
  }

  /* Final block */
  uint8_t block[64];

  /* Copy the remaining bytes */
  for (size_t i = 0; i < remaining_bytes; i++) {
    block[i] = s[full_blocks * 64 + i];
  }

  /* Append the 1 bit */
  block[remaining_bytes] = 0x80;

  /* Clear the rest of the block */
  for (size_t i = remaining_bytes + 1; i < 64; i++) {
    block[i] = 0;
  }

  uint64_t bits_len = len * 8;
  uint32_t bits_low = (uint32_t)(bits_len);
  uint32_t bits_high = (uint32_t)(bits_len >> 32);

  if (remaining_bytes < 56) {
    /* Appended 8 bytes length is in the current block */
    for (int i = 0; i < 4; i++) {
      block[56 + i] = (bits_low >> (i * 8)) & 0xFF;
      block[60 + i] = (bits_high >> (i * 8)) & 0xFF;
    }
    processHash(block);
  } else {
    /* One more block needed */
    processHash(block);
    /* Prepare the next block */
    for (int i = 0; i < 56; i++) {
      block[i] = 0;
    }
    for (int i = 0; i < 4; i++) {
      block[56 + i] = (bits_low >> (i * 8)) & 0xFF;
      block[60 + i] = (bits_high >> (i * 8)) & 0xFF;
    }
    processHash(block);
  }

  /* Copy result to digest */
  for (int i = 0; i < 4; i++) {
    digest[i] = (A >> (i * 8)) & 0xFF;
    digest[i + 4] = (B >> (i * 8)) & 0xFF;
    digest[i + 8] = (C >> (i * 8)) & 0xFF;
    digest[i + 12] = (D >> (i * 8)) & 0xFF;
  }
}

int main(void) {
  uint8_t result[16];
  const char *s = "114514";
  md5((const uint8_t *)s, strlen(s), result);
  for (int i = 0; i < 16; i++) {
    printf("%02x", result[i]);
  }
  return 0;
}