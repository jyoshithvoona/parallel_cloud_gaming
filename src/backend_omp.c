#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include "../include/simulator.h"

SimulationMetrics run_openmp(Server *servers, int num_servers,
                             const Request *requests, int num_requests,
                             const LatencyMatrix *lat, PolicyType policy,
                             int num_threads, const char *decision_log_path) {
    SimulationMetrics metrics;
    memset(&metrics, 0, sizeof(SimulationMetrics));
    metrics.total_requests = num_requests;

    if (num_threads < 1) num_threads = 1;

    FILE *log_file = NULL;
    if (decision_log_path) {
        log_file = fopen(decision_log_path, "w");
        if (log_file) {
            fprintf(log_file, "request_id,status,server_id,score\n");
        }
    }

    EventHeap *heap = event_heap_create(num_requests);

    double total_cpu_cap = 0, total_gpu_cap = 0, total_ram_cap = 0, total_bw_cap = 0;
    for (int i = 0; i < num_servers; i++) {
        total_cpu_cap += servers[i].total_cpu;
        total_gpu_cap += servers[i].total_gpu;
        total_ram_cap += servers[i].total_ram;
        total_bw_cap  += servers[i].total_bw;
    }

    double total_latency_sum = 0.0;
    Candidate *thread_bests = (Candidate*)malloc(num_threads * sizeof(Candidate));

    double start_time = omp_get_wtime();

    for (int r = 0; r < num_requests; r++) {
        const Request *req = &requests[r];

        /* Step 1: Release expired sessions (single-threaded) */
        event_heap_release_expired(heap, req->arrival_time, servers, num_servers, 0);

        /* Step 2: Parallel candidate search across threads */
        #pragma omp parallel num_threads(num_threads)
        {
            int tid = omp_get_thread_num();
            Candidate local_best = { .score = SCORE_INFINITY, .server_id = SENTINEL_SERVER_ID };

            #pragma omp for schedule(static)
            for (int i = 0; i < num_servers; i++) {
                double latency = lat->matrix[req->region][servers[i].region];
                Candidate cand = evaluate_server(&servers[i], req, latency, policy);
                local_best = compare_candidates(local_best, cand);
            }
            thread_bests[tid] = local_best;
        }

        /* Step 3: Merge thread-local candidates deterministically */
        Candidate best_cand = { .score = SCORE_INFINITY, .server_id = SENTINEL_SERVER_ID };
        for (int t = 0; t < num_threads; t++) {
            best_cand = compare_candidates(best_cand, thread_bests[t]);
        }

        /* Step 4: Reservation or Rejection (single-threaded) */
        if (best_cand.server_id != SENTINEL_SERVER_ID) {
            metrics.accepted_requests++;
            int sid = best_cand.server_id;
            reserve_server_resources(&servers[sid], req);

            Session sess;
            sess.request_id = req->id;
            sess.server_id = sid;
            sess.end_time = req->arrival_time + req->duration;
            sess.res_cpu = req->req_cpu;
            sess.res_gpu = req->req_gpu;
            sess.res_ram = req->req_ram;
            sess.res_bw  = req->req_bw;
            event_heap_push(heap, sess);

            double latency = lat->matrix[req->region][servers[sid].region];
            total_latency_sum += latency;

            if (log_file) {
                fprintf(log_file, "%d,ACCEPTED,%d,%.6f\n", req->id, sid, best_cand.score);
            }
        } else {
            metrics.rejected_requests++;
            if (log_file) {
                fprintf(log_file, "%d,REJECTED,-1,inf\n", req->id);
            }
        }

        /* Track peak utilization */
        double used_cpu = 0, used_gpu = 0, used_ram = 0, used_bw = 0;
        for (int i = 0; i < num_servers; i++) {
            used_cpu += (servers[i].total_cpu - servers[i].avail_cpu);
            used_gpu += (servers[i].total_gpu - servers[i].avail_gpu);
            used_ram += (servers[i].total_ram - servers[i].avail_ram);
            used_bw  += (servers[i].total_bw  - servers[i].avail_bw);
        }
        if (total_cpu_cap > 0 && (used_cpu / total_cpu_cap) > metrics.peak_cpu_util)
            metrics.peak_cpu_util = used_cpu / total_cpu_cap;
        if (total_gpu_cap > 0 && (used_gpu / total_gpu_cap) > metrics.peak_gpu_util)
            metrics.peak_gpu_util = used_gpu / total_gpu_cap;
        if (total_ram_cap > 0 && (used_ram / total_ram_cap) > metrics.peak_ram_util)
            metrics.peak_ram_util = used_ram / total_ram_cap;
        if (total_bw_cap > 0 && (used_bw / total_bw_cap) > metrics.peak_bw_util)
            metrics.peak_bw_util = used_bw / total_bw_cap;
    }

    double end_time = omp_get_wtime();
    metrics.scheduling_wall_time = end_time - start_time;

    if (metrics.accepted_requests > 0) {
        metrics.avg_latency = total_latency_sum / metrics.accepted_requests;
    }

    /* Cleanup */
    free(thread_bests);
    event_heap_drain_all(heap, servers, num_servers, 0);
    event_heap_destroy(heap);

    if (log_file) fclose(log_file);

    return metrics;
}
