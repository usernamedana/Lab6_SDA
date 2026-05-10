#ifndef QUEUE_DEQUE_H
#define QUEUE_DEQUE_H

#include "list_node.h"

typedef struct Deque {
    ListNode *head;
    ListNode *tail;
    int size;
} Deque;

void dequeInit(Deque *d);
void dequePushFront(Deque *d, const WarehouseRecord *r);
void dequePushBack (Deque *d, const WarehouseRecord *r);
int  dequePopFront (Deque *d, WarehouseRecord *out);
int  dequePopBack  (Deque *d, WarehouseRecord *out);
void dequeTraverse(const Deque *d);
ListNode *dequeSearchByPosition(const Deque *d, int position);
ListNode *dequeSearchBySurname(const Deque *d, const char *surname);
int  dequeDeleteByPosition(Deque *d, int position);
void dequeFree(Deque *d);

void dequeRunMenu(Deque *d);

#endif
