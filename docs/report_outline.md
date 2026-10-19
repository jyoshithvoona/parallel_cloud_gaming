# Final Report Structure & Outline

This document defines the 17-section structure required by the Mini Project Guidelines for both the **IEEE Conference** and **Springer Nature** paper submissions. Both reports must be technically identical and differ only in formatting.

---

## Required Sections & Alignment

1. **Title**  
   *Parallel Cloud Gaming Resource Management System Using C, OpenMP, and MPI*
2. **Abstract**  
   - Context: Rapid growth of interactive cloud gaming services requiring low-latency, multi-resource server assignment.  
   - Problem: In dynamic edge-cloud environments with thousands of servers, candidate evaluation introduces critical scheduling delays.  
   - Solution: A deterministic, multi-resource (CPU, GPU, RAM, Bandwidth) and latency-aware resource management simulator implemented across four backends: Sequential C, OpenMP shared memory, MPI message passing, and Hybrid MPI+OpenMP.  
   - Findings: Summary of speedup, scaling limits, communication overhead, and policy trade-offs between Scored and First-Fit allocation.
3. **Keywords**  
   Cloud Gaming, Parallel Computing, OpenMP, MPI, Hybrid Programming, Resource Allocation, Discrete-Event Simulation, Apple Silicon.
4. **Introduction**  
   - Background on cloud gaming latency sensitivities (<50 ms response thresholds).  
   - The challenge of multi-resource packing under dynamic session arrivals and departures.  
   - Role of parallel computing in accelerating admission control.
5. **Literature Review**  
   - Systematic review of 10 recent (2020–2025) papers from IEEE, ACM, Springer, Elsevier.  
   - Categorization into: DRL-based allocation, multi-resource packing heuristics, parallel discrete-event simulation, and OpenMP vs. MPI evaluations.  
   - Literature comparison matrix and synthesis.
6. **Problem Statement**  
   - Formal mathematical formulation of the multi-resource allocation problem with dynamic arrivals, session lifecycles, and regional network latency constraints.
7. **Objectives**  
   - Build a verified, reproducible C simulator with strict state conservation.  
   - Implement identical allocation semantics across Sequential, OpenMP, MPI, and Hybrid backends.  
   - Fairly benchmark equal worker budgets ($P = T = 1, 2, 4$) on Apple Silicon.  
   - Compare Scored allocation vs. First-Fit baseline across admission rate, utilization, and modeled latency.
8. **Proposed Methodology**  
   - Discrete-event simulation workflow with event priority min-heaps for session releases.  
   - Two-phase scheduling: (1) read-only candidate evaluation and scoring; (2) deterministic state reservation.  
   - Domain partitioning strategies for OpenMP loop scheduling and MPI contiguous server ownership.
9. **System Architecture**  
   - High-level architectural flowchart.  
   - Sequential engine pipeline.  
   - OpenMP shared-memory architecture (`#pragma omp parallel for`).  
   - MPI distributed-memory architecture (`MPI_Bcast`, `MPI_Allreduce(MPI_MINLOC)`).  
   - Hybrid MPI+OpenMP hierarchical model (`MPI_THREAD_FUNNELED`).
10. **Algorithms**  
    - Algorithm 1: Event-driven session release and request lifecycle.  
    - Algorithm 2: Deterministic multi-resource feasibility and scoring kernel ($S = 0.5 \cdot L + 0.5 \cdot U$).  
    - Algorithm 3: Sequential & OpenMP parallel candidate selection.  
    - Algorithm 4: MPI distributed partition search with collective winner reduction.
11. **Experimental Setup**  
    - Hardware: Apple Silicon MacBook Air (10 CPU cores, 16 GB unified RAM).  
    - Software: macOS Darwin 27, Clang 23.1.3 (LLVM), libomp 23.1.3, Open MPI 5.0.11, Python 3.12 (.venv).  
    - Workload datasets: Small verification (3 servers / 5 requests), Homogeneous & Heterogeneous pools ($10^2, 10^3, 10^4$ servers; $10^3, 10^4$ requests) with light, near-capacity, and bursty arrivals.  
    - Benchmark protocol: 5 repetitions per configuration, warm-up runs, isolated core wall-clock timing (`omp_get_wtime()`, `MPI_Wtime()`).
12. **Results**  
    - Verification and correctness validation table (12 correctness gates passed).  
    - Execution time comparison across worker budgets ($1, 2, 4$).  
    - Speedup and parallel efficiency tables.  
    - MPI communication vs. computation breakdown.  
    - Policy comparison: Scored vs. First-Fit.
13. **Performance Analysis**  
    - Strong scaling analysis and Amdahl's Law constraints.  
    - Impact of server count on OpenMP thread-coordination crossover point.  
    - Analysis of MPI collective overhead (`MPI_Bcast` and `MPI_Allreduce`) on single-machine IPC.  
    - Comparison of Hybrid MPI+OpenMP configurations at fixed worker budget ($W = P \times T = 4$).
14. **Comparison with Existing Methods**  
    - Contrasting our parallel C approach against centralized heuristics, sequential simulators (e.g., CloudSim), and offline DRL models.
15. **Conclusion**  
    - Summary of core findings, confirming that OpenMP achieves near-linear speedups for large server pools, while MPI incurs notable IPC communication overhead on single-machine memory architectures.  
    - Demonstration of strict algorithmic equivalence across all backends.
16. **Future Scope**  
    - Multi-node physical cluster deployment using InfiniBand/10GbE interconnects.  
    - Dynamic request batching to amortize MPI collective communication.  
    - GPU offloading via CUDA / OpenACC for server evaluation kernels.
17. **References**  
    - Formatted in IEEE citation style ([1]–[10] matching the literature survey).

---

## Overleaf Templates

- **Springer Nature Conference Template:**  
  [https://www.overleaf.com/latex/templates/springer-conference-proceedings-template](https://www.overleaf.com/latex/templates/springer-conference-proceedings-template)
- **IEEE Conference Template:**  
  [https://www.overleaf.com/latex/templates/ieee-conference-template/grfzhhncsfqn](https://www.overleaf.com/latex/templates/ieee-conference-template/grfzhhncsfqn)
