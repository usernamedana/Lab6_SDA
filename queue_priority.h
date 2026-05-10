#ifndef QUEUE_PRIORITY_H
#define QUEUE_PRIORITY_H

#include "list_node.h"

typedef struct PriorityQueue {
    ListNode *head;
    ListNode *tail;
    int size;
} PriorityQueue;

void priorityQueueInit(PriorityQueue *q);
void priorityQueueEnqueue(PriorityQueue *q, const WarehouseRecord *r);
int  priorityQueueDequeue(PriorityQueue *q, WarehouseRecord *out);
void priorityQueueTraverse(const PriorityQueue *q);
ListNode *priorityQueueSearchByPosition(const PriorityQueue *q, int position);
ListNode *priorityQueueSearchBySurname(const PriorityQueue *q, const char *surname);
int  priorityQueueDeleteByPosition(PriorityQueue *q, int position);
void priorityQueueFree(PriorityQueue *q);

void priorityQueueRunMenu(PriorityQueue *q);

#endif
