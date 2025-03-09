#define ELEMENTS 32

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void sort_indexes(int array[], int *i1, int *i2) {
    int x = *i1;
    int y = *i2;
    int c = array[*i1] <= array[*i2];
    *i1 = c ? x : y;
    *i2 = c ? y : x;
}

void insertionSort(int array[], int n) {
    int i, element, j;
    for (i = 1; i < n; i++) {
        element = array[i];
        j = i - 1;

        while (j >= 0 && array[j] > element) {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = element;
    }
}

int median_of_three(int array[], int low, int high) {
  if (high - low < 2) {
    return array[high];
  }

  int l = low;
  int m = (low + high) / 2;
  int h = high;

  sort_indexes(array, &l, &m);
  sort_indexes(array, &m, &h);
  sort_indexes(array, &l, &m);

  swap(&array[m], &array[high]);

  return array[high];
}

//#if defined(VARIANT) && VARIANT == 0
int partition_branch(int array[], int low, int high) {
    int pivot = median_of_three(array, low, high);

    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[high]);
    return (i + 1);
}
/*#elif defined(VARIANT) && VARIANT == 1
int partition_branchfree(int array[], int low, int high) {
    int pivot = median_of_three(array, low, high);

    int i = low;
    for (int j = low; j < high; j++) {
        int c = pivot > array[j];
        int y = array[i];
        int z = array[j];
        array[i] = c ? z : y; // Done using CMOVgl
        array[j] = c ? y : z; // Done using CMOVgl
        i += c;
    }
    swap(&array[i], &array[high]);
    return (i);
}
#elif defined(VARIANT) && VARIANT == 2
int partition_bms(int array[], int low, int high) {
    int pivot = median_of_three(array, low, high);

    int i = low - 1;
    asm volatile(
        // Loop start
        "li a1, 0\n\t"
        "bge a1, %[high], loop_end%=\n"
        "loop%=:\n\t"
        // Get the array element at index a1
        "slli a2, a1, 2\n\t"
        "add a2, a2, %[array]\n\t"
        "lw a3, 0(a2)\n"
        "bms 4,32,8\n\t"
        // Branchy variant
        "problematicbranch%=:\n\t"
        // Compare the array element with the pivot
        "blt a3, %[pivot], loop_increment%=\n\t"
        // Swap the array element with the element at index i
        "slli a6, %[i], 2\n\t"
        "add a6, a6, %[array]\n\t"
        "lw a7, 0(a6)\n\t"
        "sw a7, 0(a2)\n\t"
        "sw a3, 0(a6)\n"
        // Increase i
        "addi %[i], %[i], 1\n\t"
        "jal loop_end%=\n"
        // Branch free variant
        "branchfree%=:\n\t"
        // Load the array element at index i
        "slli a6, %[i], 2\n\t"
        "add a6, a6, %[array]\n\t"
        "lw a7, 0(a6)\n\t"
        "sltu a5, a2, %[pivot]\n\t"
        "bjr a5, -42, 76\n\t"
        // Write from index a1 to index i when a2 < pivot
        "czero.eqz a4, a3, a5\n\t"
        "czero.nez t4, a7, a5\n\t"
        "or a4, a4, t4\n\t"
        "sw a4, 0(a2)\n\t"
        // Write from index i to index a1 when a2 < pivot
        "czero.eqz t1, a7, a5\n\t"
        "czero.nez t2, a3, a5\n\t"
        "or t3, t1, t2\n\t"
        "sw t3, 0(a6)\n\t"
        // Increase a1 for the next iteration
        "loop_increment%=:\n\t"
        "addi a1, a1, 1\n\t"
        "blt a1, %[high], loop%=\n\t"
        "loop_end%=:\n\t"
        : [i] "+r" (i)
        : [array] "r" (array), [pivot] "r" (pivot), [high] "r" (high)
        : "cc"
    );
    swap(&array[i], &array[high]);
    return (i + 1);
}
#endif
*/
void quicksort(int array[], int low, int high) {

    if (low < high) {
        //if (high - low > 5) {
            int pivot;
            //#if defined(VARIANT) && VARIANT == 0
            pivot = partition_branch(array, low, high);
            /*#elif defined(VARIANT) && VARIANT == 1
            pivot = partition_branchfree(array, low, high);
            #elif defined(VARIANT) && VARIANT == 2
            pivot = partition_bms(array, low, high);
            #endif*/

            quicksort(array, low, pivot - 1);
            quicksort(array, pivot + 1, high);
        //} else {
        //    insertionSort(array + low, high - low + 1);
        //}
    }
}

int main(void) {
    int arrayToSort[ELEMENTS];
    // Initialize array of crandom bits
    //#if defined(PREDICTABLE) && PREDICTABLE == 0
    unsigned int seed = 0;
    for (int i = 0; i < ELEMENTS; i++) {
        seed = (seed * 1103515245 + 12345) & 0xffffffff;
        arrayToSort[i] = seed % 100000;
    }
    //#elif defined(PREDICTABLE) && PREDICTABLE == 1
    //for (int i = 0; i < ELEMENTS; i++) {
    //    arrayToSort[i] = ELEMENTS - i - 1;
    //}
    //for (int i = 0; i < ELEMENTS; i++) {
    //    int target = arrayToSort[i] * 7 % ELEMENTS;
    //    swap(&arrayToSort[i], &arrayToSort[target]);
    //}
    //#endif

    arrayToSort[8] = 0;

    quicksort(arrayToSort, 0, ELEMENTS - 1);

    return arrayToSort[0];
}