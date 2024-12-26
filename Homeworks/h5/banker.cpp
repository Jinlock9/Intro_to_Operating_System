#include <iostream>

#define NUM_PROC 5
#define NUM_RSRC 3

int main(void) {
    // DEMO INPUTS ************************/
    int mat_M[NUM_PROC][NUM_RSRC] = {
        // Maximum Matrix
        {7, 5, 3},
        {3, 2, 2},
        {9, 0, 2},
        {2, 2, 2},
        {4, 3, 3}
    };
    int mat_A[NUM_PROC][NUM_RSRC] = {
        // Allocation Matrix
        {0, 1, 0},
        {2, 0, 0},
        {3, 0, 2},
        {2, 1, 1},
        {0, 0, 2}
    };
    int available[NUM_RSRC] = {3, 3, 2};
    /**************************************/

    int mat_R[NUM_PROC][NUM_RSRC]; // Request Matrix
    for (int i = 0; i < NUM_PROC; i++) {
        for (int j = 0; j < NUM_RSRC; j++) {
            mat_R[i][j] = mat_M[i][j] - mat_A[i][j];
        }
    }

    int executed[NUM_PROC] = {0};

    int flag = 1;
    int safe = 0;
    while (flag) {
        flag = 0;
        for (int i = 0; i < NUM_PROC; i++) {
            if (!executed[i]) {
                int execute = 1;
                for (int j = 0; j < NUM_RSRC; j++) {
                    if (mat_R[i][j] > available[j]) execute = 0;
                }
                if (execute) {
                    flag = 1;
                    safe++;
                    executed[i] = 1;
                    for (int k = 0; k < NUM_RSRC; k++) {
                        available[k] += mat_A[i][k];
                    }
                }
            }
        }
    }

    if (safe == NUM_PROC) std::cout << "The system is in a SAFE state." << std::endl;
    else std::cout << "The system is in an UNSAFE state." << std::endl;

    return 0;
}
