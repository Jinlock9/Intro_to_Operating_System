#include "list.h"

void listConstructor(list_t *list) {
    list->head = NULL;
    list->length = 0;
}

void listInsert(list_t *list, char *key, void *data) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node) {
        new_node->key = key;
        new_node->data = data;
        new_node->next = NULL;
    }

    if(list->head == NULL) {
        list->head = new_node;
    }
    else{
        node_t *node_it = list->head;
        while(node_it->next != NULL) {
            node_it = node_it->next;
        }
        node_it->next = new_node;
    }
    list->length++;
}

void *listSearch(list_t *list, const char *key) {
    if(list->head == NULL) {
        return NULL;
    }
    node_t *node_it;
    for (node_it = list->head; node_it != NULL; node_it = node_it->next) {
        if(!strcmp(key, node_it->key)) {
            return node_it->data;
        }
    }
    return NULL;
}

void listDestructor(list_t *list) {
    node_t *node_it = list->head;
    while (node_it) {
        node_t *temp = node_it;
        node_it = node_it->next;
        if (temp->key != NULL) free(temp->key);
        if (temp->data != NULL) free(temp->data);
        free(temp);
    }
}
