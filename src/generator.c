#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/models.h"

#define NUM_REGIONS 4

/* Fixed region-to-region latency matrix in ms */
/* Regions: 0 = NA-East, 1 = NA-West, 2 = EU-Central, 3 = AP-East */
static const double LATENCY_TABLE[NUM_REGIONS][NUM_REGIONS] = {
    { 10.0,  45.0,  85.0, 180.0},
    { 45.0,  12.0, 120.0, 140.0},
    { 85.0, 120.0,  15.0, 210.0},
    {180.0, 140.0, 210.0,  20.0}
};

/* Write regional latency matrix to CSV */
void generate_latency_matrix(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Failed to open latency matrix file");
        exit(1);
    }
    fprintf(f, "region_from,region_to,latency_ms\n");
    for (int i = 0; i < NUM_REGIONS; i++) {
        for (int j = 0; j < NUM_REGIONS; j++) {
            fprintf(f, "%d,%d,%.1f\n", i, j, LATENCY_TABLE[i][j]);
        }
    }
    fclose(f);
    printf("Generated latency matrix: %s\n", filename);
}

/* Generate small deterministic verification dataset */
void generate_small_verification(const char *server_file, const char *req_file) {
    FILE *fs = fopen(server_file, "w");
    if (!fs) { perror("Failed to open server file"); exit(1); }
    fprintf(fs, "id,region,cpu,gpu,ram,bandwidth\n");
    /* 3 servers: Server 0 (Region 0), Server 1 (Region 1), Server 2 (Region 0) */
    fprintf(fs, "0,0,64.0,8.0,128.0,1000.0\n");
    fprintf(fs, "1,1,32.0,4.0,64.0,500.0\n");
    fprintf(fs, "2,0,16.0,2.0,32.0,250.0\n");
    fclose(fs);

    FILE *fr = fopen(req_file, "w");
    if (!fr) { perror("Failed to open requests file"); exit(1); }
    fprintf(fr, "id,region,arrival_time,duration,req_cpu,req_gpu,req_ram,req_bw,latency_limit\n");
    /*
       Req 0: player in region 0, arrival 10, dur 100, moderate demand -> should pick server 0 or 2 based on score
       Req 1: player in region 1, arrival 20, dur 50, fits server 1 best
       Req 2: player in region 3, arrival 30, dur 60, latency limit 15 ms -> impossible (min latency to any region is 140+ ms) -> REJECT
       Req 3: player in region 0, arrival 110, dur 40 -> arrives after Req 0 ends (at 110), verifies resource release!
       Req 4: player in region 0, arrival 120, dur 30, impossible demand (cpu 200) -> resource shortage -> REJECT
    */
    fprintf(fr, "0,0,10,100,16.0,2.0,32.0,200.0,60.0\n");
    fprintf(fr, "1,1,20,50,8.0,1.0,16.0,100.0,50.0\n");
    fprintf(fr, "2,3,30,60,4.0,1.0,8.0,50.0,15.0\n");
    fprintf(fr, "3,0,110,40,32.0,4.0,64.0,400.0,60.0\n");
    fprintf(fr, "4,0,120,30,200.0,20.0,256.0,2000.0,100.0\n");
    fclose(fr);
    printf("Generated verification dataset: %s, %s\n", server_file, req_file);
}

/* Generate synthetic server pool */
void generate_servers(const char *filename, int num_servers, unsigned int seed) {
    srand(seed);
    FILE *f = fopen(filename, "w");
    if (!f) { perror("Failed to open server file"); exit(1); }
    fprintf(f, "id,region,cpu,gpu,ram,bandwidth\n");

    /* Define typical cloud server tiers */
    /* Tier 0: Standard (32 CPU, 4 GPU, 64 GB, 1000 Mbps) */
    /* Tier 1: Performance (64 CPU, 8 GPU, 128 GB, 2500 Mbps) */
    /* Tier 2: High-Density (128 CPU, 16 GPU, 256 GB, 5000 Mbps) */
    for (int i = 0; i < num_servers; i++) {
        int region = i % NUM_REGIONS;
        int tier = rand() % 3;
        double cpu = (tier == 0) ? 32.0 : (tier == 1 ? 64.0 : 128.0);
        double gpu = (tier == 0) ? 4.0  : (tier == 1 ? 8.0  : 16.0);
        double ram = (tier == 0) ? 64.0 : (tier == 1 ? 128.0: 256.0);
        double bw  = (tier == 0) ? 1000.0 : (tier == 1 ? 2500.0 : 5000.0);

        fprintf(f, "%d,%d,%.1f,%.1f,%.1f,%.1f\n", i, region, cpu, gpu, ram, bw);
    }
    fclose(f);
    printf("Generated %d servers: %s (seed: %u)\n", num_servers, filename, seed);
}

/* Workload profile: 0=light, 1=heavy/near-capacity, 2=bursty */
void generate_requests(const char *filename, int num_requests, int workload_type, unsigned int seed) {
    srand(seed);
    FILE *f = fopen(filename, "w");
    if (!f) { perror("Failed to open requests file"); exit(1); }
    fprintf(f, "id,region,arrival_time,duration,req_cpu,req_gpu,req_ram,req_bw,latency_limit\n");

    long long current_time = 0;

    for (int i = 0; i < num_requests; i++) {
        /* Inter-arrival interval */
        int inter_arrival;
        if (workload_type == 0) {
            /* Light load: spread out */
            inter_arrival = 10 + rand() % 20;
        } else if (workload_type == 1) {
            /* Heavy load: rapid arrivals */
            inter_arrival = 1 + rand() % 5;
        } else {
            /* Bursty: alternating bursts and pauses */
            if ((i % 50) < 40) {
                inter_arrival = 1 + rand() % 3; /* Burst phase */
            } else {
                inter_arrival = 25 + rand() % 50; /* Idle phase */
            }
        }
        current_time += inter_arrival;

        /* Duration: 50 to 300 ticks */
        long long duration = 50 + rand() % 250;

        /* Client region */
        int region = rand() % NUM_REGIONS;

        /* Resource demands */
        /* Typical gaming profiles: Casual (2 CPU, 0.5 GPU, 4 GB), AAA (4-8 CPU, 1-2 GPU, 8-16 GB) */
        int profile = rand() % 3;
        double cpu = (profile == 0) ? 2.0 : (profile == 1 ? 4.0 : 8.0);
        double gpu = (profile == 0) ? 0.5 : (profile == 1 ? 1.0 : 2.0);
        double ram = (profile == 0) ? 4.0 : (profile == 1 ? 8.0 : 16.0);
        double bw  = (profile == 0) ? 25.0 : (profile == 1 ? 50.0 : 100.0);

        /* Latency limit: 50ms to 120ms */
        double latency_limit = 50.0 + (rand() % 70);

        fprintf(f, "%d,%d,%lld,%lld,%.1f,%.1f,%.1f,%.1f,%.1f\n",
                i, region, current_time, duration, cpu, gpu, ram, bw, latency_limit);
    }
    fclose(f);
    printf("Generated %d requests (type %d): %s (seed: %u)\n", num_requests, workload_type, filename, seed);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    printf("=== Generating Datasets for Cloud Gaming Simulator ===\n");

    /* 1. Regional Latency Matrix */
    generate_latency_matrix("data/latency_matrix.csv");

    /* 2. Small Verification Dataset */
    generate_small_verification("data/servers_small.csv", "data/requests_small.csv");

    /* 3. Scaled Server Datasets */
    generate_servers("data/servers_100.csv", 100, 1001);
    generate_servers("data/servers_1k.csv", 1000, 1002);
    generate_servers("data/servers_10k.csv", 10000, 1003);

    /* 4. Scaled Request Datasets (1,000 requests) */
    generate_requests("data/requests_light_1k.csv", 1000, 0, 2001);   /* Light */
    generate_requests("data/requests_heavy_1k.csv", 1000, 1, 2002);   /* Heavy */
    generate_requests("data/requests_bursty_1k.csv", 1000, 2, 2003);  /* Bursty */

    /* 5. High-Throughput Request Dataset (10,000 requests) */
    generate_requests("data/requests_10k.csv", 10000, 1, 2004);

    printf("=== All Datasets Successfully Generated ===\n");
    return 0;
}
