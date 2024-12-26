#include <stdio.h>
#include <string.h>
#include <dlfcn.h> 
#include <dirent.h> 

#include "plugin.h"

int plugInLoader(plugIn_t **plugIns) {
    DIR *dir;
    struct dirent *entry;

    int plugInNum = 0;
    if ((dir = opendir("./plugins/")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            plugIns[plugInNum] = (plugIn_t *) malloc(sizeof(plugIn_t));
            void *plugIn;

            char temp[256];
            strcpy(temp, "./plugins/");
            strcat(temp, entry->d_name);
            strcpy(entry->d_name, temp);

            if ((plugIn = dlopen(entry->d_name, RTLD_LAZY)) == NULL) {
                if (!strcmp(entry->d_name, "./plugins/.") || !strcmp(entry->d_name, "./plugins/..")) continue;
                continue;
                return -1;
            }
            if (!strcmp(entry->d_name, "./plugins/.") || !strcmp(entry->d_name, "./plugins/..")) continue;

            *(void **) (&(plugIns[plugInNum]->interface)) = dlsym(plugIn, "interface");
            if(dlerror() != NULL) return -1;
            
            char token[40];
            strcpy(token, entry->d_name);
            char *plugInType = strtok(token, ".");
            plugInType = strtok(plugInType, "/");
            plugInType = strtok(NULL, "/");
            strcpy(plugIns[plugInNum]->plugInType, plugInType);
            plugIns[plugInNum]->handle = plugIn;
            plugInNum++;
        }
    }
    else {
        return -1;
    }

    return plugInNum;
}