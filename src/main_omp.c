#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include "../include/simulator.h"

void print_usage_omp(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  --servers <file>      Path to servers CSV (required)\n");
    printf("  --requests <file>     Path to requests CSV (required)\n");
    printf("  --latency <file>      Path to latency matrix CSV (default: data/latency_matrix.csv)\n");
    printf("  --policy <scored|first_fit> Allocation policy (default: scored)\n");
    printf("  --threads <n>         Number of OpenMP threads (default: from OMP_NUM_THREADS or 4)\n");
    printf("  --log <file>          Path to output decision log\n");
    printf("  --csv <file>          Path to append summary CSV metrics\n");
    printf("  --seed <n>            Random seed identifier (default: 0)\n");
    printf("  --repeat <n>          Repetition run index (default: 1)\n");
}

int main(int argc, char **argv) {
    const char *servers_path = NULL;
    const char *requests_path = NULL;
    const char *latency_path = "data/latency_matrix.csv";
    const char *log_path = NULL;
    const char *csv_path = NULL;
    PolicyType policy = POLICY_SCORED;
    int threads = 0;
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
            print_usage_omp(argv[0]);
            return 0;
        }
    }

    if (!servers_path || !requests_path) {
        fprintf(stderr, "Error: --servers and --requests are required.\n");
        print_usage_omp(argv[0]);
        return 1;
    }

    if (threads <= 0) {
        char *env_threads = getenv("OMP_NUM_THREADS");
        if (env_threads) {
            threads = atoi(env_threads);
        } else {
            threads = 4;
        }
    }
    omp_set_num_threads(threads);

    double setup_start = omp_get_wtime();

    int num_servers = 0;
    Server *servers = load_servers(servers_path, &num_servers);
    if (!servers) return 1;

    int num_requests = 0;
    Request *requests = load_requests(requests_path, &num_requests);
    if (!requests) {
        free_servers(servers);
        return 1;
    }

    LatencyMatrix lat;
    if (!load_latency_matrix(latency_path, &lat)) {
        free_servers(servers);
        free_requests(requests);
        return 1;
    }

    double setup_time = omp_get_wtime() - setup_start;

    const char *pname = (policy == POLICY_FIRST_FIT) ? "first_fit" : "scored";
    SimulationMetrics metrics = run_openmp(servers, num_servers, requests, num_requests, &lat, policy, threads, log_path);
    metrics.setup_time = setup_time;

    print_metrics(&metrics, "OpenMP", pname, num_servers, threads);

    if (csv_path) {
        export_metrics_csv(csv_path, "OpenMP", pname, num_servers, num_requests,
                           1, threads, threads, seed, repeat,
                           metrics.scheduling_wall_time, metrics.setup_time,
                           metrics.accepted_requests, metrics.rejected_requests,
                           metrics.avg_latency, metrics.peak_cpu_util, metrics.peak_gpu_util);
    }

    free_servers(servers);
    free_requests(requests);
    return 0;
}
