#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Monotonic wall clock */
double get_wall_time(void);

/* I/O routines */
Server* load_servers(const char *filename, int *out_count);
Request* load_requests(const char *filename, int *out_count);
bool load_latency_matrix(const char *filename, LatencyMatrix *out_matrix);
void free_servers(Server *servers);
void free_requests(Request *requests);

/* Scoring & Candidate comparison */
bool is_server_feasible(const Server *s, const Request *r, double latency);
double calculate_server_score(const Server *s, const Request *r, double latency);
Candidate evaluate_server(const Server *s, const Request *r, double latency, PolicyType policy);
Candidate compare_candidates(Candidate a, Candidate b);

/* Session & Event Engine (Min-heap) */
typedef struct EventHeap EventHeap;
EventHeap* event_heap_create(int initial_capacity);
void event_heap_destroy(EventHeap *heap);
void event_heap_push(EventHeap *heap, Session session);
int event_heap_release_expired(EventHeap *heap, long long current_time, Server *servers, int server_count, int global_id_offset);
void event_heap_drain_all(EventHeap *heap, Server *servers, int server_count, int global_id_offset);
bool verify_capacities_restored(const Server *current, const Server *initial, int count);

/* Allocation & Reservation */
void reserve_server_resources(Server *s, const Request *r);
void release_server_resources(Server *s, const Session *sess);

/* Backend Runner Interfaces */
SimulationMetrics run_sequential(Server *servers, int num_servers,
                                 const Request *requests, int num_requests,
                                 const LatencyMatrix *lat, PolicyType policy,
                                 const char *decision_log_path);

SimulationMetrics run_openmp(Server *servers, int num_servers,
                             const Request *requests, int num_requests,
                             const LatencyMatrix *lat, PolicyType policy,
                             int num_threads, const char *decision_log_path);

SimulationMetrics run_mpi(Server *all_servers, int total_servers,
                          const Request *requests, int num_requests,
                          const LatencyMatrix *lat, PolicyType policy,
                          int rank, int size, const char *decision_log_path);

SimulationMetrics run_hybrid(Server *all_servers, int total_servers,
                            const Request *requests, int num_requests,
                            const LatencyMatrix *lat, PolicyType policy,
                            int rank, int size, int num_threads,
                            const char *decision_log_path);

/* Metrics reporting and export */
void print_metrics(const SimulationMetrics *m, const char *backend_name, const char *policy_name, int servers, int workers);
void export_metrics_csv(const char *csv_path, const char *backend, const char *policy,
                        int servers, int requests, int ranks, int threads, int total_workers,
                        unsigned int seed, int repeat,
                        double core_seconds, double setup_seconds,
                        int accepted, int rejected,
                        double avg_latency, double peak_cpu, double peak_gpu);

#ifdef __cplusplus
}
#endif

#endif /* SIMULATOR_H */
