#include <string.h>
#include "interface.h"

int csvReader(list_t **lists, type_t **types, char **argv) {
    printf("reading %s\n", argv[1]);

    char *line = malloc(sizeof(char) * MAX_LINE);
    memset(line, 0, MAX_LINE);

    char **buf = (char **)malloc(sizeof(char *) * MAX_CLMN);

    FILE *f_in = fopen(argv[1], "r");
    int firstLine = 1;
    int it = 0;
    while (fgets(line, MAX_LINE, f_in) != NULL) {
        char *token = strtok(line, ",\n");
        do {
            buf[it] = strdup(token);  
            it++;
            token = strtok(NULL, ",\n");
        } while (token != NULL);

        it = 0;
        
        for (int i = 0; i < MAX_CLMN; i++) {
            if (firstLine) {
                char *sort = strtok(buf[i], "_");
                char *data = strtok(NULL, "_");
                if (!strcmp(argv[2], "inc")) types[i]->sortingType = SORT_INC;
                else if (!strcmp(argv[2], "dec")) types[i]->sortingType = SORT_DEC;
                else if (!strcmp(argv[2], "rand")) types[i]->sortingType = SORT_RAND;
                else return UNKNOWN;
                if (!strcmp(data, "int")) types[i]->dataType = DATA_INT;
                else if (!strcmp(data, "double")) types[i]->dataType = DATA_DOUBLE;
                else if (!strcmp(data, "char*")) types[i]->dataType = DATA_CHAR;
                else return UNKNOWN;
            }
            else {
                char *str = strtok(buf[i], "=");
                char *key = malloc(sizeof(char) * MAX_LINE);
                memset(key, 0, MAX_LINE);
                strcpy(key, str);
                char *value = strtok(NULL, "=");
                void *data;
                if (types[i]->dataType == DATA_INT) {
                    data = malloc(sizeof(int));
                    *(int *)data = atoi(value);
                } else if (types[i]->dataType == DATA_DOUBLE) {
                    data = malloc(sizeof(double));
                    *(double *)data = atof(value);
                } else if (types[i]->dataType == DATA_CHAR) {
                    data = (char *)malloc(sizeof(char) * MAX_LINE);
                    strcpy(data, value);
                } else return UNKNOWN;

                listInsert(lists[i], key, data);
            }
        }   
        firstLine = 0;
    }

    free(line);
    for (int i = 0; i < MAX_CLMN; i++) {
        free(buf[i]);
    }
    free(buf);
    fclose(f_in);   

    return 0;
}

int csvWriter(list_t **lists, type_t **types) {
    char *filename = "output.csv";

    printf("writing %s\n", filename);

    FILE* f_out = fopen(filename, "w");
    
    for (int i = 0; i < MAX_CLMN; i++) {
        if (types[i]->sortingType == SORT_INC) fprintf(f_out, "inc_");
        else if (types[i]->sortingType == SORT_DEC) fprintf(f_out, "dec_");
        else if (types[i]->sortingType == SORT_RAND) fprintf(f_out, "rand_");
        else return UNKNOWN;

        if (types[i]->dataType == DATA_INT) fprintf(f_out, "int");
        else if (types[i]->dataType == DATA_DOUBLE) fprintf(f_out, "double");
        else if (types[i]->dataType == DATA_CHAR) fprintf(f_out, "char*");
        else return UNKNOWN;

        if (i < MAX_CLMN - 1) fprintf(f_out, ",");
        else fprintf(f_out, "\n");
    }

    node_t *temp[MAX_CLMN * MAX_CLMN];
    int it = 0;

    for (int i = 0; i < MAX_CLMN; i++) {
        node_t *node_it;
        for (node_it = lists[i]->head; node_it != NULL; node_it = node_it->next) {
            temp[it++] = node_it;
        }
    }

    for (int i = 0; i < MAX_CLMN; i++) {
        for (int j = 0; j < MAX_CLMN; j++) {
            if (types[j]->dataType == DATA_INT)
                fprintf(f_out, "%s=%d", temp[i +j*MAX_CLMN]->key, *(int *)(temp[i +j*MAX_CLMN]->data));
            else if (types[j]->dataType == DATA_DOUBLE)
                fprintf(f_out, "%s=%lf", temp[i +j*MAX_CLMN]->key, *(double *)(temp[i +j*MAX_CLMN]->data));
            else if (types[j]->dataType == DATA_CHAR)
                fprintf(f_out, "%s=%s", temp[i +j*MAX_CLMN]->key, (char *)(temp[i +j*MAX_CLMN]->data));
            else
                return UNKNOWN;
            if (j < MAX_CLMN - 1) fprintf(f_out, ",");
            else fprintf(f_out, "\n");
        }
    }

    fclose(f_out);
    return 0;
}


void interface(char **argv) {
    list_t **lists = (list_t **)malloc(sizeof(list_t *) * MAX_CLMN);
    for (int i = 0; i < MAX_CLMN; i++) {
        lists[i] = (list_t *)malloc(sizeof(list_t));
        listConstructor(lists[i]);
    }

    type_t **types = (type_t **)malloc(sizeof(type_t *) * MAX_CLMN);
    for (int i = 0; i < MAX_CLMN; i++) {
        types[i] = (type_t *)malloc(sizeof(type_t));
    }

    if (csvReader(lists, types, argv) < 0) printf("ERROR <interface.c::csvReader>: Unknown data or sorting types.\n");

    for (int i = 0; i < MAX_CLMN; i++) sorting(types[i], lists[i]);
    if (csvWriter(lists, types) < 0) printf("ERROR <interface.c::csvWriter>: Unknown data or sorting types.\n");

    for (int i = 0; i < MAX_CLMN; i++) {
        free(types[i]);
        listDestructor(lists[i]);
        free(lists[i]);
    }
    free(types);
    free(lists);
}
