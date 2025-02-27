#ifndef LAB_2_QUEUE_H
#define LAB_2_QUEUE_H

#define QUEUE_INITIAL_CAPACITY 10

typedef struct {
  double *data;
  int size;
  int capacity;
} Queue;

enum queue_result {
  SUCCESS,
  ALLOC_FAIL,
};

typedef enum queue_result queue_result;

Queue *queue_create(void);
queue_result push(Queue *queue, double element);
double back(Queue *queue);
void queue_free(Queue *queue);

#endif // LAB_2_QUEUE_H