#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define MAX_REGIONS 16
#define SENTINEL_SERVER_ID INT_MAX
#define SCORE_INFINITY INFINITY

typedef enum {
    POLICY_SCORED = 0,
    POLICY_FIRST_FIT = 1
} PolicyType;

typedef enum {
    BACKEND_SEQUENTIAL = 0,
    BACKEND_OPENMP = 1,
    BACKEND_MPI = 2,
    BACKEND_HYBRID = 3
} BackendType;

/* Server resource definition */
typedef struct {
    int id;               /* Global unique ID: 0 to N-1 */
    int region;           /* Geographic region index */
    double total_cpu;     /* Capacity in abstract CPU units */
    double avail_cpu;
    double total_gpu;     /* Capacity in abstract GPU units */
    double avail_gpu;
    double total_ram;     /* Capacity in GB */
    double avail_ram;
    double total_bw;      /* Capacity in Mbps */
    double avail_bw;
} Server;

/* Client request definition */
typedef struct {
    int id;               /* Unique request ID */
    int region;           /* Player geographic region */
    long long arrival_time; /* Arrival timestamp (integer ticks) */
    long long duration;     /* Session duration (integer ticks) */
    double req_cpu;
    double req_gpu;
    double req_ram;
    double req_bw;
    double latency_limit; /* Maximum acceptable network latency (ms) */
} Request;

/* Active gaming session reservation */
typedef struct {
    int request_id;
    int server_id;
    long long end_time;   /* arrival_time + duration */
    double res_cpu;
    double res_gpu;
    double res_ram;
    double res_bw;
} Session;

/* Candidate selection pair (compatible with MPI_DOUBLE_INT) */
typedef struct {
    double score;
    int server_id;
} Candidate;

/* Network delay matrix (region-to-region in ms) */
typedef struct {
    int num_regions;
    double matrix[MAX_REGIONS][MAX_REGIONS];
} LatencyMatrix;

/* Simulation and scheduling metrics */
typedef struct {
    int total_requests;
    int accepted_requests;
    int rejected_requests;
    double scheduling_wall_time; /* Core scheduler timing */
    double setup_time;           /* Data loading and distribution time */
    double avg_latency;          /* Average modeled latency for accepted requests */
    double peak_cpu_util;
    double peak_gpu_util;
    double peak_ram_util;
    double peak_bw_util;
} SimulationMetrics;

#endif /* MODELS_H */
