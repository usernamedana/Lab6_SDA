#include "stack.h"
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

void stackInit(Stack *s) {
    s->top = NULL;
    s->size = 0;
}

void stackPush(Stack *s, const WarehouseRecord *r) {
    ListNode *n = createNode(r);
    if (!n) { printf("Memory error.\n"); return; }
    n->next = s->top;
    if (s->top) s->top->prev = n;
    s->top = n;
    s->size++;
}

int stackPop(Stack *s, WarehouseRecord *out) {
    if (!s->top) return 0;
    ListNode *t = s->top;
    if (out) memcpy(out, &t->data, sizeof(WarehouseRecord));
    s->top = t->next;
    if (s->top) s->top->prev = NULL;
    freeNode(t);
    s->size--;
    return 1;
}

void stackTraverse(const Stack *s) {
    if (!s->top) { printf("(stack empty)\n"); return; }
    printf("Stack (top -> bottom), size = %d:\n", s->size);
    ListNode *cur = s->top;
    int idx = 0;
    while (cur) {
        printf("[%d] ", idx++);
        displayWarehouseRecord(&cur->data);
        cur = cur->next;
    }
}

ListNode *stackSearchByPosition(const Stack *s, int position) {
    ListNode *cur = s->top;
    int i = 0;
    while (cur && i < position) { cur = cur->next; i++; }
    return cur;
}

ListNode *stackSearchBySurname(const Stack *s, const char *surname) {
    ListNode *cur = s->top;
    while (cur) {
        if (strcmp(cur->data.ownerSurname, surname) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

int stackDeleteByPosition(Stack *s, int position) {
    ListNode *node = stackSearchByPosition(s, position);
    if (!node) return 0;
    if (node->prev) node->prev->next = node->next;
    else s->top = node->next;
    if (node->next) node->next->prev = node->prev;
    freeNode(node);
    s->size--;
    return 1;
}

void stackFree(Stack *s) {
    while (stackPop(s, NULL)) {}
}

static void registerToFile(const Stack *s) {
    char path[512];
    printf("\nRegister stack to file\n");
    printf("1. Text mode (.txt)\n");
    printf("2. Binary mode\n");
    printf("Mode: ");
    int mode = readMenu();
    if (mode != 1 && mode != 2) { printf("Invalid mode.\n"); return; }
    readFullPath(path, sizeof(path));
    if (saveListToFile(s->top, path, mode == 2 ? MODE_BINARY : MODE_TEXT))
        printf("Stack saved to '%s'.\n", path);
}

static void loadFromFile(Stack *s) {
    char path[512];
    printf("\nLoad stack from file\n");
    printf("1. Text mode (.txt)\n");
    printf("2. Binary mode\n");
    printf("Mode: ");
    int mode = readMenu();
    if (mode != 1 && mode != 2) { printf("Invalid mode.\n"); return; }
    readFullPath(path, sizeof(path));

    ListNode *head = NULL, *tail = NULL;
    if (!loadListFromFile(&head, &tail, path, mode == 2 ? MODE_BINARY : MODE_TEXT)) return;

    stackFree(s);
    /* file order = bottom-to-top, so push in reverse to preserve top */
    /* simpler: traverse to tail, then push each going backwards */
    ListNode *cur = tail;
    while (cur) {
        stackPush(s, &cur->data);
        cur = cur->prev;
    }
    while (head) {
        ListNode *nx = head->next;
        freeNode(head);
        head = nx;
    }
    printf("Stack loaded from '%s'.\n", path);
}

static void deleteFileFromMenu(void) {
    char path[512];
    readFullPath(path, sizeof(path));
    deleteFile(path);
}

static void displayFileFromMenu(void) {
    char path[512];
    printf("1. Text mode\n2. Binary mode\nMode: ");
    int mode = readMenu();
    if (mode != 1 && mode != 2) { printf("Invalid mode.\n"); return; }
    readFullPath(path, sizeof(path));
    displayFileContents(path, mode == 2 ? MODE_BINARY : MODE_TEXT);
}

static void printStackMenu(void) {
    printf("\n--- STACK menu ---\n");
    printf("1. Push (insert on top)\n");
    printf("2. Pop (delete from top)\n");
    printf("3. Traverse / display\n");
    printf("4. Search by position\n");
    printf("5. Search by surname\n");
    printf("6. Delete by position\n");
    printf("7. Register stack to file\n");
    printf("8. Load stack from file\n");
    printf("9. Display file contents\n");
    printf("10. Delete a file\n");
    printf("0. Back\n");
    printf("Choice: ");
}

void stackRunMenu(Stack *s) {
    int op;
    do {
        printStackMenu();
        op = readMenu();
        switch (op) {
            case 1: {
                WarehouseRecord r;
                printf("\nNew record:\n");
                inputWarehouseRecord(&r);
                stackPush(s, &r);
                printf("Pushed. Size = %d.\n", s->size);
                break;
            }
            case 2: {
                WarehouseRecord r;
                if (stackPop(s, &r)) {
                    printf("Popped: ");
                    displayWarehouseRecord(&r);
                } else printf("Stack is empty.\n");
                break;
            }
            case 3: stackTraverse(s); break;
            case 4: {
                printf("Position (0 = top): ");
                int p = readMenu();
                ListNode *n = stackSearchByPosition(s, p);
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
                ListNode *n = stackSearchBySurname(s, surname);
                if (n) { printf("Found: "); displayWarehouseRecord(&n->data); }
                else printf("Not found.\n");
                break;
            }
            case 6: {
                printf("Position to delete: ");
                int p = readMenu();
                if (stackDeleteByPosition(s, p))
                    printf("Deleted. Size = %d.\n", s->size);
                else printf("Position out of range.\n");
                break;
            }
            case 7: registerToFile(s); break;
            case 8: loadFromFile(s); break;
            case 9: displayFileFromMenu(); break;
            case 10: deleteFileFromMenu(); break;
            case 0: break;
            default: printf("Invalid option.\n");
        }
    } while (op != 0);
}
