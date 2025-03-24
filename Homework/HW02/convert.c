#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define FP32_EXPOENET 8
#define FP32_MANTISSA 23
#define FP32_SIGN 1
#define FP32_FULL_MANTISSA (((uint64_t)1 << FP32_MANTISSA) - 1)
#define FP32_FULL_EXPONENT (((uint64_t)1 << FP32_EXPOENET) - 1)

typedef union {
  float f;
  struct {
    uint32_t mantissa : FP32_MANTISSA;
    uint32_t exponent : FP32_EXPOENET;
    uint32_t sign : FP32_SIGN;
  } bits;
} fp32;

#define CA25_EXPOENET 32
#define CA25_MANTISSA 31
#define CA25_SIGN 1
#define CA35_FULL_EXPONENT (((uint64_t)1 << CA25_EXPOENET) - 1)

#define MANTISSA_SHIFT 8

typedef union {
  uint64_t f;
  struct {
    uint64_t mantissa : CA25_MANTISSA;
    uint64_t exponent : CA25_EXPOENET;
    uint64_t sign : CA25_SIGN;
  } bits;
} ca25;

typedef enum { CA_NORMAL, CA_SUBNORMAL, CA_ZERO, CA_INFINITE, CA_NAN } Ca25Type;

Ca25Type caclassify(ca25 c) {
  if (c.bits.exponent == 0 && c.bits.mantissa == 0) {
    return CA_ZERO;
  } else if (c.bits.exponent == 0 && c.bits.mantissa != 0) {
    return CA_SUBNORMAL;
  } else if (c.bits.exponent == CA35_FULL_EXPONENT && c.bits.mantissa == 0) {
    return CA_INFINITE;
  } else if (c.bits.exponent == CA35_FULL_EXPONENT && c.bits.mantissa != 0) {
    return CA_NAN;
  } else {
    return CA_NORMAL;
  }
}

void print_ca25(ca25 c) {
  Ca25Type type = caclassify(c);
  printf("ca25 S=%" PRIx64 " E=%08" PRIx64 " ", (uint64_t)c.bits.sign,
         (uint64_t)c.bits.exponent);

  uint8_t mantissa_leading_one =
      (uint8_t)(type == CA_NORMAL || type == CA_INFINITE || type == CA_NAN);

  printf("M=%" PRIx64 ".%08" PRIx32 " ", (uint64_t)mantissa_leading_one,
         (uint32_t)((uint64_t)c.bits.mantissa << 1));

  switch (type) {
  case CA_NORMAL:
    printf("normal\n");
    break;
  case CA_SUBNORMAL:
    printf("subnormal\n");
    break;
  case CA_ZERO:
    printf("zero\n");
    break;
  case CA_INFINITE:
    printf("inf\n");
    break;
  case CA_NAN:
    printf("nan\n");
    break;
  }
}

fp32 add_mantissa_with_overflow(fp32 f) {
  if (f.bits.mantissa == FP32_FULL_MANTISSA) {
    f.bits.mantissa = 0;
    f.bits.exponent += 1;
  } else {
    f.bits.mantissa += 1;
  }
  return f;
}

fp32 ca25_to_fp32(ca25 c) {
  fp32 f;
  f.bits.sign = c.bits.sign;

  Ca25Type type = caclassify(c);

  switch (type) {
  case CA_ZERO:
    f.bits.exponent = 0;
    f.bits.mantissa = 0;
    break;
  case CA_INFINITE:
    f.bits.exponent = FP32_FULL_EXPONENT;
    f.bits.mantissa = 0;
    break;
  case CA_NAN:
    f.bits.exponent = FP32_FULL_EXPONENT;
    f.bits.mantissa = (uint32_t)(c.bits.mantissa >> MANTISSA_SHIFT);
    break;
  case CA_SUBNORMAL:
    f.bits.exponent = 0;
    f.bits.mantissa = 1;
    break;
  case CA_NORMAL: {
    uint64_t mantissa = c.bits.mantissa;
    uint64_t shifted_exponent = c.bits.exponent;
    int64_t unshifted_exponent =
        (int64_t)shifted_exponent - (((int64_t)1 << (CA25_EXPOENET - 1)) - 1);
    if (unshifted_exponent >= 0) {
      if (unshifted_exponent > 127) {
        f.bits.exponent = FP32_FULL_EXPONENT;
        f.bits.mantissa = 0;
      } else {
        f.bits.exponent =
            (uint32_t)unshifted_exponent + (1 << (FP32_EXPOENET - 1)) - 1;
        f.bits.mantissa =
            (uint32_t)(mantissa >> MANTISSA_SHIFT) & FP32_FULL_MANTISSA;
        if (mantissa & ((1 << MANTISSA_SHIFT) - 1)) {
          f = add_mantissa_with_overflow(f);
        }
      }
    } else {
      if (unshifted_exponent < -149) {
        f.bits.exponent = 0;
        f.bits.mantissa = 1;
      } else if (unshifted_exponent < -126) {
        f.bits.exponent = 0;
        uint64_t exp = -(unshifted_exponent + 126);
        uint64_t shift = exp + MANTISSA_SHIFT;
        mantissa = mantissa + ((uint64_t)1 << 31);
        f.bits.mantissa = (uint32_t)(mantissa >> shift);
        if (mantissa & ((1 << shift) - 1)) {
          f = add_mantissa_with_overflow(f);
        }
      } else {
        f.bits.exponent = unshifted_exponent + 127;
        f.bits.mantissa = (uint32_t)(mantissa >> MANTISSA_SHIFT);
        if (mantissa & ((1 << MANTISSA_SHIFT) - 1)) {
          f = add_mantissa_with_overflow(f);
        }
      }
      break;
    }
  }
  }

  return f;
}

void print_fp32(fp32 f) {
  int type = fpclassify(f.f);
  printf("fp32 S=%" PRIx32 " E=%02" PRIx32 " ", (uint32_t)f.bits.sign,
         (uint32_t)f.bits.exponent);

  uint8_t mantissa_leading_one =
      (uint8_t)(type == FP_NORMAL || type == FP_INFINITE || type == FP_NAN);

  printf("M=%" PRIx32 ".%06" PRIx32 " ", (uint32_t)mantissa_leading_one,
         (uint32_t)(f.bits.mantissa << 1));

  switch (type) {
  case FP_NORMAL:
    printf("normal\n");
    break;
  case FP_SUBNORMAL:
    printf("subnormal\n");
    break;
  case FP_ZERO:
    printf("zero\n");
    break;
  case FP_INFINITE:
    printf("inf\n");
    break;
  case FP_NAN:
    printf("nan\n");
    break;
  }
  union {
    float f;
    uint32_t i;
  } u = {.f = f.f};
  printf("%08" PRIx32 "\n", u.i);
}

int main(void) {
  ca25 c;
  while (scanf("%" SCNx64, &c.f) != EOF) {
    print_ca25(c);
    print_fp32(ca25_to_fp32(c));
  }
  return 0;
}
