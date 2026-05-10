#ifndef QUEUE_CIRCULAR_H
#define QUEUE_CIRCULAR_H

#include "list_node.h"

typedef struct CircularQueue {
    ListNode *tail;   /* tail->next == head; empty if NULL */
    int size;
} CircularQueue;

void circularQueueInit(CircularQueue *q);
void circularQueueEnqueue(CircularQueue *q, const WarehouseRecord *r);
int  circularQueueDequeue(CircularQueue *q, WarehouseRecord *out);
void circularQueueTraverse(const CircularQueue *q);
ListNode *circularQueueSearchByPosition(const CircularQueue *q, int position);
ListNode *circularQueueSearchBySurname(const CircularQueue *q, const char *surname);
int  circularQueueDeleteByPosition(CircularQueue *q, int position);
void circularQueueFree(CircularQueue *q);

void circularQueueRunMenu(CircularQueue *q);

#endif
