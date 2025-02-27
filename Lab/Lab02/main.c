#include "queue.h"
#include <stdio.h>

int main(void) {
  Queue *queue = queue_create();
  for (int i = 0; i < 100; ++i) {
    queue_result result = push(queue, i * 10);
    switch (result) {
    case SUCCESS:
      break;
    case ALLOC_FAIL:
      printf("Memory allocation failed\n");
      return 1;
    }
    printf("%f\n", back(queue));
  }
  queue_free(queue);
  return 0;
}