#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include "../include/simulator.h"

SimulationMetrics run_mpi(Server *all_servers, int total_servers,
                          const Request *requests, int num_requests,
                          const LatencyMatrix *lat, PolicyType policy,
                          int rank, int size, const char *decision_log_path) {
    SimulationMetrics metrics;
    memset(&metrics, 0, sizeof(SimulationMetrics));
    metrics.total_requests = num_requests;

    /* Compute contiguous partition for this rank */
    int base = total_servers / size;
    int remainder = total_servers % size;
    int my_count = base + (rank < remainder ? 1 : 0);
    int my_start = rank * base + (rank < remainder ? rank : remainder);

    /* Allocate local partition of servers */
    Server *my_servers = NULL;
    if (my_count > 0) {
        my_servers = (Server*)malloc(my_count * sizeof(Server));
        for (int i = 0; i < my_count; i++) {
            my_servers[i] = all_servers[my_start + i];
        }
    }

    FILE *log_file = NULL;
    if (rank == 0 && decision_log_path) {
        log_file = fopen(decision_log_path, "w");
        if (log_file) {
            fprintf(log_file, "request_id,status,server_id,score\n");
        }
    }

    EventHeap *heap = event_heap_create(num_requests);

    double total_latency_sum = 0.0;

    /* Synchronize before timing core simulation interval */
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    for (int r = 0; r < num_requests; r++) {
        Request req;
        if (rank == 0) {
            req = requests[r];
        }

        /* Collective Step 1: Broadcast request from rank 0 */
        MPI_Bcast(&req, sizeof(Request), MPI_BYTE, 0, MPI_COMM_WORLD);

        /* Step 2: Release expired local sessions */
        if (my_count > 0) {
            event_heap_release_expired(heap, req.arrival_time, my_servers, my_count, my_start);
        }

        /* Step 3: Scan local partition for best candidate */
        Candidate local_best = { .score = SCORE_INFINITY, .server_id = SENTINEL_SERVER_ID };
        for (int i = 0; i < my_count; i++) {
            double latency = lat->matrix[req.region][my_servers[i].region];
            Candidate cand = evaluate_server(&my_servers[i], &req, latency, policy);
            local_best = compare_candidates(local_best, cand);
        }

        /* Collective Step 4: Reduce global winner across all ranks */
        Candidate global_best;
        MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        /* Step 5: Update state on owning rank */
        if (global_best.server_id != SENTINEL_SERVER_ID) {
            if (rank == 0) metrics.accepted_requests++;

            int sid = global_best.server_id;
            /* Check if this rank owns the winning server */
            if (sid >= my_start && sid < my_start + my_count) {
                int local_idx = sid - my_start;
                reserve_server_resources(&my_servers[local_idx], &req);

                Session sess;
                sess.request_id = req.id;
                sess.server_id = sid;
                sess.end_time = req.arrival_time + req.duration;
                sess.res_cpu = req.req_cpu;
                sess.res_gpu = req.req_gpu;
                sess.res_ram = req.req_ram;
                sess.res_bw  = req.req_bw;
                event_heap_push(heap, sess);
            }

            if (rank == 0) {
                double latency = lat->matrix[req.region][all_servers[sid].region];
                total_latency_sum += latency;
                if (log_file) {
                    fprintf(log_file, "%d,ACCEPTED,%d,%.6f\n", req.id, sid, global_best.score);
                }
            }
        } else {
            if (rank == 0) {
                metrics.rejected_requests++;
                if (log_file) {
                    fprintf(log_file, "%d,REJECTED,-1,inf\n", req.id);
                }
            }
        }
    }

    double local_elapsed = MPI_Wtime() - start_time;
    double max_elapsed = 0.0;
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        metrics.scheduling_wall_time = max_elapsed;
        if (metrics.accepted_requests > 0) {
            metrics.avg_latency = total_latency_sum / metrics.accepted_requests;
        }
    }

    /* Cleanup and drain remaining local sessions */
    if (my_count > 0) {
        event_heap_drain_all(heap, my_servers, my_count, my_start);
        free(my_servers);
    }
    event_heap_destroy(heap);

    if (rank == 0 && log_file) {
        fclose(log_file);
    }

    return metrics;
}
