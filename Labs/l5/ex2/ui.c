#include <string.h>
#include "ui.h"

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

void command(Type *type, char **argv) {
    List list;
    listConstructor(&list);

    reader(type, &list, argv);
    sorting(type, &list);
    writer(type, &list);

    listDestructor(&list);
}

void menu(void) {
    printf("=========================================================\n");
    printf("Welcome to ECE4820J Lab 5!\n");
    while (1) {
        printf("--------------------------------------------------------\n"
           "- command: [input file] <sortingType> or [ exit ]\n"
           "- [ input file ]: <sortingType>_<dataType>.txt\n"
           "- <sortingType> : rand, inc, dec\n"
           "- <dataType>    : int, char*, double\n"
           "- [ exit ]      : exit program\n"
           "- sample input 1: rand_int.txt inc\n"
           "- sample input 2: exit\n"
           "--------------------------------------------------------\n"
           "$ ");

        Type type;

        char *argv[3];
        for (int i = 0; i < 3; i++) {
            argv[i] = (char *)malloc(sizeof(char) * MAX_TEMP);
        }
        
        char temp_argv[MAX_TEMP];
        if (fgets(temp_argv, MAX_TEMP, stdin) == NULL) {
            for (int i = 0; i < 3; i++) {
                free(argv[i]);
            }
            printf("=========================================================\n");
            break;
        }

        char *token;
        token = strtok(temp_argv, " \n");
        strcpy(argv[1], token);
        if (!strcmp(argv[1], "exit")) {
            for (int i = 0; i < 3; i++) {
                free(argv[i]);
            }
            printf("=========================================================\n");
            break;
        }
        
        token = strtok(NULL, " \n");
        strcpy(argv[2], token);

        parsingType(&type, argv);
        command(&type, argv);
        printf("Success!!!\n");
        printf("---------------------------------------------------------\n");
        printf("$ ");
        for (int i = 0; i < 3; i++) {
            free(argv[i]);
        }
    }
}
