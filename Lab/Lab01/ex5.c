#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define print_info(name) printf("Size of %s: %zu\n", #name, sizeof(name))

int main(void) {
  print_info(char);
  print_info(short);
  print_info(short int);
  print_info(int);
  print_info(long int);
  print_info(unsigned int);
  print_info(void *);
  print_info(size_t);
  print_info(float);
  print_info(double);
  print_info(int8_t);
  print_info(int16_t);
  print_info(int32_t);
  print_info(int64_t);
  print_info(time_t);
  print_info(clock_t);
  print_info(struct tm);
  print_info(NULL);
  print_info(struct {
    int a;
    short b;
  });
//   printf("Reason of sizeof(struct {...}) is not equal to sum of sizeof(struct "
//          "members): alignment %zu\n",
//          _Alignof(struct {
//            int a;
//            short b;
//          }));
  return 0;
}