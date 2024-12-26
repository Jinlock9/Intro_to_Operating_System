#ifndef PLUGIN_H
#define PLUGIN_H

#include "list.h"

#define DATA_CHAR 0
#define DATA_INT 1
#define DATA_DOUBLE 2

#define SORT_RAND 0
#define SORT_INC 1
#define SORT_DEC 2

#define UNKNOWN -1

typedef struct Type {
    int dataType;
    int sortingType;
} type_t;

typedef struct PlugIn {
    void *handle;
    char plugInType[40];
    void (*interface)(char**);
} plugIn_t;

void interface(char **argv);
int plugInLoader(plugIn_t **plugIns);

#endif // PLUGIN_H
