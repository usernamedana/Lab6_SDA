#ifndef QUEUE_SIMPLE_H
#define QUEUE_SIMPLE_H

#include "list_node.h"

typedef struct SimpleQueue {
    ListNode *head;
    ListNode *tail;
    int size;
} SimpleQueue;

void simpleQueueInit(SimpleQueue *q);
void simpleQueueEnqueue(SimpleQueue *q, const WarehouseRecord *r);
int  simpleQueueDequeue(SimpleQueue *q, WarehouseRecord *out);
void simpleQueueTraverse(const SimpleQueue *q);
ListNode *simpleQueueSearchByPosition(const SimpleQueue *q, int position);
ListNode *simpleQueueSearchBySurname(const SimpleQueue *q, const char *surname);
int  simpleQueueDeleteByPosition(SimpleQueue *q, int position);
void simpleQueueFree(SimpleQueue *q);

void simpleQueueRunMenu(SimpleQueue *q);

#endif
