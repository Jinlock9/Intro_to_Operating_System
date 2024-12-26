#ifndef INTERFACE_H
#define INTERFACE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../program/list.h"
#include "../program/plugin.h"
#include "../program/sort.h"

#define MAX_LINE 1024
#define MAX_FILE 100
#define MAX_CLMN 3

int csvReader(list_t **lists, type_t **types, char **argv);
int csvWriter(list_t **lists, type_t **types);

#endif // INTERFACE_H
