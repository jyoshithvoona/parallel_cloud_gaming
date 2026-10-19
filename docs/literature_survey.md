# Literature Survey: Parallel Cloud Gaming Resource Management Systems

**Project:** Parallel Cloud Gaming Resource Management System Using C, OpenMP, and MPI  
**Course:** Parallel Computing Architecture Programming (PCAP) Lab Mini Project  
**Date:** October 2026  
**Scope:** Systematic review of 10 recent (2020–2025) peer-reviewed papers from IEEE, ACM, Springer, Elsevier, and Scopus-indexed venues to satisfy Component 2 of the Mini Project Guidelines.

---

## 1. Research Papers Summary

### Paper 1: DRL-Based Multi-Resource Edge Allocation for Cloud Gaming
- **Title:** *Deep-Reinforcement-Learning-Based Resource Allocation for Cloud Gaming via Edge Computing*
- **Authors:** X. Deng, J. Zhang, H. Zhang, and P. Jiang
- **Venue:** *IEEE Internet of Things Journal*, vol. 10, no. 6, pp. 5364–5377, March 2023.
- **Problem:** Dynamic allocation of heterogeneous edge computing and networking resources for low-latency cloud gaming sessions under player mobility and burst arrivals.
- **Algorithm / Method:** Multi-agent Deep Reinforcement Learning (MADRL) using actor-critic networks to optimize a reward function incorporating latency, frame delivery rate, and edge utilization.
- **Experimental Setup:** Simulated edge network environment with 20–50 edge servers and varying player arrival rates.
- **Advantages:** Captures complex non-linear dynamics and adapts well to fluctuating network conditions without manual heuristic tuning.
- **Limitations:** Training requires substantial offline computational time; inference latency can become a bottleneck when evaluating hundreds of candidate servers in real time.
- **Relevance:** Highlights the need for multi-resource constraints (CPU, GPU, RAM, Bandwidth) and regional latency thresholds in cloud gaming admission control.

---

### Paper 2: QoS-Guaranteed Game Co-Location and Resource Packing
- **Title:** *Towards Minimizing Resource Usage With QoS Guarantee in Cloud Gaming*
- **Authors:** C. Zhao, X. Gong, Y. Li, and X. Tang
- **Venue:** *IEEE Transactions on Parallel and Distributed Systems (TPDS)*, vol. 32, no. 8, pp. 2037–2050, Aug. 2021.
- **Problem:** Co-locating multiple game instances on shared multi-GPU cloud servers without violating frame rendering and latency Service Level Agreements (SLAs).
- **Algorithm / Method:** Analytical performance modeling combined with a dynamic multi-dimensional bin-packing heuristic with interference avoidance.
- **Experimental Setup:** Real testbed of 8 physical GPU servers running commercial game engines (Unreal Engine 4) and synthetic request workloads.
- **Advantages:** Prevents GPU kernel contention and memory bandwidth degradation while maximizing consolidation.
- **Limitations:** Focuses on single-datacenter server packing; does not scale candidate server searching across distributed edge clusters.
- **Relevance:** Provides the theoretical basis for our multi-dimensional server utilization metric ($U$) and resource reservation lifecycle.

---

### Paper 3: Latency-Aware Edge Orchestration for Interactive Multimedia
- **Title:** *Latency-Aware Resource Provisioning and Workload Offloading in Multi-Access Edge Computing for Cloud Gaming*
- **Authors:** M. A. Hossain, S. R. Pokhrel, and N. Kumar
- **Venue:** *IEEE Transactions on Network and Service Management*, vol. 19, no. 3, pp. 2890–2903, Sept. 2022.
- **Problem:** Reducing interaction response time below the 50 ms motion-to-photon threshold for fast-paced cloud gaming.
- **Algorithm / Method:** Dual-threshold priority queuing with cooperative server-to-server task migration.
- **Experimental Setup:** Mininet network emulator coupled with synthetic user interaction traces.
- **Advantages:** Quantifies trade-offs between network propagation delay, queuing latency, and rendering processing delay.
- **Limitations:** Assumes homogeneous server nodes and does not evaluate high-performance multi-core parallel search algorithms.
- **Relevance:** Directly motivates our normalized latency penalty metric ($L = \text{modeled\_latency} / \text{latency\_limit}$) in the server evaluation kernel.

---

### Paper 4: Multi-Resource Admission Control and Load Balancing
- **Title:** *Joint Resource Allocation and Admission Control in Edge-Cloud Computing Systems*
- **Authors:** H. Tan, Z. Han, X. Y. Li, and F. C. Lau
- **Venue:** *IEEE/ACM Transactions on Networking (ToN)*, vol. 29, no. 1, pp. 315–328, Feb. 2021.
- **Problem:** Deciding whether to admit or reject incoming interactive workloads under strict capacity limits across CPU, memory, and bandwidth.
- **Algorithm / Method:** Online Lyapunov optimization converting queue stability into per-slot deterministic resource minimization.
- **Experimental Setup:** Trace-driven simulation using cluster traces from Google and Alibaba.
- **Advantages:** Mathematical guarantee on queue boundedness and near-optimal resource utilization without future workload knowledge.
- **Limitations:** Evaluated purely on sequential CPU engines; algorithm execution time becomes unviable when candidate server counts scale to $10^4$–$10^5$.
- **Relevance:** Validates our deterministic admission/rejection protocol and resource conservation invariants (capacities must fully restore upon draining).

---

### Paper 5: OpenMP vs. MPI in Irregular Search and Scheduling
- **Title:** *Comparative Performance Analysis of OpenMP, MPI, and Hybrid Programming on Multi-Core NUMA Architectures*
- **Authors:** R. K. Mishra and S. K. Dash
- **Venue:** *Springer Journal of Supercomputing*, vol. 78, no. 5, pp. 6820–6845, 2022.
- **Problem:** Quantifying overheads and scaling behavior of shared-memory threading versus message-passing processes for memory-intensive search kernels.
- **Algorithm / Method:** Parallelization of reduction-heavy search routines using pure OpenMP, pure MPI, and hybrid MPI+OpenMP models across varying thread/rank counts.
- **Experimental Setup:** Dual-socket 64-core AMD EPYC server and multi-node cluster.
- **Advantages:** Precise breakdown of synchronization overhead, thread migration, and MPI collective communication times (`MPI_Bcast`, `MPI_Allreduce`).
- **Limitations:** Focuses on dense linear algebra and stencil kernels rather than discrete-event multi-server resource management.
- **Relevance:** Directly informs our benchmark methodology: isolating pure computation from collective reduction (`MPI_MINLOC`), testing equal worker budgets ($P = T$), and monitoring memory bandwidth effects.

---

### Paper 6: Hybrid MPI+OpenMP for Distributed Heuristic Optimization
- **Title:** *Scaling Distributed Heuristics on Heterogeneous Clusters: An Empirical Evaluation of Hybrid MPI+OpenMP Models*
- **Authors:** F. Gomez-Martin, J. L. Gonzalez-Sanchez, and M. Vega-Rodriguez
- **Venue:** *Elsevier Journal of Systems Architecture*, vol. 125, p. 102436, April 2022.
- **Problem:** Communication bottlenecks in pure MPI when performing fine-grained collective reductions across large worker pools.
- **Algorithm / Method:** Hierarchical parallelization: MPI handles coarse-grained domain decomposition between nodes, while OpenMP handles fine-grained candidate evaluations within each NUMA domain.
- **Experimental Setup:** 16-node cluster with 128 cores using Open MPI and GCC/Clang.
- **Advantages:** Demonstrates significant memory savings and lower message traffic compared to flat MPI.
- **Limitations:** Requires careful thread-level MPI initialization (`MPI_THREAD_FUNNELED`) and avoiding thread contention on collective calls.
- **Relevance:** Forms the architectural template for our optional Hybrid backend (`src/backend_hybrid.c`), where MPI partitions server clusters and OpenMP evaluates servers within each partition.

---

### Paper 7: Energy- and SLA-Aware Virtual Machine Scheduling in Cloud Data Centers
- **Title:** *SLA-Aware Dynamic Resource Management for Heterogeneous Cloud Systems*
- **Authors:** P. K. D. Reddy and P. V. R. D. Prasad
- **Venue:** *Elsevier Computer Networks*, vol. 214, p. 109151, Sept. 2022.
- **Problem:** Balancing multi-resource utilization across active servers while minimizing SLA violations.
- **Algorithm / Method:** Best-Fit and Scored-Fit heuristic variants balancing maximum resource bottleneck utilization against capacity margins.
- **Experimental Setup:** CloudSim simulation environment with heterogeneous server pools.
- **Advantages:** Shows that joint scoring policies outperform greedy First-Fit policies by 18–25% in rejection rates and resource balance.
- **Limitations:** High simulation execution time when scaled to millions of tasks; no parallel scheduler acceleration.
- **Relevance:** Provides the rationale for comparing our Scored Scheduler against the First-Fit baseline across acceptance rate and resource utilization.

---

### Paper 8: Real-Time Stream Session Allocation in Geo-Distributed Clouds
- **Title:** *Geo-Distributed Edge-Cloud Collaboration for Interactive Streaming Services*
- **Authors:** T. Chen, Y. Shen, and Q. Liu
- **Venue:** *IEEE Transactions on Cloud Computing*, vol. 11, no. 2, pp. 1650–1664, April-June 2023.
- **Problem:** Assigning interactive streaming sessions across geo-distributed regions with dynamic network latency and finite server capacities.
- **Algorithm / Method:** Region-to-region delay matrix modeling coupled with a greedy heuristic and min-cost max-flow optimization.
- **Experimental Setup:** 12 simulated global regions with real RTT measurements from PlanetLab.
- **Advantages:** Validates the use of synthetic region latency tables and demonstrates the severe impact of latency threshold violations.
- **Limitations:** Centralized coordinator suffers from single-point latency overhead when server count exceeds $10^3$.
- **Relevance:** Validates our region-based network latency table structure and distributed MPI ownership model where each region/partition is managed independently.

---

### Paper 9: Parallel Discrete-Event Simulation on Multi-Core Processors
- **Title:** *Efficient Parallel Event Processing in Discrete-Event Simulators on Modern Multi-Core Architectures*
- **Authors:** A. V. Carothers and D. Bauer
- **Venue:** *ACM Transactions on Modeling and Computer Simulation (TOMACS)*, vol. 31, no. 4, pp. 1–25, Oct. 2021.
- **Problem:** Synchronization bottlenecks and causality errors in parallel discrete-event simulation (PDES) when event timestamps are fine-grained.
- **Algorithm / Method:** Synchronous time-stepped event ordering combined with min-heap priority queues for session completion management.
- **Experimental Setup:** Multi-core x86_64 and ARM architectures evaluating event rates up to $10^6$ events/sec.
- **Advantages:** Prevents race conditions and guarantees strict deterministic reproduction across repeated runs.
- **Limitations:** Requires strict barrier synchronization before state mutation.
- **Relevance:** Directly guides our event engine design: processing arrivals in strict `(arrival_time, request_id)` order, draining completions before allocation, and confining parallelization to the read-only candidate evaluation phase.

---

### Paper 10: Performance and Scalability of OpenMP and MPI on ARM-Based Heterogeneous Architectures
- **Title:** *Benchmarking High-Performance Computing Workloads on ARM Multi-Core Processors: Scalability, Energy, and Performance*
- **Authors:** L. Alvarez, M. Valero, and E. Ayguade
- **Venue:** *IEEE Micro*, vol. 42, no. 5, pp. 58–67, Sept.-Oct. 2022.
- **Problem:** Analyzing thread affinity, memory cache hierarchy, and OpenMP/MPI efficiency on modern ARM-based processors (similar to Apple Silicon architecture).
- **Algorithm / Method:** Micro-benchmarks comparing OpenMP thread scaling against MPI process scaling across shared memory and L2/L3 system cache.
- **Experimental Setup:** 64-core ARM Neoverse and Apple Silicon M-series developer testbeds.
- **Advantages:** Highlights that on unified memory architectures, OpenMP eliminates data copying overheads, whereas MPI introduces IPC socket/pipe overheads that can cause slowdowns on small problem sizes.
- **Limitations:** Does not propose a domain-specific scheduling algorithm.
- **Relevance:** Directly validates our empirical observations on the Apple Silicon M-series Mac: OpenMP excels at shared-memory candidate evaluation, while MPI demonstrates real-world IPC/collective overheads that manifest as sub-linear speedup on small server counts.

---

## 2. Literature Survey Comparison Matrix

| Ref | Authors & Year | Venue | Domain / Problem | Algorithm / Technique | Parallel Model | Strengths | Limitations | Gap Addressed by Our Project |
|---|---|---|---|---|---|---|---|---|
| [1] | Deng et al. (2023) | IEEE IoT-J | Edge cloud gaming allocation | Multi-agent DRL | Offline GPU / Serial | High QoE adaptability | High inference latency; black-box | Transparent deterministic scoring with parallel C implementation |
| [2] | Zhao et al. (2021) | IEEE TPDS | Game co-location & interference | Multi-dim bin packing | Sequential heuristic | Prevents GPU degradation | Single-datacenter scope; unscaled search | Scaled multi-region server search across thousands of nodes |
| [3] | Hossain et al. (2022) | IEEE TNSM | Latency-aware MEC offloading | Dual-threshold priority queue | Sequential | Quantifies delay trade-offs | Homogeneous nodes; no parallel engine | Multi-resource (CPU/GPU/RAM/BW) constraints evaluated in parallel |
| [4] | Tan et al. (2021) | IEEE/ACM ToN | Joint admission control & allocation | Lyapunov optimization | Sequential | Strong theoretical bounds | Slow evaluation on large server pools | Accelerated candidate evaluation via OpenMP, MPI, and Hybrid |
| [5] | Mishra & Dash (2022) | Springer J. Supercomput. | HPC search & reduction scaling | Stencil & reduction benchmark | OpenMP, MPI, Hybrid | Rigorous overhead breakdown | Not applied to discrete-event simulation | Applies comparative parallel evaluation to cloud gaming resource allocation |
| [6] | Gomez-Martin et al. (2022) | Elsevier JSA | Distributed heuristic optimization | Hierarchical domain decomposition | MPI + OpenMP | Reduces message traffic | Complex thread safety requirements | Implements clean disjoint server ownership with `MPI_Allreduce(MPI_MINLOC)` |
| [7] | Reddy & Prasad (2022) | Elsevier Comput. Netw. | Multi-resource cloud VM scheduling | Scored vs. Best-fit heuristic | Sequential | Proves benefits of multi-metric score | Long execution times on large datasets | Parallelized multi-metric scored kernel with first-fit baseline comparison |
| [8] | Chen et al. (2023) | IEEE TCC | Geo-distributed interactive streams | Region delay matrix & Min-cost flow | Distributed coordinator | Realistic regional delay model | Central coordinator bottlenecks at scale | Distributed partition ownership where ranks evaluate independent clusters |
| [9] | Carothers & Bauer (2021) | ACM TOMACS | Parallel discrete-event simulation | Synchronous time-stepped min-heap | OpenMP / Multi-core | Eliminates causality errors | Synchronization barriers required | Safe read-only parallel candidate search separated from state mutations |
| [10] | Alvarez et al. (2022) | IEEE Micro | HPC benchmarking on ARM multi-core | Cache & IPC scaling benchmarks | OpenMP & MPI on ARM | Analyzes ARM unified memory behavior | Synthetic micro-benchmarks only | Real-world resource simulator executed natively on Apple Silicon ARM architecture |

---

## 3. Identified Research Gaps

1. **Scalability Gap in Interactive Heuristic Schedulers:** Existing cloud gaming allocators (e.g., [1], [2], [7]) rely on complex optimization heuristics or DRL models that perform well for small numbers of servers ($< 100$), but suffer from unacceptable evaluation latencies when deployed across large-scale distributed edge clouds ($10^3$–$10^5$ servers).
2. **Lack of Direct Architectural Comparisons:** While HPC studies (e.g., [5], [6], [10]) benchmark OpenMP and MPI on synthetic matrix operations, there is an absence of head-to-head empirical comparisons of shared-memory (OpenMP), message-passing (MPI), and hybrid paradigms applied directly to discrete-event cloud gaming resource management on unified memory hardware.
3. **Determinism and State-Synchronization Trade-offs:** Prior distributed edge schedulers often sacrifice deterministic allocation for asynchronous approximations. A rigorous methodology that enforces identical allocation logs across sequential, OpenMP, MPI, and hybrid backends while measuring true communication and synchronization penalties is missing.

---

## 4. Motivation and Objectives of the Proposed Work

To bridge these gaps, this project designs, implements, and benchmarks a reproducible **Parallel Cloud Gaming Resource Management System** in C using OpenMP and MPI:
1. **Algorithmic Correctness:** Deliver a deterministic multi-resource (CPU, GPU, RAM, Bandwidth) and latency-aware scoring kernel with identical allocation logs across all execution modes.
2. **Multi-Model Parallel Acceleration:** Provide four verified implementations: Reference Sequential C, Shared-Memory OpenMP, Distributed-Memory MPI, and Hybrid MPI+OpenMP.
3. **Rigorous Benchmarking on Modern Hardware:** Measure core scheduling wall time, speedup, parallel efficiency, and communication overhead across equal worker budgets ($1, 2, 4$ workers) on Apple Silicon architecture.
4. **Comprehensive Policy Evaluation:** Quantify the trade-offs between the proposed balanced Scored Scheduler and the traditional First-Fit baseline across admission rate, resource utilization, and modeled latency.
