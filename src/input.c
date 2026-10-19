#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/simulator.h"

double get_wall_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}

Server* load_servers(const char *filename, int *out_count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open server file %s\n", filename);
        return NULL;
    }

    char line[512];
    /* Skip header */
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return NULL;
    }

    int capacity = 128;
    int count = 0;
    Server *servers = (Server*)malloc(capacity * sizeof(Server));
    if (!servers) {
        fclose(f);
        return NULL;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) <= 1) continue;
        if (count >= capacity) {
            capacity *= 2;
            Server *new_servers = (Server*)realloc(servers, capacity * sizeof(Server));
            if (!new_servers) {
                free(servers);
                fclose(f);
                return NULL;
            }
            servers = new_servers;
        }

        Server s;
        int parsed = sscanf(line, "%d,%d,%lf,%lf,%lf,%lf",
                            &s.id, &s.region, &s.total_cpu, &s.total_gpu, &s.total_ram, &s.total_bw);
        if (parsed == 6) {
            s.avail_cpu = s.total_cpu;
            s.avail_gpu = s.total_gpu;
            s.avail_ram = s.total_ram;
            s.avail_bw  = s.total_bw;
            servers[count++] = s;
        }
    }
    fclose(f);

    *out_count = count;
    return servers;
}

Request* load_requests(const char *filename, int *out_count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open request file %s\n", filename);
        return NULL;
    }

    char line[512];
    /* Skip header */
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return NULL;
    }

    int capacity = 256;
    int count = 0;
    Request *requests = (Request*)malloc(capacity * sizeof(Request));
    if (!requests) {
        fclose(f);
        return NULL;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) <= 1) continue;
        if (count >= capacity) {
            capacity *= 2;
            Request *new_requests = (Request*)realloc(requests, capacity * sizeof(Request));
            if (!new_requests) {
                free(requests);
                fclose(f);
                return NULL;
            }
            requests = new_requests;
        }

        Request r;
        int parsed = sscanf(line, "%d,%d,%lld,%lld,%lf,%lf,%lf,%lf,%lf",
                            &r.id, &r.region, &r.arrival_time, &r.duration,
                            &r.req_cpu, &r.req_gpu, &r.req_ram, &r.req_bw, &r.latency_limit);
        if (parsed == 9) {
            requests[count++] = r;
        }
    }
    fclose(f);

    *out_count = count;
    return requests;
}

bool load_latency_matrix(const char *filename, LatencyMatrix *out_matrix) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: cannot open latency matrix file %s\n", filename);
        return false;
    }

    /* Initialize to 0 */
    memset(out_matrix, 0, sizeof(LatencyMatrix));
    int max_region = 0;

    char line[256];
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return false;
    }

    while (fgets(line, sizeof(line), f)) {
        if (strlen(line) <= 1) continue;
        int from, to;
        double lat;
        if (sscanf(line, "%d,%d,%lf", &from, &to, &lat) == 3) {
            if (from < MAX_REGIONS && to < MAX_REGIONS) {
                out_matrix->matrix[from][to] = lat;
                if (from > max_region) max_region = from;
                if (to > max_region) max_region = to;
            }
        }
    }
    fclose(f);
    out_matrix->num_regions = max_region + 1;
    return true;
}

void free_servers(Server *servers) {
    if (servers) free(servers);
}

void free_requests(Request *requests) {
    if (requests) free(requests);
}
