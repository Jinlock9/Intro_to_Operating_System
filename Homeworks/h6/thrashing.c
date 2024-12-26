#include <stdio.h>
#include <stdlib.h>

#define SIZE 1000000  
#define STEP 100   

int main() {
    int *array = (int *)malloc(SIZE * sizeof(int));  

    for (int i = 0; i < SIZE; i += STEP) {
        array[i] = i; 
    }

    printf("Program finished\n");
    free(array);
    return 0;
}