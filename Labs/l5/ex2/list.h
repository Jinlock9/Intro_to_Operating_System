#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node {
    char* string;
    void* data;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
    size_t count;
} List;

void listConstructor(List* list);
void listInsert(List* list, char* string, void* data);
void *listSearch(List* list, const char* string);
void listDestructor(List* list);

#endif
