#include <stdio.h>
#include <omp.h>

int main(void) {
    int total_threads = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
            total_threads = omp_get_num_threads();
        }
    }
    printf("OpenMP smoke test PASSED: running with %d threads\n", total_threads);
    return 0;
}
