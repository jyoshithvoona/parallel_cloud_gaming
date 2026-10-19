#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#include "../include/simulator.h"

void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  --servers <file>      Path to servers CSV (required)\n");
    printf("  --requests <file>     Path to requests CSV (required)\n");
    printf("  --latency <file>      Path to latency matrix CSV (default: data/latency_matrix.csv)\n");
    printf("  --policy <scored|first_fit> Allocation policy (default: scored)\n");
    printf("  --backend <seq|omp>   Execution backend (default: seq)\n");
    printf("  --threads <n>         Number of OpenMP threads (default: 1)\n");
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
    BackendType backend = BACKEND_SEQUENTIAL;
    int threads = 1;
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
        } else if (strcmp(argv[i], "--backend") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "omp") == 0) backend = BACKEND_OPENMP;
            else backend = BACKEND_SEQUENTIAL;
        } else if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            threads = atoi(argv[++i]);
            if (threads > 1) backend = BACKEND_OPENMP;
        } else if (strcmp(argv[i], "--log") == 0 && i + 1 < argc) {
            log_path = argv[++i];
        } else if (strcmp(argv[i], "--csv") == 0 && i + 1 < argc) {
            csv_path = argv[++i];
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)atoi(argv[++i]);
        } else if (strcmp(argv[i], "--repeat") == 0 && i + 1 < argc) {
            repeat = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    if (!servers_path || !requests_path) {
        fprintf(stderr, "Error: --servers and --requests are required.\n");
        print_usage(argv[0]);
        return 1;
    }

    double setup_start = get_wall_time();

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

    double setup_time = get_wall_time() - setup_start;

    SimulationMetrics metrics;
    const char *bname = (backend == BACKEND_OPENMP) ? "OpenMP" : "Sequential";
    const char *pname = (policy == POLICY_FIRST_FIT) ? "first_fit" : "scored";

    if (backend == BACKEND_OPENMP) {
        metrics = run_openmp(servers, num_servers, requests, num_requests, &lat, policy, threads, log_path);
    } else {
        metrics = run_sequential(servers, num_servers, requests, num_requests, &lat, policy, log_path);
        threads = 1;
    }

    metrics.setup_time = setup_time;

    print_metrics(&metrics, bname, pname, num_servers, threads);

    if (csv_path) {
        export_metrics_csv(csv_path, bname, pname, num_servers, num_requests,
                           1, threads, threads, seed, repeat,
                           metrics.scheduling_wall_time, metrics.setup_time,
                           metrics.accepted_requests, metrics.rejected_requests,
                           metrics.avg_latency, metrics.peak_cpu_util, metrics.peak_gpu_util);
    }

    free_servers(servers);
    free_requests(requests);
    return 0;
}
