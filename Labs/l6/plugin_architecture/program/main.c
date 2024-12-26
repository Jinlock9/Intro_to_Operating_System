#include <dlfcn.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "sort.h"
#include "plugin.h"

void getPlugInType(char *plugInType, char **argv);

int main(int argc, char **argv){
    if (argc != 3) return -1;

    srand((unsigned int) time(NULL));

    plugIn_t **plugIns = (plugIn_t **)malloc(sizeof(plugIn_t *) * 10);
    int plugInNum = plugInLoader(plugIns);
    if (plugInNum < 0) {
        printf("ERROR <main.c::plugInLoader>: Failed to load plug-ins.\n");
    } else {
        char plugInType[36];
        printf("MAIN::GET-PLUG-IN-TYPE\n");
        getPlugInType(plugInType, argv);
        printf("MAIN::PLUG-IN-TYPE::%s\n", plugInType);
        for (int i = 0; i < plugInNum; i++) {
            if (!strcmp(plugInType, plugIns[i]->plugInType)) {
                plugIns[i]->interface(argv);
                dlclose(plugIns[i]->handle);
                break;
            }
        }
    }

    for (int i = 0; i < plugInNum; i++) {
        if(plugIns[i] != NULL) free(plugIns[i]);
    }

    return 0;
}

void getPlugInType(char *plugInType, char **argv) {
    char filename[100];
    strcpy(filename, argv[1]);
    char *token = strtok(filename, ".");
    strcpy(plugInType, strtok(NULL, "."));
}
