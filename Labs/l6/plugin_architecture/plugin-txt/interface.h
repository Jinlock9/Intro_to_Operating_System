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

int txtParser(type_t *type, char **argv);
int txtReader(type_t *type, list_t *list, char **argv);
int txtWriter(type_t *type, const list_t *list);

#endif // INTERFACE_H
