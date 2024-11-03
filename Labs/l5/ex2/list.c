#include "list.h"

void listConstructor(List* list) {
    list->head = NULL;
    list->count = 0;
}

void listInsert(List* list, char* string, void* data) {
    Node* newNode = (Node *)malloc(sizeof(Node));
    if (newNode) {
        newNode->string = string;
        newNode->data = data;
        newNode->next = NULL;
    }

    if(list->head == NULL) {
        list->head = newNode;
    }
    else{
        Node *temp = list->head;
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    list->count++;
}

void *listSearch(List* list, const char* string) {
    if(list->head == NULL) {
        return NULL;
    }
    Node* temp;
    for (temp = list->head; temp != NULL; temp = temp->next) {
        if(!strcmp(string, temp->string)) {
            return temp->data;
        }
    }
    return NULL;
}

void listDestructor(List* list) {
    Node* current = list->head;
    while (current) {
        Node* temp = current;
        current = current->next;
        if (temp->string != NULL) {
            free(temp->string);
        }
        if (temp->data != NULL) {
            free(temp->data);
        }
        free(temp);
    }
}
