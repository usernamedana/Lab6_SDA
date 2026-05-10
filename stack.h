#ifndef STACK_H
#define STACK_H

#include "list_node.h"

typedef struct Stack {
    ListNode *top;
    int size;
} Stack;

void stackInit(Stack *s);
void stackPush(Stack *s, const WarehouseRecord *r);
int  stackPop(Stack *s, WarehouseRecord *out);
void stackTraverse(const Stack *s);
ListNode *stackSearchByPosition(const Stack *s, int position);
ListNode *stackSearchBySurname(const Stack *s, const char *surname);
int  stackDeleteByPosition(Stack *s, int position);
void stackFree(Stack *s);

void stackRunMenu(Stack *s);

#endif
