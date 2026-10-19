# Parallel Cloud Gaming Resource Management System Using C, OpenMP, and MPI

A reproducible, high-performance Cloud Gaming Resource Management Simulator implemented in C11, featuring four distinct execution backends: **Sequential C**, **Shared-Memory OpenMP**, **Distributed-Memory MPI**, and **Hybrid MPI+OpenMP**.

---

## 1. Project Overview

Cloud gaming requires ultra-low round-trip latency (<50–100 ms) and simultaneous multi-resource capacity checks (CPU, GPU, RAM, network bandwidth). When thousands of candidate servers exist in distributed edge clouds, candidate evaluation introduces significant scheduling bottlenecks.

This project implements:
1. **Deterministic Multi-Resource Scoring Kernel:** Evaluates candidate servers using $S = 0.5 \cdot L + 0.5 \cdot U$ where $L$ is the normalized latency penalty and $U$ is the peak multi-resource bottleneck utilization.
2. **Dynamic Discrete-Event Lifecycle:** Session completions are managed with an event priority min-heap. When sessions expire, reserved capacities are restored.
3. **Four Execution Modes:** Sequential reference, OpenMP (`#pragma omp for`), MPI (`MPI_Bcast` + `MPI_Allreduce(MPI_MINLOC)`), and Hybrid (`MPI_THREAD_FUNNELED`).
4. **Guaranteed Cross-Backend Equivalence:** All four backends produce 100% bitwise equivalent decision logs on identical inputs.

---

## 2. Directory Structure

```text
├── Makefile                # Build system for all targets
├── README.md               # Project documentation and guide
├── Project_Requirements.md # Companion requirements checklist
├── Implementation_Plan.md  # Detailed technical roadmap
├── include/
│   ├── models.h            # Core data structs (Server, Request, Session, Candidate)
│   └── simulator.h         # Function declarations across modules
├── src/
│   ├── generator.c         # Synthetic dataset generator
│   ├── input.c             # High-speed CSV parsing and monotonic timer
│   ├── scoring.c           # Feasibility checks and scoring kernel
│   ├── events.c            # Priority min-heap session lifecycle engine
│   ├── metrics.c           # Results reporting and CSV metrics export
│   ├── backend_seq.c       # Sequential reference scheduler
│   ├── backend_omp.c       # OpenMP shared-memory scheduler
│   ├── backend_mpi.c       # MPI distributed-partition scheduler
│   ├── backend_hybrid.c    # Hybrid MPI+OpenMP scheduler
│   ├── main_seq.c          # Sequential executable entry point
│   ├── main_omp.c          # OpenMP executable entry point
│   ├── main_mpi.c          # MPI executable entry point
│   └── main_hybrid.c       # Hybrid executable entry point
├── data/                   # Generated simulation datasets (CSV)
├── results/                # Decision logs and benchmark metrics CSVs
├── plots/                  # Generated benchmark scaling and policy figures
├── tests/
│   ├── test_correctness.c  # 12 correctness gates verification suite
│   ├── smoke_openmp.c      # OpenMP toolchain validation
│   ├── smoke_mpi.c         # MPI collective validation
│   └── smoke_hybrid.c      # Hybrid thread support level validation
├── scripts/
│   ├── run_benchmarks.sh   # 135-run benchmark suite across worker budgets
│   └── generate_plots.py   # Python plotting script (Matplotlib)
└── docs/
    ├── literature_survey.md# 10 recent peer-reviewed papers & comparison matrix
    ├── report_outline.md   # 17-section template mapping
    ├── IEEE_Report.md      # IEEE conference paper draft
    └── Springer_Report.md  # Springer Nature conference paper draft
```

---

## 3. Quick Start & Build Instructions

### Prerequisites
- macOS Apple Silicon (or Linux x86_64/ARM64)
- LLVM Clang with OpenMP (`brew install llvm libomp`)
- Open MPI (`brew install open-mpi`)
- Python 3 with `matplotlib`, `pandas`, `numpy`

### Build All Executables
```bash
make all
```
This builds four binaries: `./sequential`, `./openmp`, `./mpi`, and `./hybrid`.

### Run Correctness Gates Verification Suite
```bash
make test
```
Verifies all 12 correctness gates (exact fit, capacity bounds, latency limits, tie-breaking, completion order, capacity conservation, and cross-backend equivalence).

---

## 4. Running the Schedulers

### 1. Sequential Mode (Reference Oracle)
```bash
./sequential --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --policy scored --log results/log_seq.txt
```

### 2. OpenMP Mode (Shared Memory)
```bash
./openmp --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --threads 4 --policy scored --log results/log_omp.txt
```

### 3. MPI Mode (Distributed Ranks)
```bash
mpirun -np 4 ./mpi --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --policy scored --log results/log_mpi.txt
```

### 4. Hybrid Mode (MPI Ranks × OpenMP Threads)
```bash
mpirun -np 2 ./hybrid --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --threads 2 --policy scored --log results/log_hybrid.txt
```

---

## 5. Benchmarks & Plot Generation

### Execute Automated Benchmark Suite (135 Runs)
```bash
./scripts/run_benchmarks.sh
```
Executes 5 repetitions across worker budgets ($1, 2, 4$) on 100, 1,000, and 10,000 servers, outputting raw measurements to `results/benchmark_results.csv`.

### Generate Plots
```bash
.venv/bin/python scripts/generate_plots.py
```
Outputs the four required evaluation figures to `plots/`:
- `plots/fig1_runtime_vs_workers.png`
- `plots/fig2_speedup_efficiency.png`
- `plots/fig3_scaling_servers.png`
- `plots/fig4_policy_comparison.png`

---

## 6. Academic Deliverables & Reports

- **Literature Survey (5 Marks):** [`docs/literature_survey.md`](file:///Users/voonajyosith/Documents/PCAP_llel_cloud_gaming/docs/literature_survey.md)
- **IEEE Conference Format (10 Marks):** [`docs/IEEE_Report.md`](file:///Users/voonajyosith/Documents/PCAP_llel_cloud_gaming/docs/IEEE_Report.md)
- **Springer Nature Format (10 Marks):** [`docs/Springer_Report.md`](file:///Users/voonajyosith/Documents/PCAP_llel_cloud_gaming/docs/Springer_Report.md)
