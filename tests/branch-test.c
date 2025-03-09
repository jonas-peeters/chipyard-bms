#define ITERATIONS 32

unsigned int seed = 0;

unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

int main(void) {

    // Initialize array of random bits
    unsigned int randomBitsArray[ITERATIONS];
    #if defined(PREDICTABLE) && PREDICTABLE == 0
    for (int i = 0; i < ITERATIONS; i++) {
        randomBitsArray[i] = rand();
    }
    #elif defined(PREDICTABLE) && PREDICTABLE == 1
    for (int i = 0; i < ITERATIONS; i++) {
        randomBitsArray[i] = 0;
    }
    #endif

    int count = 0;

    for (int x = 0; x < ITERATIONS; x++) {
        unsigned int randomBits = randomBitsArray[x];
        for (int i = 0; i < 32; i++) {
            #if defined(VARIANT) && VARIANT == 0
            if (randomBits & (1 << i)) {
                count++;
                asm volatile("");
            }
            #elif defined(VARIANT) && VARIANT == 1
            //int shift = (randomBits >> i);
            //int and = shift & 1;
            //count += and;
            asm volatile(
                "srlw a6, %[randomBits], %[i]\n\t" // 4 bytes
                "andi a7, a6, 1\n\t" // 4 bytes
                "add %[count], %[count], a7\n\t" // 2 bytes
                : [count] "+r" (count)
                : [randomBits] "r" (randomBits), [i] "r" (i)
                : "cc"
            );
            #elif defined(VARIANT) && VARIANT == 2
            // RISC-V assembly
            asm volatile(
                "bms 14,32,8\n\t"
                // Shift 1 to the left by i bits
                "li a6, 1\n\t" // Load 1 into a6
                "sll a6, a6, %[i]\n\t" // 4 bytes
                // Extract the i-th bit
                "and a7, %[randomBits], a6\n" // 4 bytes
                "problematicbranch:\n\t"
                "bnez a7,increment\n\t" // 4 bytes
                "jal end\n" // 4 bytes
                "increment:\n\t"
                "addi %[count], %[count], 1\n" // 2 bytes
                "jal end\n" // 4 bytes
                "branchfree:\n\t"
                "srlw a6, %[randomBits], %[i]\n\t" // 4 bytes
                "andi a7, a6, 1\n\t" // 4 bytes
                "add %[count], %[count], a7\n\t" // 2 bytes
                "bjr a7, -28, 8\n\t"
                "end:"
                : [count] "+r" (count)
                : [randomBits] "r" (randomBits), [i] "r" (i)
                : "cc"
            );
            #endif
        }
    }

    //printf("Count: %d\n", count);

    //return count == 16 * (ITERATIONS/32) ? 0 : 1;
    return 0;
}