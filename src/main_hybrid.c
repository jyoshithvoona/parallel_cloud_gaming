#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>
#include <omp.h>
#include "../include/simulator.h"

void print_usage_hybrid(const char *prog) {
    printf("Usage: mpirun -np <ranks> %s [options]\n", prog);
    printf("Options:\n");
    printf("  --servers <file>      Path to servers CSV (required)\n");
    printf("  --requests <file>     Path to requests CSV (required)\n");
    printf("  --latency <file>      Path to latency matrix CSV (default: data/latency_matrix.csv)\n");
    printf("  --policy <scored|first_fit> Allocation policy (default: scored)\n");
    printf("  --threads <n>         Threads per rank (default: 2)\n");
    printf("  --log <file>          Path to output decision log (written by rank 0)\n");
    printf("  --csv <file>          Path to append summary CSV metrics (written by rank 0)\n");
    printf("  --seed <n>            Random seed identifier (default: 0)\n");
    printf("  --repeat <n>          Repetition run index (default: 1)\n");
}

int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const char *servers_path = NULL;
    const char *requests_path = NULL;
    const char *latency_path = "data/latency_matrix.csv";
    const char *log_path = NULL;
    const char *csv_path = NULL;
    PolicyType policy = POLICY_SCORED;
    int threads = 2;
    unsigned int seed = 0;
    int repeat = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--servers") == 0 && i + 1 < argc) {
            servers_path = argv[++i];
        } else if (strcmp(argv[i], "--requests") == 0 && i + 1 < argc) {
            requests_path = argv[++i];
        } else if (strcmp(argv[i], "--latency") == 0 && i + 1 < argc) {
            latency_path = argv[++i];
        } else if (strcmp(argv[i], "--policy") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "first_fit") == 0) policy = POLICY_FIRST_FIT;
            else policy = POLICY_SCORED;
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            threads = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--log") == 0 && i + 1 < argc) {
            log_path = argv[++i];
        } else if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) {
            csv_path = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--repeat") == 0 && i + 1 < argc) {
            repeat = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            if (rank == 0) print_usage_hybrid(argv[0]);
            MPI_Finalize();
            return 0;
        }
    }

    if (!servers_path || !requests_path) {
        if (rank == 0) {
            fprintf(stderr, "Error: --servers and --requests are required.\n");
            print_usage_hybrid(argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    double setup_start = MPI_Wtime();

    int num_servers = 0;
    Server *all_servers = NULL;
    int num_requests = 0;
    Request *requests = NULL;
    LatencyMatrix lat;

    if (rank == 0) {
        all_servers = load_servers(servers_path, &num_servers);
        requests = load_requests(requests_path, &num_requests);
        load_latency_matrix(latency_path, &lat);
    }

    MPI_Bcast(&num_servers, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_requests, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&lat, sizeof(LatencyMatrix), MPI_BYTE, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        all_servers = (Server*)malloc(num_servers * sizeof(Server));
    }
    MPI_Bcast(all_servers, num_servers * sizeof(Server), MPI_BYTE, 0, MPI_COMM_WORLD);

    double setup_time = MPI_Wtime() - setup_start;

    const char *pname = (policy == POLICY_FIRST_FIT) ? "first_fit" : "scored";
    SimulationMetrics metrics = run_hybrid(all_servers, num_servers, requests, num_requests, &lat, policy, rank, size, threads, log_path);

    if (rank == 0) {
        metrics.setup_time = setup_time;
        int total_workers = size * threads;
        print_metrics(&metrics, "Hybrid", pname, num_servers, total_workers);

        if (csv_path) {
            export_metrics_csv(csv_path, "Hybrid", pname, num_servers, num_requests,
                               size, threads, total_workers, seed, repeat,
                               metrics.scheduling_wall_time, metrics.setup_time,
                               metrics.accepted_requests, metrics.rejected_requests,
                               metrics.avg_latency, metrics.peak_cpu_util, metrics.peak_gpu_util);
        }
    }

    if (all_servers) free(all_servers);
    if (rank == 0 && requests) free(requests);

    MPI_Finalize();
    return 0;
}
