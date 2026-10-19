#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/simulator.h"

void print_metrics(const SimulationMetrics *m, const char *backend_name, const char *policy_name, int servers, int workers) {
    printf("============================================================\n");
    printf(" Simulation Results: %s Backend (%s Policy)\n", backend_name, policy_name);
    printf("------------------------------------------------------------\n");
    printf(" Total Servers:           %d\n", servers);
    printf(" Total Requests:          %d\n", m->total_requests);
    printf(" Total Workers:           %d\n", workers);
    printf(" Accepted Requests:       %d (%.2f%%)\n",
           m->accepted_requests, (double)m->accepted_requests / m->total_requests * 100.0);
    printf(" Rejected Requests:       %d (%.2f%%)\n",
           m->rejected_requests, (double)m->rejected_requests / m->total_requests * 100.0);
    printf(" Avg Modeled Latency:     %.2f ms\n", m->avg_latency);
    printf(" Peak Resource Util:      CPU: %.1f%% | GPU: %.1f%% | RAM: %.1f%% | BW: %.1f%%\n",
           m->peak_cpu_util * 100.0, m->peak_gpu_util * 100.0,
           m->peak_ram_util * 100.0, m->peak_bw_util * 100.0);
    printf(" Core Scheduler Time:     %.6f seconds\n", m->scheduling_wall_time);
    printf(" Setup & Data Load Time:  %.6f seconds\n", m->setup_time);
    printf("============================================================\n");
}

void export_metrics_csv(const char *csv_path, const char *backend, const char *policy,
                        int servers, int requests, int ranks, int threads, int total_workers,
                        unsigned int seed, int repeat,
                        double core_seconds, double setup_seconds,
                        int accepted, int rejected,
                        double avg_latency, double peak_cpu, double peak_gpu) {
    bool file_exists = false;
    FILE *check = fopen(csv_path, "r");
    if (check) {
        file_exists = true;
        fclose(check);
    }

    FILE *f = fopen(csv_path, "a");
    if (!f) {
        fprintf(stderr, "Warning: failed to write metrics to %s\n", csv_path);
        return;
    }

    if (!file_exists) {
        fprintf(f, "backend,policy,servers,requests,ranks,threads,total_workers,seed,repeat,core_seconds,setup_seconds,accepted,rejected,avg_latency_ms,peak_cpu_pct,peak_gpu_pct\n");
    }

    fprintf(f, "%s,%s,%d,%d,%d,%d,%d,%u,%d,%.6f,%.6f,%d,%d,%.2f,%.2f,%.2f\n",
            backend, policy, servers, requests, ranks, threads, total_workers,
            seed, repeat, core_seconds, setup_seconds, accepted, rejected,
            avg_latency, peak_cpu * 100.0, peak_gpu * 100.0);
    fclose(f);
}
