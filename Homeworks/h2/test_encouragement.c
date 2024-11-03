#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

#define SYS_ENCOURAGEMENT 548

int main() {
    long result = syscall(SYS_ENCOURAGEMENT);

    if (!result) {
        printf("System call succeeded!\n");
    } else {
        printf("System call failed: %ld\n", result);
    }

    return 0;
}
