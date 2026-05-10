#include "queue_deque.h"
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

void dequeInit(Deque *d) {
    d->head = d->tail = NULL;
    d->size = 0;
}

void dequePushFront(Deque *d, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }
    if (!d->head) { d->head = d->tail = n; }
    else { n->next = d->head; d->head->prev = n; d->head = n; }
    d->size++;
}

void dequePushBack(Deque *d, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }
    if (!d->tail) { d->head = d->tail = n; }
    else { d->tail->next = n; n->prev = d->tail; d->tail = n; }
    d->size++;
}

int dequePopFront(Deque *d, WarehouseRecord *out) {
    if (!d->head) return 0;
    ListNode *t = d->head;
    if (out) memcpy(out, &t->data, sizeof(WarehouseRecord));
    d->head = t->next;
    if (d->head) d->head->prev = NULL;
    else d->tail = NULL;
    freeNode(t);
    d->size--;
    return 1;
}

int dequePopBack(Deque *d, WarehouseRecord *out) {
    if (!d->tail) return 0;
    ListNode *t = d->tail;
    if (out) memcpy(out, &t->data, sizeof(WarehouseRecord));
    d->tail = t->prev;
    if (d->tail) d->tail->next = NULL;
    else d->head = NULL;
    freeNode(t);
    d->size--;
    return 1;
}

void dequeTraverse(const Deque *d) {
    if (!d->head) { printf("(deque empty)\n"); return; }
    printf("Deque (front -> back), size = %d:\n", d->size);
    ListNode *cur = d->head;
    int idx = 0;
    while (cur) {
        printf("[%d] ", idx++);
        displayWarehouseRecord(&cur->data);
        cur = cur->next;
    }
}

ListNode *dequeSearchByPosition(const Deque *d, int position) {
    ListNode *cur = d->head;
    int i = 0;
    while (cur && i < position) { cur = cur->next; i++; }
    return cur;
}

ListNode *dequeSearchBySurname(const Deque *d, const char *surname) {
    ListNode *cur = d->head;
    while (cur) {
        if (strcmp(cur->data.ownerSurname, surname) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int dequeDeleteByPosition(Deque *d, int position) {
    ListNode *node = dequeSearchByPosition(d, position);
    if (!node) return 0;
    if (node->prev) node->prev->next = node->next;
    else d->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else d->tail = node->prev;
    freeNode(node);
    d->size--;
    return 1;
}

void dequeFree(Deque *d) {
    while (dequePopFront(d, NULL)) {}
}

static void printMenu(void) {
    printf("\n--- DOUBLE-ENDED QUEUE menu ---\n");
    printf("1. Push front\n");
    printf("2. Push back\n");
    printf("3. Pop front\n");
    printf("4. Pop back\n");
    printf("5. Traverse / display\n");
    printf("6. Search by position\n");
    printf("7. Search by surname\n");
    printf("8. Delete by position\n");
    printf("9. Register deque to file\n");
    printf("10. Load deque from file\n");
    printf("11. Display file contents\n");
    printf("12. Delete a file\n");
    printf("0. Back\n");
    printf("Choice: ");
}

void dequeRunMenu(Deque *d) {
    int op;
    do {
        printMenu();
        op = readMenu();
        switch (op) {
            case 1: case 2: {
                WarehouseRecord r;
                printf("\nNew record:\n");
                inputWarehouseRecord(&r);
                if (op == 1) dequePushFront(d, &r);
                else dequePushBack(d, &r);
                printf("Pushed. Size = %d.\n", d->size);
                break;
            }
            case 3: case 4: {
                WarehouseRecord r;
                int ok = (op == 3) ? dequePopFront(d, &r) : dequePopBack(d, &r);
                if (ok) { printf("Popped: "); displayWarehouseRecord(&r); }
                else printf("Deque is empty.\n");
                break;
            }
            case 5: dequeTraverse(d); break;
            case 6: {
                printf("Position (0 = front): ");
                int p = readMenu();
                ListNode *n = dequeSearchByPosition(d, p);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Position out of range.\n");
                break;
            }
            case 7: {
                char surname[NAME_LEN];
                printf("Surname: ");
                if (fgets(surname, sizeof(surname), stdin)) {
                    size_t len = strlen(surname);
                    if (len > 0 && surname[len-1] == '\n') surname[len-1] = '\0';
                }
                ListNode *n = dequeSearchBySurname(d, surname);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Not found.\n");
                break;
            }
            case 8: {
                printf("Position to delete: ");
                int p = readMenu();
                if (dequeDeleteByPosition(d, p))
                    printf("Deleted. Size = %d.\n", d->size);
                else printf("Position out of range.\n");
                break;
            }
            case 9: queueRegisterToFile(d->head, "Deque"); break;
            case 10: {
                ListNode *h = NULL, *t = NULL;
                if (queueLoadFromFile(&h, &t, "Deque")) {
                    dequeFree(d);
                    d->head = h; d->tail = t;
                    int cnt = 0; ListNode *c = h;
                    while (c) { cnt++; c = c->next; }
                    d->size = cnt;
                }
                break;
            }
            case 11: queueDisplayFileFromMenu(); break;
            case 12: queueDeleteFileFromMenu(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (op != 0);
}
