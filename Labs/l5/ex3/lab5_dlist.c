#include "lab5_dlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct Node {
    char *key;
    dlistValue value;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
    dlistValueType type;
    int count;
} List;

dlist createDlist(dlistValueType type) {
    if (type == DLIST_UNKOWN) return NULL;

    dlist list = (List *)malloc(sizeof(List));
    ((List *)list)->type = type;
    ((List *)list)->head = NULL;
    ((List *)list)->count = 0;

    return list;
}

int dlistIsEmpty(dlist_const this) {
    return ((List *)this)->count == 0;
}

void dlistAppend(dlist this, const char *key, dlistValue value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = (char *)malloc(strlen(key) + 1);
    strcpy(node->key, key);

    if (((List *)this)->type == DLIST_INT)
        node->value.intValue = value.intValue;
    else if (((List *)this)->type == DLIST_DOUBLE)
        node->value.doubleValue = value.doubleValue;
    else if (((List *)this)->type == DLIST_STR) {
        node->value.strValue = malloc(strlen(value.strValue) + 1);
        strcpy(node->value.strValue, value.strValue);
    }

    node->next = NULL;

    if (dlistIsEmpty(this)) {
        ((List *)this)->head = node;
    } else {
        Node *temp = ((List *)this)->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
    }
    ((List *)this)->count++;
}

static int rnd(const void *a, const void *b) {
    (void)a, (void)b;
    return ((rand() % 2) * 2 - 1);
}

static int int_inc(const void *a, const void *b) {
    return (*(Node **)a)->value.intValue - (*(Node **)b)->value.intValue;
}

static int int_dec(const void *a, const void *b) {
    return (*(Node **)b)->value.intValue - (*(Node **)a)->value.intValue;
}

static int double_inc(const void *a, const void *b) {
    return (*(Node **)a)->value.doubleValue > (*(Node **)b)->value.doubleValue ? 1 : -1;
}

static int double_dec(const void *a, const void *b) {
    return (*(Node **)a)->value.doubleValue < (*(Node **)b)->value.doubleValue ? 1 : -1;
}

static int char_inc(const void *a, const void *b) {
    return strcmp((*(Node **)a)->value.strValue, (*(Node **)b)->value.strValue);
}

static int char_dec(const void *a, const void *b) {
    return strcmp((*(Node **)b)->value.strValue, (*(Node **)a)->value.strValue);
}

int (*const cmp[3][3])(const void *, const void *) = {
    {rnd, int_inc, int_dec},
    {rnd, char_inc, char_dec},
    {rnd, double_inc, double_dec}
};

void dlistSort(dlist_const this, dlist listDst, dlistSortMethod method) {
    size_t count = (size_t)(((List *)this)->count);
    if (count == 0) return;

    Node *temp = ((List *)this)->head;
    Node **vec = malloc(sizeof(Node) * count);
    for (size_t i = 0; i < count; i++) {
        vec[i] = temp;
        temp = temp->next;
    }

    qsort(vec, count, sizeof(Node *), cmp[((List *)this)->type - 1][method - 1]);

    if (((List *)listDst)->count != 0)  freeNodes(listDst);

    ((List *)listDst)->count = (int)count;
    ((List *)listDst)->head = vec[0];

    temp = ((List *)listDst)->head;
    for (size_t i = 1; i < count; i++) {
        temp->next = vec[i];
        temp = temp->next;
    }
    temp->next = NULL;
    ((List *)this)->count = 0;

    free(vec);
}


void dlistPrint(dlist_const this) {
    Node *temp = ((List *)this)->head;
    while (temp != NULL) {
        if (((List *)this)->type == DLIST_INT) {
            printf("%s=%d\n", temp->key, temp->value.intValue);
        } else if (((List *)this)->type == DLIST_DOUBLE) {
            printf("%s=%lf\n", temp->key, temp->value.doubleValue);
        } else {
            printf("%s=%s\n", temp->key, temp->value.strValue);
        }
        temp = temp->next;
    }
}

void freeNodes(dlist this) {
    if (((List *)this)->count == 0) return;
    Node *temp = ((List *)this)->head;
    while (temp != NULL) {
        Node *current = temp;
        free(current->key);
        if (((List *)this)->type == DLIST_STR) free(temp->value.strValue);
        temp = temp->next;
        free(current);
    }
    ((List *)this)->count = 0;
}

void dlistFree(dlist this) {
    freeNodes(this);
    free(this);
}
