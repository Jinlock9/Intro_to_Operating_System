#include "ui.h"
#include "sort.h"

int main(int argc, char *argv[]){
    if (argc != 3){
        return 0;
    }

    srand((unsigned int) time(NULL));
    
    Type type;
    parsingType(&type, argv);

    command(&type, argv);

    return 0;
}
