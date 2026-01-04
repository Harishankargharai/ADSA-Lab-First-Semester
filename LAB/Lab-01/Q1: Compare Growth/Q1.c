#include <stdio.h>
#include <math.h>
#include <time.h>

volatile long dummy = 0; // prevent compiler optimization

// Run loop for 'count' iterations
void run_loop(long count) {
    for (long i = 0; i < count; i++) {
        dummy += i % 7; // small operation to waste time
    }
}

// Measure time for a function
void measure(const char *name, long count) {
    clock_t start = clock();
    run_loop(count);
    clock_t end = clock();
    printf("%-15s : %f seconds (iterations ~ %ld)\n",
           name, (double)(end - start) / CLOCKS_PER_SEC, count);
}

int main() {
    long n = 10000; // base input

    // Map each function to a scaled iteration count
    measure("1/n",          1000);
    measure("log n",        10000);
    measure("sqrt n",       20000);
    measure("n^0.51",       50000);
    measure("n",            100000);
    measure("n log n",      200000);
    measure("n^2",          1000000);
    measure("n^2 (alt)",    1200000); // 100n^2+6n
    measure("n^3",          5000000);
    measure("n^(log n)",    20000000);
    measure("3^n",          50000000);

    return 0;
}
