#include <stdio.h>
#include <math.h>
#include "../include/simulator.h"

bool is_server_feasible(const Server *s, const Request *r, double latency) {
    if (latency > r->latency_limit) return false;
    if (s->avail_cpu < r->req_cpu) return false;
    if (s->avail_gpu < r->req_gpu) return false;
    if (s->avail_ram < r->req_ram) return false;
    if (s->avail_bw  < r->req_bw)  return false;
    return true;
}

double calculate_server_score(const Server *s, const Request *r, double latency) {
    /* L = modeled_latency / request_latency_limit */
    double L = latency / r->latency_limit;

    /* Resource fraction after allocating request */
    double u_cpu = (s->total_cpu - s->avail_cpu + r->req_cpu) / s->total_cpu;
    double u_gpu = (s->total_gpu - s->avail_gpu + r->req_gpu) / s->total_gpu;
    double u_ram = (s->total_ram - s->avail_ram + r->req_ram) / s->total_ram;
    double u_bw  = (s->total_bw  - s->avail_bw  + r->req_bw)  / s->total_bw;

    /* U = max of all resource fractions */
    double U = u_cpu;
    if (u_gpu > U) U = u_gpu;
    if (u_ram > U) U = u_ram;
    if (u_bw  > U) U = u_bw;

    /* Score = 0.5 * L + 0.5 * U */
    return 0.5 * L + 0.5 * U;
}

Candidate evaluate_server(const Server *s, const Request *r, double latency, PolicyType policy) {
    Candidate cand;
    if (!is_server_feasible(s, r, latency)) {
        cand.score = SCORE_INFINITY;
        cand.server_id = SENTINEL_SERVER_ID;
        return cand;
    }

    if (policy == POLICY_FIRST_FIT) {
        cand.score = 0.0;
        cand.server_id = s->id;
    } else {
        cand.score = calculate_server_score(s, r, latency);
        cand.server_id = s->id;
    }
    return cand;
}

Candidate compare_candidates(Candidate a, Candidate b) {
    if (a.score < b.score) return a;
    if (b.score < a.score) return b;
    /* Tie-breaker: lowest global server ID */
    return (a.server_id < b.server_id) ? a : b;
}

void reserve_server_resources(Server *s, const Request *r) {
    s->avail_cpu -= r->req_cpu;
    s->avail_gpu -= r->req_gpu;
    s->avail_ram -= r->req_ram;
    s->avail_bw  -= r->req_bw;
}

void release_server_resources(Server *s, const Session *sess) {
    s->avail_cpu += sess->res_cpu;
    s->avail_gpu += sess->res_gpu;
    s->avail_ram += sess->res_ram;
    s->avail_bw  += sess->res_bw;
}
