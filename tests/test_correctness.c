#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "../include/simulator.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond, name) do { \
    if (cond) { \
        printf("[PASS] %s\n", name); \
        tests_passed++; \
    } else { \
        printf("[FAIL] %s (line %d)\n", name, __LINE__); \
        tests_failed++; \
    } \
} while(0)

/* Gate 1 & 2: Exact fit & Resource shortage */
void test_exact_fit_and_shortage(void) {
    Server s = { .id = 0, .region = 0, .total_cpu = 10, .avail_cpu = 10,
                 .total_gpu = 2, .avail_gpu = 2, .total_ram = 16, .avail_ram = 16,
                 .total_bw = 100, .avail_bw = 100 };
    Request r_exact = { .id = 1, .region = 0, .req_cpu = 10, .req_gpu = 2,
                        .req_ram = 16, .req_bw = 100, .latency_limit = 50 };
    Request r_excess = { .id = 2, .region = 0, .req_cpu = 11, .req_gpu = 2,
                         .req_ram = 16, .req_bw = 100, .latency_limit = 50 };

    TEST_ASSERT(is_server_feasible(&s, &r_exact, 10.0), "Gate 1: Exact fit is feasible");
    TEST_ASSERT(!is_server_feasible(&s, &r_excess, 10.0), "Gate 2: Resource shortage is rejected");

    reserve_server_resources(&s, &r_exact);
    TEST_ASSERT(s.avail_cpu == 0.0 && s.avail_gpu == 0.0 && s.avail_ram == 0.0 && s.avail_bw == 0.0,
                "Gate 1: Remaining capacity becomes zero on exact fit");
}

/* Gate 3: Latency violation */
void test_latency_violation(void) {
    Server s = { .id = 0, .region = 0, .total_cpu = 100, .avail_cpu = 100,
                 .total_gpu = 10, .avail_gpu = 10, .total_ram = 100, .avail_ram = 100,
                 .total_bw = 1000, .avail_bw = 1000 };
    Request r = { .id = 1, .region = 0, .req_cpu = 1, .req_gpu = 0.5,
                  .req_ram = 1, .req_bw = 10, .latency_limit = 20.0 };

    TEST_ASSERT(!is_server_feasible(&s, &r, 25.0), "Gate 3: Latency violation is excluded");
    TEST_ASSERT(is_server_feasible(&s, &r, 15.0), "Gate 3: Latency within limit is accepted");
}

/* Gate 4: Equal candidate scores tie-breaker */
void test_tie_breaker(void) {
    Candidate a = { .score = 0.450000, .server_id = 5 };
    Candidate b = { .score = 0.450000, .server_id = 2 };
    Candidate winner = compare_candidates(a, b);
    TEST_ASSERT(winner.server_id == 2, "Gate 4: Equal scores break tie by lowest server ID");
}

/* Gate 5: Completion equals arrival order */
void test_completion_timing(void) {
    EventHeap *heap = event_heap_create(4);
    Server servers[1] = {
        { .id = 0, .region = 0, .total_cpu = 10, .avail_cpu = 0,
          .total_gpu = 1, .avail_gpu = 0, .total_ram = 8, .avail_ram = 0,
          .total_bw = 50, .avail_bw = 0 }
    };
    Session sess = { .request_id = 10, .server_id = 0, .end_time = 50,
                     .res_cpu = 10, .res_gpu = 1, .res_ram = 8, .res_bw = 50 };
    event_heap_push(heap, sess);

    /* Advance to arrival time 50 */
    int released = event_heap_release_expired(heap, 50, servers, 1, 0);
    TEST_ASSERT(released == 1 && servers[0].avail_cpu == 10.0,
                "Gate 5: Completion at arrival time processed before next allocation");
    event_heap_destroy(heap);
}

/* Gate 6: Capacity conservation upon drain */
void test_conservation(void) {
    Server initial[2] = {
        { .id = 0, .region = 0, .total_cpu = 64, .avail_cpu = 64, .total_gpu = 8, .avail_gpu = 8, .total_ram = 128, .avail_ram = 128, .total_bw = 1000, .avail_bw = 1000 },
        { .id = 1, .region = 1, .total_cpu = 32, .avail_cpu = 32, .total_gpu = 4, .avail_gpu = 4, .total_ram = 64, .avail_ram = 64, .total_bw = 500, .avail_bw = 500 }
    };
    Server current[2];
    memcpy(current, initial, sizeof(initial));

    EventHeap *heap = event_heap_create(4);
    Request r = { .id = 1, .region = 0, .req_cpu = 16, .req_gpu = 2, .req_ram = 32, .req_bw = 200, .latency_limit = 50 };
    reserve_server_resources(&current[0], &r);
    Session s = { .request_id = 1, .server_id = 0, .end_time = 100, .res_cpu = 16, .res_gpu = 2, .res_ram = 32, .res_bw = 200 };
    event_heap_push(heap, s);

    TEST_ASSERT(!verify_capacities_restored(current, initial, 2), "Gate 11: Capacities altered during active session");
    event_heap_drain_all(heap, current, 2, 0);
    TEST_ASSERT(verify_capacities_restored(current, initial, 2), "Gate 11: Final drain restores all original capacities");
    event_heap_destroy(heap);
}

/* Gate 7: Partition math for uneven N/P */
void test_partition_math(void) {
    int N = 7, P = 3;
    int base = N / P;
    int rem = N % P;
    int covered[7] = {0};

    for (int r = 0; r < P; r++) {
        int count = base + (r < rem ? 1 : 0);
        int start = r * base + (r < rem ? r : rem);
        for (int i = 0; i < count; i++) {
            covered[start + i]++;
        }
    }

    bool perfect = true;
    for (int i = 0; i < N; i++) {
        if (covered[i] != 1) perfect = false;
    }
    TEST_ASSERT(perfect, "Gate 7: Uneven N/P partitions every server exactly once without overlap or gaps");
}

/* Gate 8: Cross-backend equivalence on small dataset */
void test_cross_backend_equivalence(void) {
    int num_s = 0, num_r = 0;
    Server *s_seq = load_servers("data/servers_small.csv", &num_s);
    Server *s_omp = load_servers("data/servers_small.csv", &num_s);
    Request *reqs = load_requests("data/requests_small.csv", &num_r);
    LatencyMatrix lat;
    load_latency_matrix("data/latency_matrix.csv", &lat);

    SimulationMetrics m_seq = run_sequential(s_seq, num_s, reqs, num_r, &lat, POLICY_SCORED, "results/test_seq.log");
    SimulationMetrics m_omp = run_openmp(s_omp, num_s, reqs, num_r, &lat, POLICY_SCORED, 4, "results/test_omp.log");

    TEST_ASSERT(m_seq.accepted_requests == m_omp.accepted_requests &&
                m_seq.rejected_requests == m_omp.rejected_requests,
                "Gate 12: Backend decision counts match identically");

    /* Verify files identical */
    FILE *f1 = fopen("results/test_seq.log", "r");
    FILE *f2 = fopen("results/test_omp.log", "r");
    bool identical = true;
    char line1[256], line2[256];
    while (fgets(line1, sizeof(line1), f1) && fgets(line2, sizeof(line2), f2)) {
        if (strcmp(line1, line2) != 0) {
            identical = false;
            break;
        }
    }
    fclose(f1);
    fclose(f2);
    TEST_ASSERT(identical, "Gate 12: Backend decision logs are 100% bitwise equivalent");

    free_servers(s_seq);
    free_servers(s_omp);
    free_requests(reqs);
}

int main(void) {
    printf("====================================================\n");
    printf(" RUNNING CORRECTNESS GATES VERIFICATION SUITE       \n");
    printf("====================================================\n");

    test_exact_fit_and_shortage();
    test_latency_violation();
    test_tie_breaker();
    test_completion_timing();
    test_conservation();
    test_partition_math();
    test_cross_backend_equivalence();

    printf("----------------------------------------------------\n");
    printf(" SUMMARY: %d Passed, %d Failed\n", tests_passed, tests_failed);
    printf("====================================================\n");

    return (tests_failed == 0) ? 0 : 1;
}
