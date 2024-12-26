#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node {
    char *key;
    void *data;
    struct Node *next;
} node_t;

typedef struct List {
    node_t *head;
    size_t length;
} list_t;

void listConstructor(list_t *list);
void listInsert(list_t *list, char *key, void *data);
void *listSearch(list_t *list, const char *key);
void listDestructor(list_t *list);

#endif
