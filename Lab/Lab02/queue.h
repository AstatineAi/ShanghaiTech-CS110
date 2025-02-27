#ifndef LAB_2_QUEUE_H
#define LAB_2_QUEUE_H

#define QUEUE_INITIAL_CAPACITY 10

typedef struct {
  double *data;
  int size;
  int capacity;
} Queue;

typedef enum {
  SUCCESS,
  ALLOC_FAIL,
  NULL_QUEUE
} QueueResult;

typedef enum {
  NOTHING,
  JUST
} Maybe;

typedef struct {
  Maybe result;
  double value;
} Element;

Queue *queue_create(void);
QueueResult push(Queue *queue, double element);
Element back(Queue *queue);
void queue_free(Queue *queue);

#endif // LAB_2_QUEUE_H