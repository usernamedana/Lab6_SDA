#include "queue_circular.h"
#include "queue_file_ops.h"
#include "file_io.h"
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

void circularQueueInit(CircularQueue *q) {
    q->tail = NULL;
    q->size = 0;
}

void circularQueueEnqueue(CircularQueue *q, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }
    if (!q->tail) {
        q->tail = n;
        n->next = n;
        n->prev = n;
    } else {
        ListNode *head = q->tail->next;
        n->next = head;
        n->prev = q->tail;
        q->tail->next = n;
        head->prev = n;
        q->tail = n;
    }
    q->size++;
}

int circularQueueDequeue(CircularQueue *q, WarehouseRecord *out) {
    if (!q->tail) return 0;
    ListNode *head = q->tail->next;
    if (out) memcpy(out, &head->data, sizeof(WarehouseRecord));
    if (head == q->tail) {
        freeNode(head);
        q->tail = NULL;
    } else {
        ListNode *new_head = head->next;
        q->tail->next = new_head;
        new_head->prev = q->tail;
        freeNode(head);
    }
    q->size--;
    return 1;
}

void circularQueueTraverse(const CircularQueue *q) {
    if (!q->tail) { printf("(circular queue empty)\n"); return; }
    printf("Circular Queue (front -> back, wraps), size = %d:\n", q->size);
    ListNode *head = q->tail->next;
    ListNode *cur = head;
    int idx = 0;
    do {
        printf("[%d] ", idx++);
        displayWarehouseRecord(&cur->data);
        cur = cur->next;
    } while (cur != head);
}

ListNode *circularQueueSearchByPosition(const CircularQueue *q, int position) {
    if (!q->tail || position < 0 || position >= q->size) return NULL;
    ListNode *cur = q->tail->next;
    for (int i = 0; i < position; i++) cur = cur->next;
    return cur;
}

ListNode *circularQueueSearchBySurname(const CircularQueue *q, const char *surname) {
    if (!q->tail) return NULL;
    ListNode *head = q->tail->next;
    ListNode *cur = head;
    do {
        if (strcmp(cur->data.ownerSurname, surname) == 0) return cur;
        cur = cur->next;
    } while (cur != head);
    return NULL;
}

int circularQueueDeleteByPosition(CircularQueue *q, int position) {
    ListNode *node = circularQueueSearchByPosition(q, position);
    if (!node) return 0;
    if (q->size == 1) {
        freeNode(node);
        q->tail = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (node == q->tail) q->tail = node->prev;
        freeNode(node);
    }
    q->size--;
    return 1;
}

void circularQueueFree(CircularQueue *q) {
    while (circularQueueDequeue(q, NULL)) {}
}

/* For file I/O we treat the circular queue as a linear list head->tail
 * and rebuild circularity on load. This requires temporarily breaking
 * the cycle. */

static void linearizeForSave(const CircularQueue *q, ListNode **head, ListNode **tail) {
    if (!q->tail) { *head = *tail = NULL; return; }
    *head = q->tail->next;
    *tail = q->tail;
    /* break the back link so save sees a normal singly-flowing list */
    (*tail)->next = NULL;
    (*head)->prev = NULL;
}

static void restoreCircularity(CircularQueue *q, ListNode *head, ListNode *tail) {
    if (!head) return;
    tail->next = head;
    head->prev = tail;
    q->tail = tail;
}

static void registerCircularToFile(CircularQueue *q) {
    if (!q->tail) {
        char path[512];
        printf("\nRegister Circular Queue to file (empty)\n");
        printf("1. Text mode (.txt)\n2. Binary mode\nMode: ");
        int m = readMenu();
        if (m != 1 && m != 2) { printf("Invalid mode.\n"); return; }
        readFullPath(path, sizeof(path));
        if (saveListToFile(NULL, path, m == 2 ? MODE_BINARY : MODE_TEXT))
            printf("Empty queue saved to '%s'.\n", path);
        return;
    }
    ListNode *h, *t;
    linearizeForSave(q, &h, &t);
    queueRegisterToFile(h, "Circular Queue");
    restoreCircularity(q, h, t);
}

static void loadCircularFromFile(CircularQueue *q) {
    ListNode *h = NULL, *t = NULL;
    if (!queueLoadFromFile(&h, &t, "Circular Queue")) return;
    circularQueueFree(q);
    if (h) {
        restoreCircularity(q, h, t);
        int cnt = 0; ListNode *c = h;
        do { cnt++; c = c->next; } while (c != h);
        q->size = cnt;
    } else {
        q->tail = NULL;
        q->size = 0;
    }
}

static void printMenu(void) {
    printf("\n--- CIRCULAR QUEUE menu ---\n");
    printf("1. Enqueue\n");
    printf("2. Dequeue\n");
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

void circularQueueRunMenu(CircularQueue *q) {
    int op;
    do {
        printMenu();
        op = readMenu();
        switch (op) {
            case 1: {
                WarehouseRecord r;
                printf("\nNew record:\n");
                inputWarehouseRecord(&r);
                circularQueueEnqueue(q, &r);
                printf("Enqueued. Size = %d.\n", q->size);
                break;
            }
            case 2: {
                WarehouseRecord r;
                if (circularQueueDequeue(q, &r)) {
                    printf("Dequeued: ");
                    displayWarehouseRecord(&r);
                } else printf("Queue is empty.\n");
                break;
            }
            case 3: circularQueueTraverse(q); break;
            case 4: {
                printf("Position (0 = front): ");
                int p = readMenu();
                ListNode *n = circularQueueSearchByPosition(q, p);
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
                ListNode *n = circularQueueSearchBySurname(q, surname);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Not found.\n");
                break;
            }
            case 6: {
                printf("Position to delete: ");
                int p = readMenu();
                if (circularQueueDeleteByPosition(q, p))
                    printf("Deleted. Size = %d.\n", q->size);
                else printf("Position out of range.\n");
                break;
            }
            case 7: registerCircularToFile(q); break;
            case 8: loadCircularFromFile(q); break;
            case 9: queueDisplayFileFromMenu(); break;
            case 10: queueDeleteFileFromMenu(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (op != 0);
}
