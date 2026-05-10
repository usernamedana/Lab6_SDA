#include "queue_file_ops.h"
#include "file_io.h"
#include <stdio.h>
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

static int askMode(void) {
    printf("1. Text mode (.txt)\n2. Binary mode\nMode: ");
    int m = readMenu();
    if (m == 1) return MODE_TEXT;
    if (m == 2) return MODE_BINARY;
    return 0;
}

void queueRegisterToFile(const ListNode *head, const char *label) {
    char path[512];
    printf("\nRegister %s to file\n", label);
    int mode = askMode();
    if (!mode) { printf("Invalid mode.\n"); return; }
    readFullPath(path, sizeof(path));
    if (saveListToFile(head, path, mode))
        printf("%s saved to '%s'.\n", label, path);
}

int queueLoadFromFile(ListNode **head, ListNode **tail, const char *label) {
    char path[512];
    printf("\nLoad %s from file\n", label);
    int mode = askMode();
    if (!mode) { printf("Invalid mode.\n"); return 0; }
    readFullPath(path, sizeof(path));
    if (!loadListFromFile(head, tail, path, mode)) return 0;
    printf("%s loaded from '%s'.\n", label, path);
    return 1;
}

void queueDisplayFileFromMenu(void) {
    char path[512];
    int mode = askMode();
    if (!mode) { printf("Invalid mode.\n"); return; }
    readFullPath(path, sizeof(path));
    displayFileContents(path, mode);
}

void queueDeleteFileFromMenu(void) {
    char path[512];
    readFullPath(path, sizeof(path));
    deleteFile(path);
}
