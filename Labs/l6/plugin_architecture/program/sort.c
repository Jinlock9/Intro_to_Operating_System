#include "sort.h"

static int rnd(const void *a, const void *b) {
  (void)a, (void)b;
  return (((rand()) % 2) * 2 - 1);
}

static int int_inc(const void *a, const void *b) {
  return *((int *)(((node_t *)a)->data)) - *((int *)(((node_t *)b)->data));
}

static int int_dec(const void *a, const void *b) {
  return *(int *)(((node_t *)b)->data) - *(int *)(((node_t *)a)->data);
}

static int double_inc(const void *a, const void *b) {
  return *(double *)(((node_t *)a)->data) > *(double *)(((node_t *)b)->data) ? 1 : -1;
}

static int double_dec(const void *a, const void *b) {
  return *(double *)(((node_t *)a)->data) < *(double *)(((node_t *)b)->data) ? 1 : -1;
}

static int char_inc(const void *a, const void *b) {
  return strcmp((char *)(((node_t *)a)->data), (char *)(((node_t *)b)->data));
}

static int char_dec(const void *a, const void *b) {
  return strcmp((char *)(((node_t *)b)->data), (char *)(((node_t *)a)->data));
}

int (*const compare[3][3])(const void *, const void *) = {
    {rnd, char_inc, char_dec},
    {rnd, int_inc, int_dec},
    {rnd, double_inc, double_dec}
};

void sorting(type_t *type, list_t *list) {
    printf("sorting elements\n");
    size_t length = list->length;
    if (length < 2) return;

    node_t *node_it = list->head;
    node_t *temp = malloc(sizeof(node_t) * length);
    for (size_t i = 0; i < length; i++) {
        memcpy(temp + i, node_it, sizeof(node_t));
        node_it = node_it->next;
    }

    qsort(temp, length, sizeof(node_t), compare[type->dataType][type->sortingType]);

    node_it = list->head;
    for (size_t i = 0; i < length; i++) {
        node_it->key = temp[i].key;
        node_it->data = temp[i].data;
        node_it = node_it->next;
    }

    free(temp);
}
