#include <stdio.h>

int main(void) {

    __uint64_t counter = 0;
    for (int i = 0; i < 100; i++) {
        if (i % 3 == 0) {
            counter += i;
            asm volatile("");
        }
    }
    //printf("Counter: %ld\n", counter);

    return 0;
}