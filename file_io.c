#include "file_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readFullPath(char *path, size_t size) {
    printf("Enter full file path: ");
    if (fgets(path, (int)size, stdin) != NULL) {
        size_t len = strlen(path);
        if (len > 0 && path[len - 1] == '\n') path[len - 1] = '\0';
    }
}

static int countNodes(const ListNode *head) {
    int n = 0;
    while (head) { n++; head = head->next; }
    return n;
}

static int saveText(const ListNode *head, const char *path) {
    FILE *fp = fopen(path, "w");
    if (!fp) { printf("Cannot open '%s' for writing (text).\n", path); return 0; }

    int total = countNodes(head);
    fprintf(fp, "COUNT=%d\n", total);

    int idx = 0;
    while (head) {
        const WarehouseRecord *r = &head->data;
        fprintf(fp, "[%d]\n", idx++);
        fprintf(fp, "ownerName=%s\n",      r->ownerName);
        fprintf(fp, "ownerSurname=%s\n",   r->ownerSurname);
        fprintf(fp, "productName=%s\n",    r->productName);
        fprintf(fp, "manufacturer=%s\n",   r->manufacturer);
        fprintf(fp, "contractDate=%02d.%02d.%04d\n",
                r->contractDate.day, r->contractDate.month, r->contractDate.year);
        fprintf(fp, "currency=%s\n",       r->currency);
        fprintf(fp, "wholesalePrice=%.2f\n", r->wholesalePrice);
        fprintf(fp, "unitPrice=%.2f\n",    r->unitPrice);
        fprintf(fp, "quantity=%u\n",       r->quantity);
        head = head->next;
    }
    fclose(fp);
    return 1;
}

static int loadText(ListNode **head, ListNode **tail, const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) { printf("Cannot open '%s' for reading (text).\n", path); return 0; }

    int count = 0;
    if (fscanf(fp, "COUNT=%d\n", &count) != 1) {
        printf("File '%s' has no COUNT header.\n", path);
        fclose(fp); return 0;
    }

    *head = *tail = NULL;
    char line[256];

    for (int i = 0; i < count; i++) {
        WarehouseRecord r;
        memset(&r, 0, sizeof(r));

        if (fgets(line, sizeof(line), fp) == NULL) goto bad;

        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "ownerName=%49[^\n]", r.ownerName) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "ownerSurname=%49[^\n]", r.ownerSurname) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "productName=%79[^\n]", r.productName) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "manufacturer=%79[^\n]", r.manufacturer) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "contractDate=%d.%d.%d",
                   &r.contractDate.day, &r.contractDate.month, &r.contractDate.year) != 3) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "currency=%9[^\n]", r.currency) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "wholesalePrice=%lf", &r.wholesalePrice) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "unitPrice=%lf", &r.unitPrice) != 1) goto bad;
        if (fgets(line, sizeof(line), fp) == NULL) goto bad;
        if (sscanf(line, "quantity=%u", &r.quantity) != 1) goto bad;

        ListNode *node = createNode(&r);
        if (!node) goto bad;
        if (!*head) { *head = *tail = node; }
        else { (*tail)->next = node; node->prev = *tail; *tail = node; }
    }
    fclose(fp);
    return 1;

bad:
    printf("File '%s' is corrupted (text mode).\n", path);
    fclose(fp);
    return 0;
}

static int saveBinary(const ListNode *head, const char *path) {
    FILE *fp = fopen(path, "wb");
    if (!fp) { printf("Cannot open '%s' for writing (binary).\n", path); return 0; }

    int total = countNodes(head);
    if (fwrite(&total, sizeof(int), 1, fp) != 1) { fclose(fp); return 0; }

    while (head) {
        if (fwrite(&head->data, sizeof(WarehouseRecord), 1, fp) != 1) {
            fclose(fp); return 0;
        }
        head = head->next;
    }
    fclose(fp);
    return 1;
}

static int loadBinary(ListNode **head, ListNode **tail, const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) { printf("Cannot open '%s' for reading (binary).\n", path); return 0; }

    int total = 0;
    if (fread(&total, sizeof(int), 1, fp) != 1) {
        printf("File '%s' has no count header (binary).\n", path);
        fclose(fp); return 0;
    }
    if (total < 0) { fclose(fp); return 0; }

    *head = *tail = NULL;
    for (int i = 0; i < total; i++) {
        WarehouseRecord r;
        if (fread(&r, sizeof(WarehouseRecord), 1, fp) != 1) {
            printf("File '%s' truncated at record %d (binary).\n", path, i + 1);
            fclose(fp); return 0;
        }
        ListNode *node = createNode(&r);
        if (!node) { fclose(fp); return 0; }
        if (!*head) { *head = *tail = node; }
        else { (*tail)->next = node; node->prev = *tail; *tail = node; }
    }
    fclose(fp);
    return 1;
}

int saveListToFile(const ListNode *head, const char *path, int mode) {
    if (mode == MODE_BINARY) return saveBinary(head, path);
    return saveText(head, path);
}

int loadListFromFile(ListNode **head, ListNode **tail, const char *path, int mode) {
    if (mode == MODE_BINARY) return loadBinary(head, tail, path);
    return loadText(head, tail, path);
}

int deleteFile(const char *path) {
    if (remove(path) == 0) {
        printf("File '%s' deleted.\n", path);
        return 1;
    }
    printf("Cannot delete '%s' (does it exist?).\n", path);
    return 0;
}

int displayFileContents(const char *path, int mode) {
    if (mode == MODE_BINARY) {
        ListNode *head = NULL, *tail = NULL;
        if (!loadBinary(&head, &tail, path)) return 0;
        printf("\nContents of '%s' (binary, decoded):\n", path);
        int idx = 0;
        ListNode *cur = head;
        while (cur) {
            printf("[%d] ", idx++);
            displayWarehouseRecord(&cur->data);
            cur = cur->next;
        }
        while (head) {
            ListNode *next = head->next;
            freeNode(head);
            head = next;
        }
        return 1;
    }

    FILE *fp = fopen(path, "r");
    if (!fp) { printf("Cannot open '%s' for reading.\n", path); return 0; }
    printf("\nContents of '%s' (text):\n", path);
    char line[512];
    while (fgets(line, sizeof(line), fp)) fputs(line, stdout);
    fclose(fp);
    printf("--- end of file ---\n");
    return 1;
}
