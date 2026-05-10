#include "queue_priority.h"
#include "queue_file_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void clearBuf(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static int readMenu(void) {
    int v;
    if (scanf("%d", &v) != 1) { clearBuf(); return -1; }
    clearBuf();
    return v;
}

void priorityQueueInit(PriorityQueue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

/* Insert sorted: highest unitPrice at head. */
void priorityQueueEnqueue(PriorityQueue *q, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }

    if (!q->head) {
        q->head = q->tail = n;
        q->size++;
        return;
    }
    ListNode *cur = q->head;
    while (cur && cur->data.unitPrice >= r->unitPrice) cur = cur->next;

    if (!cur) {
        n->prev = q->tail;
        q->tail->next = n;
        q->tail = n;
    } else if (cur == q->head) {
        n->next = q->head;
        q->head->prev = n;
        q->head = n;
    } else {
        n->prev = cur->prev;
        n->next = cur;
        cur->prev->next = n;
        cur->prev = n;
    }
    q->size++;
}

int priorityQueueDequeue(PriorityQueue *q, WarehouseRecord *out) {
    if (!q->head) return 0;
    ListNode *t = q->head;
    if (out) memcpy(out, &t->data, sizeof(WarehouseRecord));
    q->head = t->next;
    if (q->head) q->head->prev = NULL;
    else q->tail = NULL;
    freeNode(t);
    q->size--;
    return 1;
}

void priorityQueueTraverse(const PriorityQueue *q) {
    if (!q->head) { printf("(priority queue empty)\n"); return; }
    printf("Priority Queue (highest unit price first), size = %d:\n", q->size);
    ListNode *cur = q->head;
    int idx = 0;
    while (cur) {
        printf("[%d] (priority %.2f) ", idx++, cur->data.unitPrice);
        displayWarehouseRecord(&cur->data);
        cur = cur->next;
    }
}

ListNode *priorityQueueSearchByPosition(const PriorityQueue *q, int position) {
    ListNode *cur = q->head;
    int i = 0;
    while (cur && i < position) { cur = cur->next; i++; }
    return cur;
}

ListNode *priorityQueueSearchBySurname(const PriorityQueue *q, const char *surname) {
    ListNode *cur = q->head;
    while (cur) {
        if (strcmp(cur->data.ownerSurname, surname) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int priorityQueueDeleteByPosition(PriorityQueue *q, int position) {
    ListNode *node = priorityQueueSearchByPosition(q, position);
    if (!node) return 0;
    if (node->prev) node->prev->next = node->next;
    else q->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else q->tail = node->prev;
    freeNode(node);
    q->size--;
    return 1;
}

void priorityQueueFree(PriorityQueue *q) {
    while (priorityQueueDequeue(q, NULL)) {}
}

static void printMenu(void) {
    printf("\n--- PRIORITY QUEUE menu (priority = unit price, desc) ---\n");
    printf("1. Enqueue (priority insert)\n");
    printf("2. Dequeue (highest-priority)\n");
    printf("3. Traverse / display\n");
    printf("4. Search by position\n");
    printf("5. Search by surname\n");
    printf("6. Delete by position\n");
    printf("7. Register queue to file\n");
    printf("8. Load queue from file\n");
    printf("9. Display file contents\n");
    printf("10. Delete a file\n");
    printf("0. Back\n");
    printf("Choice: ");
}

void priorityQueueRunMenu(PriorityQueue *q) {
    int op;
    do {
        printMenu();
        op = readMenu();
        switch (op) {
            case 1: {
                WarehouseRecord r;
                printf("\nNew record:\n");
                inputWarehouseRecord(&r);
                priorityQueueEnqueue(q, &r);
                printf("Inserted. Size = %d.\n", q->size);
                break;
            }
            case 2: {
                WarehouseRecord r;
                if (priorityQueueDequeue(q, &r)) {
                    printf("Dequeued (highest priority): ");
                    displayWarehouseRecord(&r);
                } else printf("Queue is empty.\n");
                break;
            }
            case 3: priorityQueueTraverse(q); break;
            case 4: {
                printf("Position (0 = highest priority): ");
                int p = readMenu();
                ListNode *n = priorityQueueSearchByPosition(q, p);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Position out of range.\n");
                break;
            }
            case 5: {
                char surname[NAME_LEN];
                printf("Surname: ");
                if (fgets(surname, sizeof(surname), stdin)) {
                    size_t len = strlen(surname);
                    if (len > 0 && surname[len-1] == '\n') surname[len-1] = '\0';
                }
                ListNode *n = priorityQueueSearchBySurname(q, surname);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Not found.\n");
                break;
            }
            case 6: {
                printf("Position to delete: ");
                int p = readMenu();
                if (priorityQueueDeleteByPosition(q, p))
                    printf("Deleted. Size = %d.\n", q->size);
                else printf("Position out of range.\n");
                break;
            }
            case 7: queueRegisterToFile(q->head, "Priority Queue"); break;
            case 8: {
                ListNode *h = NULL, *t = NULL;
                if (queueLoadFromFile(&h, &t, "Priority Queue")) {
                    priorityQueueFree(q);
                    /* re-insert through priorityQueueEnqueue to preserve sort order */
                    ListNode *cur = h;
                    while (cur) {
                        priorityQueueEnqueue(q, &cur->data);
                        cur = cur->next;
                    }
                    while (h) { ListNode *nx = h->next; freeNode(h); h = nx; }
                }
                break;
            }
            case 9: queueDisplayFileFromMenu(); break;
            case 10: queueDeleteFileFromMenu(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (op != 0);
}
