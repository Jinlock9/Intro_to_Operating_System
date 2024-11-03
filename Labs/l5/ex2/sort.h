#ifndef SORT_H
#define SORT_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"

#define TYPE_CHAR 0
#define TYPE_INT 1
#define TYPE_DOUBLE 2

#define TYPE_RAND 0
#define TYPE_INC 1
#define TYPE_DEC 2

typedef struct Type {
    int dataType;
    int sortingType;
} Type;

void sorting(Type* type, List* list);

#endif
