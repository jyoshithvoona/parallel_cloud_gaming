# Presentation Slides: Parallel Cloud Gaming Resource Management System

**Companion File:** `presentation.pptx` (15 Styled Slides with Embedded Figures)  
**Course:** Parallel Computing Architecture Programming (PCAP) Lab Mini Project  
**Target:** 10 Marks - Project Presentation & Demonstration Component

---

### Slide 1: Title Slide
- **Title:** Parallel Cloud Gaming Resource Management System
- **Subtitle:** High-Performance Discrete-Event Scheduling Using C, OpenMP, and MPI
- **Course:** Parallel Computing Architecture Programming (PCAP) Lab Mini Project
- **Target Machine:** Apple Silicon Multi-Core Architecture (macOS)
- **Deliverables:** Sequential, OpenMP, MPI, Hybrid Backends & Research Reports

---

### Slide 2: Introduction & Motivation
- **Cloud Gaming Imperative:** Renders 3D graphics on remote edge-cloud servers and streams video to thin clients. Demands strict round-trip motion-to-photon latency under 50–100 ms to preserve QoE.
- **Multi-Dimensional Packing:** Incoming gaming sessions simultaneously constrain four physical hardware metrics: CPU cores, GPU rendering units, RAM capacity, and network streaming bandwidth.
- **The Scalability Bottleneck:** In large-scale geo-distributed edge environments with 1,000 to 10,000+ candidate servers, sequential feasibility checks and fitness scoring create severe admission delays during peak bursts.
- **Parallel Computing Solution:** Leveraging shared-memory threading (OpenMP), message-passing processes (MPI), and hybrid decomposition allows parallel exploration of candidate servers to achieve real-time response times.

---

### Slide 3: Problem Statement & Objectives
- **Mathematical Problem Formulation:**
  $$\min_{s \in \mathcal{S}} S(s, r) = 0.5 \cdot L + 0.5 \cdot U$$
  Subject to:
  1. *Multi-Resource Capacity:* $\sum \text{demands} \le \text{capacity}$ across CPU, GPU, RAM, Bandwidth.
  2. *Network Latency:* $\text{latency}(\text{player\_region}, \text{server\_region}) \le \text{latency\_limit}$.
  3. *Lifecycle Conservation:* Exact resource restoration when sessions finish at $t_e = t_a + d$.
- **Core Objectives:**
  1. Reproducible C Simulator with discrete-event priority min-heap.
  2. Multi-Model Parallel Acceleration: OpenMP, MPI, and Hybrid.
  3. 100% Cross-Backend Equivalence in allocation decisions.
  4. Systematic benchmarks up to 10,000 servers across equal worker budgets ($1, 2, 4$).
  5. Policy comparison against First-Fit heuristic baseline.

---

### Slide 4: Literature Review (10 Papers: 2020–2025)
- **Deng et al. (IEEE IoT-J 2023):** Multi-Agent DRL for edge cloud gaming. High QoE adaptability, but high offline training and inference delay.
- **Zhao et al. (IEEE TPDS 2021):** Multi-GPU game co-location. Eliminates frame rate degradation via multi-dimensional bin packing.
- **Hossain et al. (IEEE TNSM 2022):** MEC latency-aware offloading. Analyzed delay trade-offs; evaluated on homogeneous sequential models.
- **Tan et al. (IEEE/ACM ToN 2021):** Lyapunov online admission control. Theoretical queue bounds, but computationally unviable at 10,000 servers.
- **Mishra & Dash (Springer 2022):** OpenMP vs. MPI on NUMA systems. Quantified communication overheads in reduction-heavy algorithms.
- **Gomez-Martin (Elsevier JSA 2022):** Hybrid MPI+OpenMP heuristics. Hierarchical decomposition reduces collective network message traffic.
- **Research Gap:** Existing schedulers rely on slow sequential heuristics or black-box ML models. A deterministic C simulator evaluating OpenMP vs. MPI vs. Hybrid scaling directly on unified memory architectures has been unaddressed.

---

### Slide 5: System Architecture & Workflow Diagram
*(Embedded: `plots/architecture_flowchart.png`)*
- **Input Module (`src/input.c`):** Fast CSV parsing for servers, requests, and $4 \times 4$ latency matrix.
- **Event Engine (`src/events.c`):** Session completion priority min-heap. Automatically restores capacities when $t_e \le t_a$.
- **Scoring Kernel (`src/scoring.c`):** Deterministic evaluation: $S = 0.5 L + 0.5 U$. Lowest score wins; ties broken by lowest server ID.
- **Four Modular Backends:** Sequential, OpenMP shared memory, MPI message-passing, and Hybrid MPI+OpenMP.

---

### Slide 6: Mathematical Model & Algorithms
- **Feasibility Filter:**
  $$\text{Latency}(r, s) \le \text{Limit}(r) \quad \wedge \quad \text{Avail}[res] \ge \text{Demand}[res] \quad \forall res$$
- **Composite Score Formula:**
  $$L = \frac{\text{latency}}{\text{latency\_limit}}, \quad u_r = \frac{\text{total}_r - \text{avail}_r + \text{demand}_r}{\text{total}_r}$$
  $$\text{Score} = 0.5 \cdot L + 0.5 \cdot \max_{r} (u_r)$$
- **Deterministic Comparator:**
  Candidate pair: `struct { double score; int server_id; }` directly compatible with `MPI_Allreduce(..., MPI_DOUBLE_INT, MPI_MINLOC)`.

---

### Slide 7: Implementation Details of the 4 Backends
1. **Sequential C (`src/backend_seq.c`):** Reference scanner through all $N$ servers. Baseline oracle for correctness and speedup calculation.
2. **OpenMP Shared-Memory (`src/backend_omp.c`):** `#pragma omp parallel` with `#pragma omp for schedule(static)`. Thread-private best candidate search with single reduction at loop exit. Zero false sharing.
3. **MPI Message-Passing (`src/backend_mpi.c`):** Contiguous partition split: $\text{count} = N/P + (rank < N\%P ? 1 : 0)$. Rank 0 broadcasts request (`MPI_Bcast`). Each rank scans local partition. Global winner reduced via `MPI_Allreduce(MPI_MINLOC)`.
4. **Hybrid MPI+OpenMP (`src/backend_hybrid.c`):** Initialized with `MPI_Init_thread(MPI_THREAD_FUNNELED)`. Combines MPI domain decomposition across ranks with OpenMP multi-threaded scanning inside each rank.

---

### Slide 8: Experimental Setup & Hardware Specifications
- **Host Machine:** Apple Silicon MacBook Air (Apple M-series)
- **CPU:** 10 logical cores (heterogeneous performance & efficiency cores)
- **Memory:** 16 GB unified RAM (17.18 GB), 512 GB SSD storage
- **Compilers:** Homebrew LLVM Clang 23.1.3 (`-std=c11 -O2 -Wall -Wextra`)
- **OpenMP Runtime:** Homebrew `libomp` 23.1.3 (`-fopenmp`)
- **MPI Environment:** Open MPI 5.0.11 (`mpicc` / `mpirun`)
- **Python:** Python 3.12 (.venv) with Matplotlib, Pandas, NumPy
- **Workloads:** 100, 1,000, and 10,000 server pools evaluated across 1,000 high-arrival requests with 5 repetitions.

---

### Slide 9: Correctness Gates Verification (12 Gates)
*(Automated test suite: `make test`)*
- `[PASS]` Gate 1 & 2: Boundary Feasibility (Exact fit allocates completely; shortage rejects safely).
- `[PASS]` Gate 3: Latency Invariant (Rejects servers exceeding latency threshold).
- `[PASS]` Gate 4: Deterministic Tie-Breaking (Lowest score, then lowest Server ID).
- `[PASS]` Gate 5: Event Ordering (Completions at current timestamp released before next allocation).
- `[PASS]` Gate 6 & 7: Lifecycle Conservation (Full drain restores 100.0% initial capacities).
- `[PASS]` Gate 8–12: Cross-Backend Equivalence (Sequential == OpenMP == MPI == Hybrid: 100% bitwise identical decision logs).

---

### Slide 10: Benchmark Results & Scalability Figures
*(Embedded: `plots/fig1_runtime_vs_workers.png` & `plots/fig2_speedup_efficiency.png`)*
- At 10,000 servers, MPI achieves **$2.92\times$ speedup** on 4 ranks ($0.0125$ s vs. $0.0366$ s sequential).
- MPI parallel efficiency reaches **$73.1\%$**, successfully amortizing per-request collective communication.

---

### Slide 11: Scaling Behavior & Crossover Points
*(Embedded: `plots/fig3_scaling_servers.png`)*
- **Small Scale (100 Servers):** Sequential C is fastest ($0.0004$ s). Thread barrier synchronization and IPC socket overheads exceed compute savings.
- **Medium Scale (1,000 Servers):** Crossover region: MPI 4 ranks ($0.0027$ s) surpasses Sequential ($0.0037$ s).
- **Large Scale (10,000 Servers):** MPI achieves dominant performance ($0.0125$ s vs. $0.0366$ s).
- **Hybrid Configuration:** 2 ranks $\times$ 2 threads achieves $0.0256$ s.

---

### Slide 12: Policy Evaluation: Scored vs. First-Fit
*(Embedded: `plots/fig4_policy_comparison.png`)*
- **54.7% Latency Reduction:** Scored allocation reduces average network delay from $31.43$ ms (First-Fit) to $14.22$ ms (Scored).
- **Balanced Utilization:** Scored policy prevents server hot-spotting and premature GPU exhaustion.
- **First-Fit Trade-off:** First-Fit terminates early ($0.007$ s), but produces over $2\times$ higher network latency for players.

---

### Slide 13: Live Demonstration Guide
- **Correctness Suite:** `make test`
- **Sequential Run:** `./sequential --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --policy scored --log results/log_seq.txt`
- **OpenMP Run:** `./openmp --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --threads 4 --policy scored --log results/log_omp.txt`
- **MPI Run:** `mpirun -np 4 ./mpi --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --policy scored --log results/log_mpi.txt`
- **Hybrid Run:** `mpirun -np 2 ./hybrid --servers data/servers_1k.csv --requests data/requests_heavy_1k.csv --threads 2 --policy scored --log results/log_hybrid.txt`

---

### Slide 14: Team Contributions & Role Distribution
- **Architecture & Event Engine:** Data models (`models.h`), priority min-heap (`events.c`), lifecycle restoration, CSV parsers.
- **Sequential & OpenMP Development:** Reference sequential scheduler, OpenMP loop distribution, thread-private candidate reduction, correctness test suite.
- **MPI & Hybrid Distributed Design:** Contiguous partition math, `MPI_Bcast`, collective `MPI_Allreduce(MPI_MINLOC)` reduction, and `MPI_THREAD_FUNNELED` hybrid mode.
- **Benchmarking & Paper Writing:** 135 benchmark runs, Matplotlib plotting script, IEEE & Springer conference paper drafts.

---

### Slide 15: Conclusion & Future Scope
- **Complete Multi-Backend Simulator:** Reference C, OpenMP, MPI, and Hybrid engines in native C.
- **$2.92\times$ Empirical Speedup:** Validated scaling on Apple Silicon hardware for 10,000 servers.
- **$54.7\%$ Latency Advantage:** Demonstrates superior QoE for interactive cloud gaming.
- **100% Cross-Backend Equivalence:** 12 correctness gates verified bitwise.
- **Future Scope:** Physical multi-node InfiniBand cluster testing, request batching to reduce collective frequency, and GPU acceleration via CUDA/OpenACC.
