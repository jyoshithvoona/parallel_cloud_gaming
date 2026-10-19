#include <stdio.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int thread_count = 0;
    #pragma omp parallel
    {
        #pragma omp single
        {
            thread_count = omp_get_num_threads();
        }
    }

    printf("Hybrid smoke test: rank %d/%d with %d OpenMP threads\n", rank, size, thread_count);

    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Hybrid smoke test PASSED: thread support level %d (requested %d)\n", provided, MPI_THREAD_FUNNELED);
    }

    MPI_Finalize();
    return 0;
}
