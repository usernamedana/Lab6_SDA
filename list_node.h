#ifndef LIST_NODE_H
#define LIST_NODE_H

#include "warehouse.h"

typedef struct ListNode {
    WarehouseRecord  data;
    struct ListNode *prev;
    struct ListNode *next;
} ListNode;

ListNode *createNode(const WarehouseRecord *r);
void      freeNode(ListNode *node);

#endif
