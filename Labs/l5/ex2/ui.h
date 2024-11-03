#ifndef UI_H
#define UI_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "list.h"
#include "sort.h"

#define MAX_LINE 2048
#define MAX_TEMP 1024
#define MAX_FILE 20

void parsingType(Type* type, char** argv);

void reader(Type* type, List* list, char** argv);

void writer(Type* type, const List* list);

void command(Type* type, char** argv);

void menu(void);

#endif
