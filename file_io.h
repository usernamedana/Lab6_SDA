#ifndef FILE_IO_H
#define FILE_IO_H

#include "list_node.h"

#define MODE_TEXT   1
#define MODE_BINARY 2

void readFullPath(char *path, size_t size);

int saveListToFile(const ListNode *head, const char *path, int mode);
int loadListFromFile(ListNode **head, ListNode **tail, const char *path, int mode);
int deleteFile(const char *path);
int displayFileContents(const char *path, int mode);

#endif
