#!/bin/bash
set -e

CSV_OUT="results/benchmark_results.csv"
rm -f "$CSV_OUT"

echo "=== Starting Parallel Cloud Gaming Benchmark Suite ==="
echo "Output CSV: $CSV_OUT"

SERVERS_LIST=("data/servers_100.csv" "data/servers_1k.csv" "data/servers_10k.csv")
REQUESTS="data/requests_heavy_1k.csv"
REPEATS=5
MPIRUN="/opt/homebrew/bin/mpirun"

# 1. Baseline Sequential (Scored & First-Fit)
echo "--- Running Sequential Benchmarks ---"
for S in "${SERVERS_LIST[@]}"; do
    for POL in "scored" "first_fit"; do
        for REP in $(seq 1 $REPEATS); do
            ./sequential --servers "$S" --requests "$REQUESTS" --policy "$POL" --csv "$CSV_OUT" --repeat "$REP"
        done
    done
done

# 2. OpenMP (1, 2, 4 threads - Scored)
echo "--- Running OpenMP Benchmarks ---"
for S in "${SERVERS_LIST[@]}"; do
    for T in 1 2 4; do
        for REP in $(seq 1 $REPEATS); do
            ./openmp --servers "$S" --requests "$REQUESTS" --policy scored --threads "$T" --csv "$CSV_OUT" --repeat "$REP"
        done
    done
done

# 3. MPI (1, 2, 4 ranks - Scored)
echo "--- Running MPI Benchmarks ---"
for S in "${SERVERS_LIST[@]}"; do
    for P in 1 2 4; do
        for REP in $(seq 1 $REPEATS); do
            $MPIRUN -np "$P" ./mpi --servers "$S" --requests "$REQUESTS" --policy scored --csv "$CSV_OUT" --repeat "$REP"
        done
    done
done

# 4. Hybrid (2 ranks x 2 threads = 4 workers - Scored)
echo "--- Running Hybrid Benchmarks ---"
for S in "${SERVERS_LIST[@]}"; do
    for REP in $(seq 1 $REPEATS); do
        $MPIRUN -np 2 ./hybrid --servers "$S" --requests "$REQUESTS" --threads 2 --policy scored --csv "$CSV_OUT" --repeat "$REP"
    done
done

echo "=== Benchmark Suite Completed Successfully ==="
