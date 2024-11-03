#ifndef EX2_H
#define EX2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 2048
#define MAX_FILE 20
#define MAX_TEMP 1024

#define TYPE_CHAR 0
#define TYPE_INT 1
#define TYPE_DOUBLE 2

#define TYPE_RAND 0
#define TYPE_INC 1
#define TYPE_DEC 2

typedef struct Node {
    char* string;
    void* data;
    struct Node* next;
} Node;

typedef struct List {
    Node* head;
    size_t count;
} List;

typedef struct Type {
    int dataType;
    int sortingType;
} Type;

#endif
