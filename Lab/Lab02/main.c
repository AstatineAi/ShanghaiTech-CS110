#include "queue.h"
#include <stdio.h>

int main(void) {
  Queue *queue = queue_create();

  if (queue == NULL) {
    printf("Memory allocation failed\n");
    return 1;
  }

  for (int i = 0; i < 100; ++i) {
    QueueResult result = push(queue, i * 10);
    switch (result) {
    case SUCCESS:
      break;
    case ALLOC_FAIL:
      printf("Memory allocation failed\n");
      return 1;
    case NULL_QUEUE:
      printf("Queue is NULL\n");
      return 1;
    }

    Element element = back(queue);

    switch (element.result) {
    case NOTHING:
      printf("Queue is empty\n");
      return 1;
    case JUST:
      printf("%f\n", element.value);
      break;
    }
  }
  queue_free(queue);
  return 0;
}