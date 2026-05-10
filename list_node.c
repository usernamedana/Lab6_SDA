#include "list_node.h"
#include <stdlib.h>
#include <string.h>

ListNode *createNode(const WarehouseRecord *r) {
    ListNode *n = (ListNode *)malloc(sizeof(ListNode));
    if (!n) return NULL;
    memcpy(&n->data, r, sizeof(WarehouseRecord));
    n->prev = NULL;
    n->next = NULL;
    return n;
}

void freeNode(ListNode *node) {
    free(node);
}
