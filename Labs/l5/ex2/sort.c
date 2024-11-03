#include "sort.h"

static int rnd(const void *a, const void *b) {
  (void)a, (void)b;
  return (((rand()) % 2) * 2 - 1);
}

static int int_inc(const void *a, const void *b) {
  return *((int *)(((Node *)a)->data)) - *((int *)(((Node *)b)->data));
}

static int int_dec(const void *a, const void *b) {
  return *(int *)(((Node *)b)->data) - *(int *)(((Node *)a)->data);
}

static int double_inc(const void *a, const void *b) {
  return *(double *)(((Node *)a)->data) > *(double *)(((Node *)b)->data) ? 1 : -1;
}

static int double_dec(const void *a, const void *b) {
  return *(double *)(((Node *)a)->data) < *(double *)(((Node *)b)->data) ? 1 : -1;
}

static int char_inc(const void *a, const void *b) {
  return strcmp((char *)(((Node *)a)->data), (char *)(((Node *)b)->data));
}

static int char_dec(const void *a, const void *b) {
  return strcmp((char *)(((Node *)b)->data), (char *)(((Node *)a)->data));
}

int (*const cmp[3][3])(const void *, const void *) = {
    {rnd, char_inc, char_dec},
    {rnd, int_inc, int_dec},
    {rnd, double_inc, double_dec}
};

void sorting(Type* type, List* list) {
    printf("sorting elements\n");
    size_t count = list->count;
    if (count < 2)
        return;

    Node *temp_node = list->head;
    Node *temp_vec = malloc(sizeof(Node) * count);
    for (size_t i = 0; i < count; i++) {
        memcpy(temp_vec + i, temp_node, sizeof(Node));
        temp_node = temp_node->next;
    }

    qsort(temp_vec, count, sizeof(Node), cmp[type->dataType][type->sortingType]);

    temp_node = list->head;
    for (size_t i = 0; i < count; i++) {
        temp_node->string = temp_vec[i].string;
        temp_node->data = temp_vec[i].data;
        temp_node = temp_node->next;
    }

    free(temp_vec);
}
