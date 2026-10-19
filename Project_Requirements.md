# Project Requirements

## Project identity

**Title:** Parallel Cloud Gaming Resource Management System Using C, OpenMP, and MPI

**Course:** Parallel Computing Architecture Programming Lab Mini Project

**Scope:** Build a simulator that assigns gaming sessions to servers according to available resources and simulated latency. Implement the same allocation algorithm in sequential C, OpenMP, and MPI, then compare correctness, computation cost, communication overhead, and scaling. Add an MPI + OpenMP hybrid version after the three required modes are validated.

The project simulates resource management; it does not render games or stream video. C is the programming language. OpenMP and MPI are the selected parallel programming technologies. Resource allocation is the core project feature. The guidelines permit one or more parallel technologies. This expanded plan deliberately uses both OpenMP and MPI; combining them in one hybrid executable is an additional extension. Confirm the expanded scope and exact deadline with your mentor.

## 1. Hardware and software checklist

| Requirement | Selected setup or purpose |
|---|---|
| Computer | Your Apple silicon MacBook Air |
| Storage | Your 512 GB total capacity is sufficient if adequate free space remains for tools and results |
| Memory | Record installed RAM in the report; choose workload sizes that fit it |
| Language | C; use `.c` source files |
| Compiler | Homebrew LLVM Clang with OpenMP support |
| OpenMP runtime | Homebrew `libomp` |
| MPI implementation | Homebrew `open-mpi`, including `mpicc` and `mpirun` |
| Optional cluster | Two or more accessible lab computers for a separate multi-node experiment; not required for local MPI |
| Development tools | Xcode Command Line Tools and Homebrew |
| Editor | VS Code or another C editor |
| Version control | Git; GitHub is encouraged by the guidelines |
| Timing | `omp_get_wtime()` for elapsed wall-clock time |
| Data format | CSV inputs and results |
| Graphs | Spreadsheet software or optional Python; core implementation remains C |
| Documentation | Markdown notes, IEEE and Springer report templates, presentation slides |

A paid cloud account, physical GPU, real gaming server, CUDA, and graphical dashboard are not required for this scope. MPI is now part of the core project. Local MPI ranks are separate processes on one MacBook; they do not establish multi-machine performance. OpenMP runs the scheduler on the CPU; GPU capacity is a simulated quantity.

## 2. Functional requirements

- [ ] Load or generate server configurations and gaming-session requests.
- [ ] Process requests in arrival-time order, breaking arrival ties by request ID.
- [ ] Check CPU, GPU, RAM, bandwidth, and simulated latency constraints.
- [ ] Score eligible servers and choose the best one deterministically.
- [ ] Reserve resources for accepted sessions.
- [ ] Reject requests when no eligible server exists.
- [ ] Release resources when sessions finish.
- [ ] Provide sequential, OpenMP, and MPI modes of the same scored scheduler; add hybrid after validation.
- [ ] Include first-fit as a simple allocation-policy baseline.
- [ ] Export decisions, timing, acceptance rates, and utilization measurements.

## 3. Data requirements

| Structure | Fields |
|---|---|
| Server | ID, region, total and available CPU, GPU, RAM, and bandwidth |
| Request | ID, player region, arrival time, duration, resource demands, latency limit |
| Session | Request ID, assigned server ID, end time, reserved resources |
| Metrics | Accepted/rejected counts, scheduling time, total simulation runtime, utilization |
| Latency table | Modeled network delay between each player region and server region |

Use abstract CPU/GPU capacity units, GB for memory, Mbps for bandwidth, and milliseconds for latency. Document assumptions explicitly. Synthetic input values are not measured hardware requirements for real games.

## 4. Correctness and performance requirements

- Available resources must stay between zero and the server's total capacity.
- Every accepted request must satisfy all resource and latency constraints.
- Resources must be released exactly once per completed session.
- Sequential, OpenMP, MPI, and any hybrid scored schedulers must produce the same assignments with the same input and tie-breaker.
- Validate malformed inputs, nonpositive capacities/latency limits, invalid regions, and invalid durations.
- Use identical input files and seeds across comparisons.
- Measure scheduler time separately from total simulator runtime.
- Keep file loading and console output outside scheduler timing.
- Repeat benchmarks at least five times; report median and variability.
- Report actual speedup, including slowdowns for small workloads.

## 5. Guideline deliverables and marks

| Component | Marks | Required content |
|---|---:|---|
| Title and proposal | 5 | Title, team details, problem, motivation, objectives, parallel model, methodology, expected outcomes, tentative plan |
| Literature survey, objectives, methodology | 5 | 8–10 papers from the last five years, survey comparison, research gaps, objectives, methodology, architecture/flowchart, IEEE references |
| Results, analysis, final report | 10 | Implementation, screenshots, graphs, comparisons, speedup/efficiency, result tables |
| Presentation and demonstration | 10 | Working code, input/output, performance, correctness, contributions of every member |
| Paper submission proof | 5 | Submission acknowledgement or other accepted documentary proof |
| Paper acceptance proof | 5 | Acceptance email/letter or other accepted proof; follow faculty instructions for delayed acceptance |
| **Total** | **40** | |

### Proposal checklist

- [ ] Final project title and team details.
- [ ] Problem statement and motivation.
- [ ] Objectives: correct allocation; equivalent OpenMP and MPI implementations; fair comparisons at equal worker counts; reproducible evaluation of speedup and overhead.
- [ ] Parallel models: C with OpenMP shared-memory threads and MPI message-passing processes; optional hybrid combines both.
- [ ] Methodology, expected outcomes, and implementation schedule.

### Literature survey checklist

Review 8–10 papers published within the last five years relative to submission, preferably from the sources listed in the guidelines: IEEE, ACM, Springer Nature, Elsevier, MDPI, Wiley, or Scopus-indexed venues.

Suggested search topics: cloud gaming resource allocation, latency-aware scheduling, GPU resource sharing, multi-resource admission control, parallel scheduling, load balancing, and shared-memory versus message-passing scheduling. Do not compare reported speedups across papers without considering their different workloads and hardware.

For each paper record: title/year, problem, algorithm, experimental setup, advantages, limitations, performance results, and relevance. Establish research gaps from the reviewed evidence; do not assume the proposed heuristic is novel.

### Final report checklist

Prepare technically identical reports in both IEEE conference and Springer conference formats. Include:

1. Title
2. Abstract
3. Keywords
4. Introduction
5. Literature review
6. Problem statement
7. Objectives
8. Proposed methodology
9. System architecture
10. Algorithms
11. Experimental setup
12. Results
13. Performance analysis
14. Comparison with existing methods
15. Conclusion
16. Future scope
17. References

### Final submission package

- [ ] Documented C source code, separate sequential/OpenMP/MPI builds, and reproducible build/run instructions.
- [ ] Input datasets and generation parameters/seeds.
- [ ] Correctness checks and repeatable demonstration.
- [ ] Raw results, tables, screenshots, and graphs.
- [ ] Literature survey and references.
- [ ] IEEE and Springer reports.
- [ ] Presentation and team contribution statement.
- [ ] Paper submission and acceptance evidence as applicable.

## 6. Guideline timeline

| Period | Activity |
|---|---|
| Second week of August | Team formation and mentor allocation |
| Fourth week of August | Title, proposal, survey, objectives, methodology |
| Second week of September | Initial implementation and architecture review |
| Fourth week of September | Major implementation and preliminary results |
| Second week of October | Final implementation, analysis, report draft |
| Fourth week of October | Final report, presentation, demonstration |
| As notified by faculty | Paper submission/acceptance evidence |

The guidelines also specify progress submissions every two weeks after the initial proposal. Confirm exact dates with your mentor. All team members must contribute; cite sources, document code, and avoid plagiarism.

## Sources and companion document

- Primary course source: user-provided **Mini Project Guidelines_5_Aug_26.pdf**.
- [Homebrew LLVM](https://formulae.brew.sh/formula/llvm)
- [Homebrew OpenMP runtime](https://formulae.brew.sh/formula/libomp)
- [OpenMP specifications](https://www.openmp.org/specifications/)
- Implementation details: **Implementation_Plan.md**.

## 7. Expanded scope and assessment evidence

The earlier OpenMP plan covered the guideline categories, but this revision expands the implementation and experimental depth. More technologies do not automatically earn more marks. The 40 marks include 10 for publication evidence; implementation alone cannot satisfy those components or guarantee full marks.

| Assessment | Concrete evidence to prepare |
|---|---|
| Proposal: 5 | Explicit research questions, defined simulation boundaries, sequential/OpenMP/MPI design, feasibility and schedule |
| Survey/methodology: 5 | 8–10 verified recent papers, comparison matrix, evidence-based gap, architecture, algorithm and partitioning rationale |
| Results/report: 10 | Correctness agreement, repeatable benchmarks, strong scaling, equal-worker comparisons, overhead analysis, policy comparison, limitations, both report formats |
| Presentation/demo: 10 | Live execution of all three core modes, allocation/release/rejection cases, results explanation, contribution log, all members participating |
| Submission proof: 5 | Actual acknowledgement from an appropriate venue after faculty review |
| Acceptance proof: 5 | Genuine acceptance documentation; delayed acceptance handled by faculty under the guideline note |

### Implementation acceptance criteria

- [ ] Sequential C is the reference implementation.
- [ ] OpenMP distributes server checks across threads.
- [ ] MPI partitions servers across ranks, communicates requests and winning candidates, and updates only the owning rank's server state.
- [ ] Uneven partitions and ranks with zero servers work correctly.
- [ ] All ranks participate in collectives in the same order, including rejected requests and termination.
- [ ] Same inputs produce the same allocation log and final state across execution modes.
- [ ] Worker budgets are equal when comparing OpenMP and MPI: T threads versus P ranks where T = P.
- [ ] MPI timings include required communication; per-rank elapsed times are reduced with a maximum.
- [ ] At least five repeated runs per chosen configuration, with median and variability.
- [ ] At least three meaningful workload patterns and three server-pool scales, subject to pilot runtime.
- [ ] Data, commands, seeds, compiler details, and raw results are retained.
- [ ] Optional hybrid is compared at P × T total workers, not only at the same rank count.

### Research questions

1. At what server-pool size does OpenMP overcome thread coordination overhead?
2. How does MPI compare with OpenMP for the same scheduler and total workers on one Apple silicon machine?
3. How much of MPI runtime is spent in local evaluation versus communication/synchronization?
4. How do offered load and resource-demand mix affect acceptance and utilization?
5. If hybrid is implemented, which rank/thread split works best at a fixed worker budget?

### Recommended versus optional scope

Required for this expanded plan: sequential + OpenMP + MPI; resource lifecycle; reproducible workload generator; first-fit baseline; correctness tests; benchmark harness; full assessment documentation.

Optional extension: MPI + OpenMP hybrid. Multi-node testing is valuable only if lab access and time permit. A dashboard, real streaming, dynamic migration, and CUDA should not displace correctness or analysis.

Additional official resources: [Open MPI installation via Homebrew](https://formulae.brew.sh/formula/open-mpi), [Open MPI documentation](https://docs.open-mpi.org/).
