# Implementation Plan: C, OpenMP, and MPI

**Project:** Parallel Cloud Gaming Resource Management System Using C, OpenMP, and MPI  
**Target machine:** User's Apple silicon MacBook Air  
**Companion document:** Project_Requirements.md  
**Status:** Proposed design and implementation roadmap; code, experiments, and results are not yet completed.

## 1. Scope and assessment alignment

Build a reproducible resource-management simulator with three core implementations:

1. **Sequential C:** reference algorithm and correctness baseline.
2. **C + OpenMP:** threads evaluate servers in shared memory.
3. **C + MPI:** separate processes own partitions of servers and exchange messages.
4. **Optional hybrid:** MPI partitions servers; OpenMP threads evaluate each rank's partition.

Using OpenMP and MPI separately permits direct comparison. Using both together is a hybrid implementation and a separate experimental case. Local MPI works with multiple processes on one MacBook; it does not measure communication between physical machines.

The project simulates CPU/GPU capacity, RAM, bandwidth, arrivals, session duration, and regional latency. It does not stream games or measure real gaming FPS. Allocation, admission/rejection, session completion, and performance analysis together form a substantial simulator, without requiring a graphical interface.

| Guideline component | Marks | Planned output |
|---|---:|---|
| Proposal | 5 | Specific title, team, problem, motivation, objectives, three-mode design, expected outcomes, timeline |
| Survey and methodology | 5 | 8–10 recent papers, comparison matrix, justified gap, architecture, algorithms, IEEE references |
| Results, analysis, report | 10 | Correctness, scaling, overhead, policy results, figures, raw data, IEEE and Springer reports |
| Presentation/demo | 10 | Live three-mode execution, input/output, resource lifecycle, performance, every member's contribution |
| Paper submission proof | 5 | Genuine submission acknowledgement |
| Paper acceptance proof | 5 | Genuine acceptance evidence, subject to faculty handling of delays |

This plan addresses all categories; it does not guarantee 40 marks. Publication evidence cannot be replaced by extra code or graphs.

## 2. Research questions and success criteria

- Does parallel server evaluation reduce decision time as server count grows?
- How do OpenMP and MPI compare at the same total worker count?
- Where does MPI communication outweigh local computation savings?
- Do all execution modes preserve the same allocation decisions?
- How does scored allocation compare with first-fit in acceptance, latency, and utilization?
- Optional: does hybrid help at a fixed process × thread budget?

Success means correct, reproducible implementations and evidence-based answers, including negative results. Do not set an unsupported target such as guaranteed 4× speedup.

## 3. Mac setup

Install command-line tools if absent, then Homebrew from its official instructions:

```bash
xcode-select --install
```

```bash
brew install llvm libomp open-mpi
```

Inspect installations:

```bash
"$(brew --prefix llvm)/bin/clang" --version
mpicc --showme:command
mpicc --showme:compile
mpicc --showme:link
mpirun --version
sysctl -n hw.logicalcpu
sysctl -n hw.memsize
```

Use native Apple silicon tools consistently. `mpicc` is a compiler wrapper, not itself a C compiler; check which compiler it invokes. For fair benchmarks, use the same compatible compiler and optimization flags across modes. Open MPI allows the `OMPI_CC` override; validate it with a small C MPI program before using it throughout. Never mix incompatible architectures or assume an untested compiler substitution works.

Implement a Makefile with targets `sequential`, `openmp`, `mpi`, and optionally `hybrid`. Illustrative single-file compilation commands, to adapt after source files exist:

```bash
# Sequential
"$(brew --prefix llvm)/bin/clang" -std=c11 -O2 -Wall -Wextra sequential.c -o sequential

# OpenMP
"$(brew --prefix llvm)/bin/clang" -std=c11 -O2 -Wall -Wextra -fopenmp \
  -I"$(brew --prefix libomp)/include" \
  -L"$(brew --prefix libomp)/lib" \
  -Wl,-rpath,"$(brew --prefix libomp)/lib" openmp.c -o openmp

# MPI, using a validated LLVM compiler override
OMPI_CC="$(brew --prefix llvm)/bin/clang" mpicc \
  -std=c11 -O2 -Wall -Wextra mpi.c -o mpi

# Optional hybrid
OMPI_CC="$(brew --prefix llvm)/bin/clang" mpicc \
  -std=c11 -O2 -Wall -Wextra -fopenmp \
  -I"$(brew --prefix libomp)/include" \
  -L"$(brew --prefix libomp)/lib" \
  -Wl,-rpath,"$(brew --prefix libomp)/lib" hybrid.c -o hybrid
```

Smoke-test one thread, multiple threads, one MPI rank, and multiple ranks before developing the full simulator. Final binaries and command-line options below are proposed interfaces, not already implemented software.

## 4. Software architecture

| Component | Responsibility |
|---|---|
| Input/generator | Reproducible servers, requests, latency matrix, seeds |
| Shared scoring kernel | Feasibility checks, score, deterministic candidate comparison |
| Event engine | Ordered arrivals and session completions |
| Sequential backend | One server array and serial candidate search |
| OpenMP backend | Shared array, thread-local candidates, final merge |
| MPI backend | Disjoint server ownership, request broadcast, global winner reduction |
| Optional hybrid backend | OpenMP local search inside each MPI rank |
| Metrics and benchmark harness | Wall time, outputs, repeated runs, CSV data |
| Validation harness | Cross-mode decisions, capacity invariants, edge cases |

Suggested files:

| File or directory | Role |
|---|---|
| `include/models.h` | Common structs and interfaces |
| `src/input.c`, `src/generator.c` | Data input and saved synthetic workloads |
| `src/scoring.c` | One shared feasibility/scoring implementation |
| `src/events.c` | Completion event min-heap and resource updates |
| `src/backend_seq.c` | Sequential selection |
| `src/backend_omp.c` | OpenMP selection |
| `src/backend_mpi.c` | Partitioning, collective communication, local ownership |
| `src/backend_hybrid.c` | Optional hybrid search |
| `src/metrics.c` | Timing and CSV output |
| `src/main_local.c`, `src/main_mpi.c` | Local and MPI entry points |
| `tests/` | Small expected-output inputs and verification |
| `data/`, `results/`, `plots/` | Inputs, raw measurements, figures |
| `Makefile`, `README.md` | Build/run instructions and assumptions |

All simulator and scheduler logic is in C. Shell scripts may automate experiments; spreadsheet software or optional Python may generate figures.

## 5. Data and reproducibility

| Structure | Fields |
|---|---|
| Server | Unique global ID, region, total/available CPU, GPU, RAM, bandwidth |
| Request | ID, region, arrival, duration, four resource demands, latency limit |
| Session | Request ID, owning server ID, end time, reserved resource vector |
| Candidate | Double score and integer global server ID |
| Run configuration | Backend, policy, ranks, threads, seed, input filenames |

Use global server IDs 0 through N−1 and reserve `INT_MAX` for the invalid candidate. Require N below that sentinel. Use integer resource units when practical, with documented scaling for fractional demands. Use fixed-unit integer simulated timestamps to avoid ambiguous equality at completions. Cast utilization ratios to double.

Model network latency with a fixed region-to-region table. Explain that latency and GPU units are synthetic assumptions. Validate finite numeric input, positive capacities/durations/latency limits, valid regions, and nonnegative demands. Keep workload generation outside the timed region and outside parallel execution; save inputs for every mode.

Initial datasets: three servers/five requests for manual verification; then homogeneous and heterogeneous capacity pools with light, heavy, and bursty arrivals. Calibrate arrival rate and duration relative to capacity so cases include accepted sessions, rejection, and resource reuse.

## 6. Common scheduling semantics

Process arrivals by `(arrival_time, request_id)`. Before each arrival, release every session with `end_time <= arrival_time`. Use a completion min-heap; after the final arrival, drain completions for final conservation checks. No real-time sleeping is necessary.

A candidate must satisfy all resource demands and the modeled latency limit. For feasible servers:

```text
L = modeled_latency / request_latency_limit
resource_fraction[r] = (total[r] - available[r] + demand[r]) / total[r]
U = max(resource_fraction[cpu], resource_fraction[gpu],
        resource_fraction[ram], resource_fraction[bandwidth])
score = 0.5 * L + 0.5 * U
```

Use the same score calculation order, compiler settings, and score/ID comparator in every backend. Lowest score wins; exact ties select the lowest global server ID. Exclude NaN inputs and avoid fast-math transformations. No epsilon comparator is needed; inconsistent approximate comparisons can make reductions non-deterministic.

Reserve resources only after selection. A rejected request changes no resource state. The session's end time is arrival plus duration. Sequential and parallel forms of this policy should produce identical outputs; parallelization should change runtime, not acceptance quality.

## 7. Sequential implementation

1. Load and validate data.
2. Initialize servers and the event heap.
3. For every ordered request, release completed sessions.
4. Scan all servers, retaining the best feasible candidate.
5. Reserve resources or reject; log the decision.
6. Drain remaining sessions and verify initial capacity is restored.

This is the correctness oracle. Also implement first-fit as a separate policy that selects the lowest-ID eligible server without scoring all feasible candidates. Use first-fit for policy-quality comparison, not as the denominator for scored-scheduler parallel speedup.

## 8. OpenMP implementation

Keep one shared server array and event heap. The calling thread handles event release and reservation. Only candidate search is parallel:

1. Create a thread team.
2. Each thread starts with an invalid private candidate.
3. Divide server indices with `omp for schedule(static)`.
4. Each thread evaluates its assigned servers without changing shared state.
5. Merge thread-local candidates once per thread using a short critical region, or merge an array after the region.
6. The calling thread reserves the winning server after all evaluation finishes.

Do not lock the whole evaluation loop. Repeated team creation can dominate small searches; optimize to a persistent thread team only after profiling, preserving barriers and read-only search state. Record which variant is benchmarked.

## 9. MPI implementation: explicit ownership

Use P MPI ranks, including rank 0 as a computing rank. Rank 0 also reads requests and produces global decision output. Partition N servers contiguously:

```text
base = N / P
remainder = N % P
count(rank) = base + (rank < remainder ? 1 : 0)
start(rank) = rank * base + min(rank, remainder)
```

Every server and its active sessions belong to exactly one rank. Each rank maintains its own event heap and resource state. Use `MPI_Scatterv` for initial server distribution, with a properly defined MPI datatype, or explicit packed arrays. Broadcast shared latency data/configuration. Avoid sending pointer-containing structs or relying on raw C padding as a portable wire format.

For each request, all ranks execute the same collective sequence:

1. Rank 0 broadcasts a request header and request data with `MPI_Bcast`.
2. Each rank advances to that arrival time and releases completed local sessions.
3. Each rank scans its local server partition and finds its best candidate.
4. Use `MPI_Allreduce` with `MPI_MINLOC` and `MPI_DOUBLE_INT` on a compatible `{double score; int server_id;}` pair. The integer represents global server ID, not rank. Lowest score wins; equal scores choose the lowest ID.
5. If no candidate exists, every rank contributes `(INFINITY, INT_MAX)`; the reduced sentinel means reject.
6. Every rank sees the same winner. Only the rank that owns that ID reserves resources and inserts the session into its local heap. Rank 0 records the decision once.
7. Continue to the next request.

No per-request global server-state replication is necessary. Each rank updates only its local partition before its next search. Collectives must execute in the same order, even on ranks with no servers and on rejected requests. Use a broadcast stop flag at the end, not rank-local early exits.

Use MPI reductions to summarize final metrics and capacity checks. Gather debug snapshots only for small correctness cases, outside performance timing. Handle unrecoverable input/startup errors consistently across ranks so peers cannot remain blocked in a collective.

This design performs roughly N/P server evaluations per request, plus communication. Broadcast and reduction costs can dominate cheap scoring; a valid result may show MPI slower than OpenMP on one laptop. Do not add artificial computation to manufacture speedup.

## 10. Optional MPI + OpenMP hybrid

Reuse the MPI partition and ownership design. Replace each rank's serial local scan with the OpenMP scanner. Initialize MPI using `MPI_Init_thread` requesting `MPI_THREAD_FUNNELED`; check the returned support level and stop clearly if insufficient. Only the initializing/main thread makes MPI calls, outside OpenMP parallel regions. OpenMP `single` alone does not guarantee that the main thread makes a call.

Compare hybrid configurations at fixed total workers W = P × T. For example, four workers means 4 ranks × 1 thread, 2 ranks × 2 threads, or 1 rank × 4 threads. Include the pure OpenMP executable separately because hybrid 1-rank execution still has MPI overhead.

Hybrid is a stretch goal after all three core backends and tests work, not a substitute for validation or required reports.

## 11. Correctness gates

Do not move to performance claims until these checks pass:

| Test | Expected behavior |
|---|---|
| Exact fit | Accepted, remaining relevant capacity becomes zero |
| Resource shortage | Rejected without state mutation |
| Latency violation | Excluded despite adequate capacity |
| Equal candidate scores | Lowest global server ID selected |
| Completion equals arrival | Completion processed before allocation |
| Simultaneous arrivals | Request ID ordering preserved |
| Uneven N/P partition | Every server belongs to exactly one rank |
| More ranks than servers | Empty ranks contribute invalid candidate and join collectives |
| All ranks infeasible | Global rejection; no rank hangs |
| Completion on a nonzero rank | Only owner releases resources, exactly once |
| Final drain | All original capacities restored |
| Backend equivalence | Same per-request assignments/rejections and final state |

Check bounds after every update in validation builds. Compare full decision logs for small and medium deterministic cases, not only accepted counts. Use a checksum for larger logs with selected exact comparisons. Repeat tests at different worker counts. Benchmark builds may disable verbose assertions/logging, consistently across backends.

## 12. Fair benchmark protocol

### Workloads and worker budgets

Start with server counts 100, 1,000, and 10,000; use 100,000 only if pilot runtime/memory allow. Start with 1,000 requests and increase to 10,000 or more after calibration. Include light, near-capacity, and bursty workloads, with heterogeneous resource demands.

Use worker budgets 1, 2, and 4; add larger counts only when supported without oversubscription. All MPI ranks participate in computation.

| Budget | OpenMP | MPI | Optional hybrid |
|---:|---|---|---|
| 1 | 1 thread | 1 rank | 1 × 1 |
| 2 | 2 threads | 2 ranks | 1 × 2 |
| 4 | 4 threads | 4 ranks | 2 × 2 |
| 8, if supported | 8 threads | 8 ranks | 2 × 4 and 4 × 2 |

Example proposed commands, after the CLI is implemented:

```bash
./sequential --servers data/servers.csv --requests data/requests.csv --policy scored
OMP_NUM_THREADS=4 ./openmp --servers data/servers.csv --requests data/requests.csv --policy scored
mpirun --bind-to none -np 4 ./mpi --servers data/servers.csv --requests data/requests.csv --policy scored
OMP_NUM_THREADS=2 mpirun --bind-to none -np 2 ./hybrid --servers data/servers.csv --requests data/requests.csv --policy scored
```

Explicitly document process binding and OpenMP affinity settings. The illustrative `--bind-to none` lets macOS schedule workers; it is not a claim of an optimal placement. Apple silicon core heterogeneity and operating-system scheduling can influence scaling. Record actual thread counts, ranks, machine details, and power/thermal conditions. Run modes sequentially, not simultaneously.

### Timing boundaries

Primary metric: core simulation wall time, including arrivals, necessary request communication, session release, evaluation, winner communication, and allocation. Exclude file loading, initial distribution, terminal printing, and final CSV writing from this metric for every backend. Report setup/distribution time separately, and optionally measure full process-launch-to-exit time externally for all modes.

For MPI, synchronize with a barrier before the core interval. Each rank measures its own elapsed time using `MPI_Wtime`; reduce elapsed values with `MPI_MAX` after the interval. Do not subtract timestamps from different ranks or sum rank runtimes. For local modes use a monotonic wall clock consistently. Include MPI communication in the reported runtime; never compare MPI compute-only time with OpenMP end-to-end time.

Instrument local evaluation time, collective time, and lifecycle time separately. Collective durations include waiting/synchronization, not solely network transfer. Sums of per-component maxima need not equal total maximum runtime; explain this when presenting breakdowns. Avoid extra diagnostic collectives inside timed runs.

### Repetitions and analysis

- Fixed input files, seeds, policy weights, and compiler optimization settings.
- Fresh initial state for every run.
- Warm-up, then at least five repetitions per selected configuration.
- Rotate execution order to reduce systematic thermal/order bias.
- Report median and interquartile range, retaining raw measurements.
- Use several saved workload seeds for policy robustness; distinguish seed variation from repeated-run timing variation.
- Strong scaling: fixed workload, increasing workers.
- Optional weak scaling: increase server count with workers, label workload changes explicitly and avoid interpreting it as strong scaling.

Formulas:

```text
speedup = sequential scored core time / backend scored core time
efficiency (%) = speedup / worker_budget * 100
worker_budget = OpenMP threads, MPI ranks, or hybrid ranks * threads
acceptance (%) = accepted / requests * 100
```

One-thread OpenMP and one-rank MPI expose framework overhead; the plain sequential executable remains the common speedup baseline. Capacity-weighted utilization is total used resource divided by total capacity. Time averages must integrate utilization over simulated event intervals, including completions between arrivals. Report modeled latency only for accepted requests, alongside acceptance rate, to avoid selection bias.

Suggested CSV columns:

```csv
backend,policy,servers,requests,ranks,threads,total_workers,seed,repeat,core_seconds,setup_seconds,accepted,rejected
```

Save optional component timings, per-resource utilization, and modeled latency statistics in additional CSVs.

### Required figures and tables

1. Core runtime versus equal worker budget: sequential, OpenMP, MPI, optional hybrid.
2. Speedup and efficiency versus worker budget.
3. Runtime versus server count, identifying crossover points if present.
4. MPI local evaluation versus communication/synchronization measurements.
5. First-fit versus scored acceptance, utilization, and modeled latency.
6. Correctness agreement table across backends and worker counts.
7. Experimental configuration table and variability/error bars.

Do not claim MPI is universally slower or OpenMP universally faster from single-machine results. Do not claim reduced real gameplay latency, improved FPS, or paper novelty without evidence.

## 13. Milestones and schedule

A realistic sequence for a team is about 12–15 working days, depending on C/MPI experience. Since the guideline's final window is the fourth week of October and this revision is dated October 19, 2026, confirm the actual remaining time. This schedule is not an extension granted by faculty.

| Stage | Suggested effort | Deliverable and exit gate |
|---|---|---|
| A: design/setup | 1 day | Proposal, architecture, OpenMP/MPI smoke tests |
| B: sequential engine | 2 days | Data loading, score, event heap, correct lifecycle |
| C: OpenMP | 1–2 days | Equivalent decisions, thread-safety checks |
| D: MPI | 2–3 days | Ownership, collectives, uneven/empty partitions, equivalence |
| E: benchmark harness | 1 day | Saved workloads, CSV output, repeatable runs |
| F: experiments | 2 days | Scaling, overhead, policy comparisons, figures |
| G: report/demo | 2–3 days | Both formats, slides, rehearsed live demo |
| Optional hybrid | 1–2 additional days | Thread-support checks and equal-budget comparison |

Survey work and report drafting should run alongside development. If fewer days remain, prioritize sequential + OpenMP + MPI, a focused experiment matrix, and the required documentation. Defer hybrid and UI, not correctness checks. Split team responsibilities by backend/data-analysis tasks, but maintain common interfaces and require all members to understand the full flow.

## 14. Literature, reports, and paper workflow

Review 8–10 papers from the last five years as required. Cover gaming/edge allocation, multi-resource scheduling, admission control, parallel optimization, and MPI/OpenMP comparisons where relevant. Record method, assumptions, hardware, workload, metrics, strengths, limitations, and relation to your work. Verify each bibliographic record. The survey is required work, not completed by this plan.

Use the same technical content in IEEE and Springer reports. Include all guideline sections: title, abstract, keywords, introduction, literature review, problem statement, objectives, proposed methodology, system architecture, algorithms, experimental setup, results, performance analysis, comparison with existing methods, conclusion, future scope, and references.

Explain the contribution cautiously: an implemented and evaluated multi-backend scheduler under stated workloads. Claim a research gap or novel method only after the survey supports it. Submit the paper to an appropriate venue after mentor review; retain genuine acknowledgement and subsequent acceptance evidence. Acceptance depends on external review and cannot be promised.

## 15. Demonstration and final acceptance

Demonstrate a small allocation, rejection, and resource-release example. Then run the same larger input in sequential, OpenMP, and MPI modes. Show exact output agreement, measured runtime, worker counts, overhead, and graphs. Show the optional hybrid only if validated. Every member explains their implementation and experimental contribution.

Final package: C sources, Makefile, README, tests, saved inputs/seeds, benchmark commands, raw CSVs, plots, literature matrix, IEEE/Springer reports, slides, team contribution log, and publication evidence required by faculty.

## Sources

- Primary course source: user-provided **Mini Project Guidelines_5_Aug_26.pdf**.
- [Homebrew Open MPI](https://formulae.brew.sh/formula/open-mpi)
- [Homebrew LLVM](https://formulae.brew.sh/formula/llvm)
- [Homebrew libomp](https://formulae.brew.sh/formula/libomp)
- [Open MPI documentation](https://docs.open-mpi.org/)
- [MPI thread initialization](https://docs.open-mpi.org/en/v5.0.0/man-openmpi/man3/MPI_Init_thread.3.html)
- [OpenMP specifications](https://www.openmp.org/specifications/)
