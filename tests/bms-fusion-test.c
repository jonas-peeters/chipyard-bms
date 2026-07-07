// Test program for BMS+branch macro-op fusion (alternative 1b).
//
// Filtered-sum kernel in three variants:
//   v1 branchy, v2 branch-free (czero), v3 adaptive with a fusion-friendly
//   BMS pair: the BMS is 8-byte aligned so the pair never splits a fetch
//   packet, the guarded branch is forced uncompressed (fusion requires a
//   32-bit branch at BMS+4), and the branch-free block ends in a plain `j`
//   (the fused branch itself trains BMH/BPD, no BJR needed).
//
// Build (xbmis clang, pk-hosted):
//   riscv64-unknown-elf-clang --target=riscv64-unknown-elf -O2 -fno-common \
//     -fno-builtin-printf -Wall -march=rv64imafdc_zicond_zicntr_xbmis0p1 \
//     -mabi=lp64d -mcmodel=medlow -menable-experimental-extensions \
//     bms-fusion-test.c -o bms-fusion-test.riscv -lc -fuse-ld=lld -lm -g
//
// Run on a Fused*BoomConfig simulator; +bms_fusion_enable=0 selects the
// legacy behavior on the same binary (v3 then degrades to branchy-with-NOP
// because the BMS's f3 redirect is the fusion one only).

#include <stdio.h>
#include <stdlib.h>

#ifndef ELEMENTS
#define ELEMENTS 2048
#endif
#ifndef BMS_MODIFIER
#define BMS_MODIFIER 19
#endif
#ifndef RANDOM_SEED
#define RANDOM_SEED 0
#endif

#define STR(x) #x
#define XSTR(s) STR(s)

struct Element {
    int value;
    int flag;
};

static unsigned long rdcycle(void) {
    unsigned long c;
    asm volatile("csrr %0, cycle" : "=r"(c));
    return c;
}

// v1: branchy
static void sum_v1(struct Element *elements, long size, long *sum) {
    asm volatile(
        "beqz %[size], end%=\n\t"
        "start%=:"
        "lw a1, 4(%[elements])\n\t"
        "beqz a1, loopend%=\n\t"
        "lw a0, 0(%[elements])\n\t"
        "add %[sum], %[sum], a0\n\t"
        "loopend%=:"
        "addi %[size], %[size], -1\n\t"
        "addi %[elements], %[elements], 8\n\t"
        "bnez %[size], start%=\n"
        "end%=:"
        : [sum] "+r"(*sum), [elements] "+r"(elements), [size] "+r"(size)
        :
        : "a0", "a1", "memory");
}

// v2: branch-free (czero.eqz)
static void sum_v2(struct Element *elements, long size, long *sum) {
    asm volatile(
        "beqz %[size], end%=\n\t"
        "start%=:"
        "lw a0, 0(%[elements])\n\t"
        "lw a1, 4(%[elements])\n\t"
        "czero.eqz a2, a0, a1\n\t"
        "add %[sum], %[sum], a2\n\t"
        "addi %[size], %[size], -1\n\t"
        "addi %[elements], %[elements], 8\n\t"
        "bnez %[size], start%=\n"
        "end%=:"
        : [sum] "+r"(*sum), [elements] "+r"(elements), [size] "+r"(size)
        :
        : "a0", "a1", "a2", "memory");
}

// v3: adaptive, fusion-friendly BMS pair
static void sum_v3(struct Element *elements, long size, long *sum) {
    asm volatile(
        "beqz %[size], end%=\n\t"
        "start%=:"
        "lw a1, 4(%[elements])\n\t"
        ".balign 8\n\t"
        "bms problematicbranch%=,branchfree%=," XSTR(BMS_MODIFIER) "\n"
        "problematicbranch%=:\n\t"
        ".option push\n\t"
        ".option norvc\n\t"
        "beqz a1, loopendtop%=\n\t"
        ".option pop\n\t"
        "lw a0, 0(%[elements])\n\t"
        "add %[sum], %[sum], a0\n\t"
        "loopendtop%=:"
        "addi %[size], %[size], -1\n\t"
        "addi %[elements], %[elements], 8\n\t"
        "bnez %[size], start%=\n\t"
        "j end%=\n"
        "branchfree%=:\n\t"
        "lw a0, 0(%[elements])\n\t"
        "czero.eqz a2, a0, a1\n\t"
        "add %[sum], %[sum], a2\n\t"
        "j loopendtop%=\n"
        "end%=:"
        : [sum] "+r"(*sum), [elements] "+r"(elements), [size] "+r"(size)
        :
        : "a0", "a1", "a2", "memory");
}

int main(void) {
    struct Element *elements = malloc(ELEMENTS * sizeof(struct Element));

    printf("Percentage,Variant,Sum,Cycles\n");
    for (int percentage = 0; percentage <= 100; percentage += 25) {
        for (int variant = 1; variant <= 3; variant++) {
            srand(RANDOM_SEED);
            for (int i = 0; i < ELEMENTS; i++) {
                int flag = i % 2;
                if (i < percentage * ELEMENTS / 100) {
                    flag = rand() % 2;
                }
                elements[i].value = i;
                elements[i].flag = flag;
            }
            long sum = 0;
            unsigned long start = rdcycle();
            switch (variant) {
                case 1: sum_v1(elements, ELEMENTS, &sum); break;
                case 2: sum_v2(elements, ELEMENTS, &sum); break;
                case 3: sum_v3(elements, ELEMENTS, &sum); break;
            }
            unsigned long cycles = rdcycle() - start;
            printf("%d,%d,%ld,%lu\n", percentage, variant, sum, cycles);
        }
    }
    free(elements);
    return 0;
}
