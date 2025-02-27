#include "queue.h"
#include <stdlib.h>

Queue *queue_create(void) {
  Queue *queue = malloc(sizeof(Queue));
  queue->size = 0;
  queue->capacity = QUEUE_INITIAL_CAPACITY;
  queue->data = malloc(sizeof(double) * queue->capacity);
  return queue;
}

queue_result push(Queue *queue, double element) {

  if (queue->size == queue->capacity) {
    int capacity = queue->capacity * 2;

    double *new_data = realloc(queue->data, sizeof(double) * capacity);
    if (new_data == NULL) {
      return ALLOC_FAIL;
    } else {
      queue->data = new_data;
    }

    queue->capacity = capacity;
  }

  int insert_index = queue->size % queue->capacity;
  queue->data[insert_index] = element;
  queue->size++;

  return SUCCESS;
}

double back(Queue *queue) { return queue->data[queue->size - 1]; }

void queue_free(Queue *queue) {

  free(queue->data);
  free(queue);
}
