#include <string.h>
#include "interface.h"

int txtParser(type_t *type, char **argv) {
    char filename[MAX_FILE];
    strcpy(filename, argv[1]);

    char *token = strtok(filename, "_");
    token = strtok(NULL, "_");
    char *data = strtok(token, ".");

    if (!strcmp(data, "int")) type->dataType = DATA_INT;
    else if (!strcmp(data, "double")) type->dataType = DATA_DOUBLE;
    else if (!strcmp(data, "char")) type->dataType = DATA_CHAR;
    else return UNKNOWN;

    if (!strcmp(argv[2], "inc")) type->sortingType = SORT_INC;
    else if (!strcmp(argv[2], "dec")) type->sortingType = SORT_DEC;
    else if (!strcmp(argv[2], "rand"))  type->sortingType = SORT_RAND;
    else return UNKNOWN;

    return 0;
}

int txtReader(type_t *type, list_t *list, char **argv) {
    printf("reading %s\n", argv[1]);

    char *line = malloc(sizeof(char) * MAX_LINE);
    memset(line, 0, MAX_LINE);

    char *temp_key;
    char *temp_data;

    FILE *f_in = fopen(argv[1], "r");
    while (fgets(line, MAX_LINE, f_in) != NULL) {
        char *key = NULL;
        void *data = NULL;

        temp_key = strtok(line, "=");
        key = malloc(sizeof(char) * MAX_LINE);
        memset(key, 0, MAX_LINE);
        strcpy(key, temp_key);

        temp_data = strtok(NULL, "\n");
        if (temp_data == NULL) {
            if (key != NULL) free(key);
        } else {
            if (type->dataType == DATA_INT) {
                int value = atoi(temp_data);
                data = (void *)malloc(sizeof(int));
                *(int *)data = value;
            } else if (type->dataType == DATA_DOUBLE) {
                double value = atof(temp_data);
                data = (void *)malloc(sizeof(double));
                *(double *)data = value;
            } else if (type->dataType == DATA_CHAR) {
                data = (void *)malloc(sizeof(char) * MAX_LINE);
                memset(data, 0, MAX_LINE);
                strcpy((char *)data, temp_data);
            } else return UNKNOWN; 

            listInsert(list, key, data);
        }
        memset(line, 0, MAX_LINE);
    }
    free(line);
    fclose(f_in);   

    return 0;
}

int txtWriter(type_t *type, const list_t *list) {
    char filename[MAX_FILE] = {0,};

    if (type->sortingType == SORT_INC) strcat(filename, "inc_");
    else if (type->sortingType == SORT_DEC) strcat(filename, "dec_");
    else if (type->sortingType == SORT_RAND) strcat(filename, "rand_");
    else return UNKNOWN;

    if (type->dataType == DATA_INT) strcat(filename, "int.txt");
    else if (type->dataType == DATA_DOUBLE) strcat(filename, "double.txt");
    else if (type->dataType == DATA_CHAR) strcat(filename, "char*.txt");
    else return UNKNOWN;

    printf("writing %s\n", filename);

    FILE* f_out = fopen(filename, "w");
    if (list->head == NULL) return 0;

    node_t *node_it;
    for (node_it = list->head; node_it != NULL; node_it = node_it->next) {
        if (type->dataType == DATA_INT) fprintf(f_out, "%s=%d\n", node_it->key, *(int *)(node_it->data));
        else if (type->dataType == DATA_DOUBLE) fprintf(f_out, "%s=%lf\n", node_it->key, *(double *)(node_it->data));
        else if (type->dataType == DATA_CHAR) fprintf(f_out, "%s=%s\n", node_it->key, (char *)(node_it->data));
        else return UNKNOWN;
    }
    fclose(f_out); 

    return 0;
}

void interface(char **argv) {
    list_t list;
    listConstructor(&list);

    type_t type;
    if (txtParser(&type, argv) < 0) printf("ERROR <interface.c::txtParser>: Unknown data or sorting types.\n");

    if (txtReader(&type, &list, argv) < 0) printf("ERROR <interface.c::txtReader>: Unknown data or sorting types.\n");
    sorting(&type, &list);
    if (txtWriter(&type, &list) < 0) printf("ERROR <interface.c::txtWriter>: Unknown data or sorting types.\n");

    listDestructor(&list);
    return;
}

