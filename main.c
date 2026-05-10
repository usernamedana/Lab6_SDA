#include <stdio.h>
#include "stack.h"
#include "queue_simple.h"
#include "queue_deque.h"
#include "queue_circular.h"
#include "queue_priority.h"

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

static void runQueueSubmenu(void) {
    int choice;
    do {
        printf("\n QUEUE SUB-MENU \n");
        printf("1. Simple Queue\n");
        printf("2. Double-Ended Queue\n");
        printf("3. Circular Queue\n");
        printf("4. Priority Queue\n");
        printf("0. Back to main\n");
        printf("Choice: ");
        choice = readMenu();
        switch (choice) {
            case 1: {
                SimpleQueue q;
                simpleQueueInit(&q);
                simpleQueueRunMenu(&q);
                simpleQueueFree(&q);
                break;
            }
            case 2: {
                Deque d;
                dequeInit(&d);
                dequeRunMenu(&d);
                dequeFree(&d);
                break;
            }
            case 3: {
                CircularQueue q;
                circularQueueInit(&q);
                circularQueueRunMenu(&q);
                circularQueueFree(&q);
                break;
            }
            case 4: {
                PriorityQueue q;
                priorityQueueInit(&q);
                priorityQueueRunMenu(&q);
                priorityQueueFree(&q);
                break;
            }
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void printMainMenu(void) {
    printf("\n");
    printf("1. Stack\n");
    printf("2. Queue (Simple / Double-Ended / Circular / Priority)\n");
    printf("0. Exit\n");
    printf("Choice: ");
}

int main(void) {
    int choice;
    do {
        printMainMenu();
        choice = readMenu();
        switch (choice) {
            case 1: {
                Stack s;
                stackInit(&s);
                stackRunMenu(&s);
                stackFree(&s);
                break;
            }
            case 2: runQueueSubmenu(); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    printf("Program finished.\n");
    return 0;
}
