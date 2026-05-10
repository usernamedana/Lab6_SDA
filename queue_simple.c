#include "queue_simple.h"
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

void simpleQueueInit(SimpleQueue *q) {
    q->head = q->tail = NULL;
    q->size = 0;
}

void simpleQueueEnqueue(SimpleQueue *q, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }
    if (!q->tail) { q->head = q->tail = n; }
    else { q->tail->next = n; n->prev = q->tail; q->tail = n; }
    q->size++;
}

int simpleQueueDequeue(SimpleQueue *q, WarehouseRecord *out) {
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

void simpleQueueTraverse(const SimpleQueue *q) {
    if (!q->head) { printf("(queue empty)\n"); return; }
    printf("Simple Queue (front -> back), size = %d:\n", q->size);
    ListNode *cur = q->head;
    int idx = 0;
    while (cur) {
        printf("[%d] ", idx++);
        displayWarehouseRecord(&cur->data);
        cur = cur->next;
    }
}

ListNode *simpleQueueSearchByPosition(const SimpleQueue *q, int position) {
    ListNode *cur = q->head;
    int i = 0;
    while (cur && i < position) { cur = cur->next; i++; }
    return cur;
}

ListNode *simpleQueueSearchBySurname(const SimpleQueue *q, const char *surname) {
    ListNode *cur = q->head;
    while (cur) {
        if (strcmp(cur->data.ownerSurname, surname) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int simpleQueueDeleteByPosition(SimpleQueue *q, int position) {
    ListNode *node = simpleQueueSearchByPosition(q, position);
    if (!node) return 0;
    if (node->prev) node->prev->next = node->next;
    else q->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else q->tail = node->prev;
    freeNode(node);
    q->size--;
    return 1;
}

void simpleQueueFree(SimpleQueue *q) {
    while (simpleQueueDequeue(q, NULL)) {}
}

static void printMenu(void) {
    printf("\n--- SIMPLE QUEUE menu ---\n");
    printf("1. Enqueue (insert at back)\n");
    printf("2. Dequeue (remove from front)\n");
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

void simpleQueueRunMenu(SimpleQueue *q) {
    int op;
    do {
        printMenu();
        op = readMenu();
        switch (op) {
            case 1: {
                WarehouseRecord r;
                printf("\nNew record:\n");
                inputWarehouseRecord(&r);
                simpleQueueEnqueue(q, &r);
                printf("Enqueued. Size = %d.\n", q->size);
                break;
            }
            case 2: {
                WarehouseRecord r;
                if (simpleQueueDequeue(q, &r)) {
                    printf("Dequeued: ");
                    displayWarehouseRecord(&r);
                } else printf("Queue is empty.\n");
                break;
            }
            case 3: simpleQueueTraverse(q); break;
            case 4: {
                printf("Position (0 = front): ");
                int p = readMenu();
                ListNode *n = simpleQueueSearchByPosition(q, p);
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
                ListNode *n = simpleQueueSearchBySurname(q, surname);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Not found.\n");
                break;
            }
            case 6: {
                printf("Position to delete: ");
                int p = readMenu();
                if (simpleQueueDeleteByPosition(q, p))
                    printf("Deleted. Size = %d.\n", q->size);
                else printf("Position out of range.\n");
                break;
            }
            case 7: queueRegisterToFile(q->head, "Simple Queue"); break;
            case 8: {
                ListNode *h = NULL, *t = NULL;
                if (queueLoadFromFile(&h, &t, "Simple Queue")) {
                    simpleQueueFree(q);
                    q->head = h; q->tail = t;
                    int cnt = 0; ListNode *c = h;
                    while (c) { cnt++; c = c->next; }
                    q->size = cnt;
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
