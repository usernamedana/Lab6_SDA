#ifndef QUEUE_FILE_OPS_H
#define QUEUE_FILE_OPS_H

#include "list_node.h"

void queueRegisterToFile(const ListNode *head, const char *label);
int  queueLoadFromFile(ListNode **head, ListNode **tail, const char *label);
void queueDisplayFileFromMenu(void);
void queueDeleteFileFromMenu(void);

#endif
