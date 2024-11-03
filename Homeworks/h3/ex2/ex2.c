#include "ex2.h"

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

void parsingType(Type* type, char** argv) {
    char filename[MAX_FILE];
    strcpy(filename, argv[1]);

    char *token = strtok(filename, "_");
    token = strtok(NULL, ".");

    if (!strcmp(token, "int")) {
        type->dataType = TYPE_INT;
    } else if (!strcmp(token, "double")) {
        type->dataType = TYPE_DOUBLE;
    } else {
        type->dataType = TYPE_CHAR;
    }

    if (!strcmp(argv[2], "inc")) {
        type->sortingType = TYPE_INC;
    } else if (!strcmp(argv[2], "dec")) {
        type->sortingType = TYPE_DEC;
    } else {
        type->sortingType = TYPE_RAND;
    }
}

void reader(Type* type, List* list, char** argv) {
    printf("reading %s\n", argv[1]);

    char* line = malloc(sizeof(char) * MAX_LINE);
    memset(line, 0, MAX_LINE);

    char* temp_string;
    char* temp_data;

    FILE* fin = fopen(argv[1], "r");
    while (fgets(line, MAX_LINE, fin) != NULL) {
        char* string = NULL;
        void* data = NULL;

        temp_string = strtok(line, "=");
        string = malloc(sizeof(char) * MAX_TEMP);
        memset(string, 0, MAX_TEMP);
        strcpy(string, temp_string);

        temp_data = strtok(NULL, "\n");
        if (temp_data == NULL) {
            if (string != NULL) {
                free(string);
            }
        } else {
            if (type->dataType == TYPE_INT) {
                int temp_val = atoi(temp_data);
                data = (void *)malloc(sizeof(int));
                *(int *)data = temp_val;
            } else if (type->dataType == TYPE_DOUBLE) {
                double temp_val = atof(temp_data);
                data = (void *)malloc(sizeof(double));
                *(double *)data = temp_val;
            } else {
                data = (void *)malloc(sizeof(char) * MAX_TEMP);
                memset(data, 0, MAX_TEMP);
                strcpy((char *)data, temp_data);
            }    

            listInsert(list, string, data);
        }
        memset(line, 0, MAX_LINE);
    }
    free(line);
    fclose(fin);   
}

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

void writer(Type* type, const List* list) {
    char outFile[MAX_FILE] = {0,};

    if (type->sortingType == TYPE_INC) {
        strcat(outFile, "inc_");
    } else if (type->sortingType == TYPE_DEC) {
        strcat(outFile, "dec_");
    } else {
        strcat(outFile, "rand_");
    }

    if (type->dataType == TYPE_INT) {
        strcat(outFile, "int.txt");
    } else if (type->dataType == TYPE_DOUBLE) {
        strcat(outFile, "double.txt");
    } else {
        strcat(outFile, "char*.txt");
    }

    printf("writing %s\n", outFile);

    FILE* fout = fopen(outFile, "w");
    if (list->head == NULL) {
        return;
    }
    Node* temp;
    for (temp = list->head; temp != NULL; temp = temp->next) {
        if (type->dataType == TYPE_INT) {
            fprintf(fout, "%s=%d\n", temp->string, *(int *)(temp->data));
        } else if (type->dataType == TYPE_DOUBLE) {
            fprintf(fout, "%s=%lf\n", temp->string, *(double *)(temp->data));
        } else {
            fprintf(fout, "%s=%s\n", temp->string, (char *)(temp->data));
        }
    }
    fclose(fout); 
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return 0;
    }

    srand((unsigned int)time(NULL));

    Type type;
    parsingType(&type, argv);

    List list;
    listConstructor(&list);

    reader(&type, &list, argv);
    sorting(&type, &list);
    writer(&type, &list);

    listDestructor(&list);

    return 0;
}
