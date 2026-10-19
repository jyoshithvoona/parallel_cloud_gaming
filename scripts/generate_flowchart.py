import os
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.patches as patches

os.environ['MPLCONFIGDIR'] = '/tmp/matplotlib'

def draw_flowchart():
    fig, ax = plt.subplots(figsize=(14, 18), dpi=300)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis('off')

    # Color palette
    c_title = '#1a365d'
    c_input = '#ebf8ff'
    c_input_border = '#3182ce'
    c_event = '#feebc8'
    c_event_border = '#dd6b20'
    c_kernel = '#faf5ff'
    c_kernel_border = '#805ad5'
    c_backend = '#f0fff4'
    c_backend_border = '#38a169'
    c_output = '#edf2f7'
    c_output_border = '#4a5568'

    # Title
    ax.text(50, 97.5, "Parallel Cloud Gaming Resource Management Architecture",
            ha='center', va='center', fontsize=18, fontweight='bold', color=c_title)
    ax.text(50, 95.5, "Discrete-Event Simulation Workflow Across Sequential, OpenMP, MPI, and Hybrid Backends",
            ha='center', va='center', fontsize=12, style='italic', color='#4a5568')

    # Box 1: Input Datasets
    b_input = patches.FancyBboxPatch((5, 84), 90, 8.5, boxstyle="round,pad=0.8",
                                     fc=c_input, ec=c_input_border, linewidth=2)
    ax.add_patch(b_input)
    ax.text(50, 90.5, "1. WORKLOAD & INPUT REPOSITORY (CSV DATASETS)",
            ha='center', va='center', fontsize=12, fontweight='bold', color='#2b6cb0')
    ax.text(20, 86.5, "[Servers Dataset]\nID, Region, Total & Avail\nCPU, GPU, RAM, Bandwidth",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.3", fc="white", ec="#cbd5e0"))
    ax.text(50, 86.5, "[Network Latency Matrix]\n4x4 Regional Delay Model\nDelays: 10ms to 210ms",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.3", fc="white", ec="#cbd5e0"))
    ax.text(80, 86.5, "[Gaming Requests]\nID, Region, Arrival Time,\nDuration, Demands, Latency Limit",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.3", fc="white", ec="#cbd5e0"))

    # Arrow Down
    ax.annotate("", xy=(50, 80), xytext=(50, 84),
                arrowprops=dict(arrowstyle="->", lw=2.5, color="#2d3748"))

    # Box 2: Discrete-Event Simulator Engine
    b_event = patches.FancyBboxPatch((5, 68), 90, 12, boxstyle="round,pad=0.8",
                                     fc=c_event, ec=c_event_border, linewidth=2)
    ax.add_patch(b_event)
    ax.text(50, 78, "2. DISCRETE-EVENT ENGINE & LIFECYCLE MANAGEMENT",
            ha='center', va='center', fontsize=12, fontweight='bold', color='#c05621')
    ax.text(50, 75, "Process arrivals in strict (Arrival_Time, Request_ID) order",
            ha='center', va='center', fontsize=10, style='italic')

    # Event Heap Detail
    ax.text(25, 71.5, "Session Completion Min-Heap\nPriority Queue sorted by End_Time\n(end_time = arrival_time + duration)",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#fbd38d"))
    ax.text(75, 71.5, "Dynamic Session Release\nFor all sessions where end_time <= current_time:\nPop session -> Restore CPU, GPU, RAM, BW",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#fbd38d"))

    # Arrow Down
    ax.annotate("", xy=(50, 64), xytext=(50, 68),
                arrowprops=dict(arrowstyle="->", lw=2.5, color="#2d3748"))

    # Box 3: Candidate Scoring & Feasibility Kernel
    b_kernel = patches.FancyBboxPatch((5, 52), 90, 12, boxstyle="round,pad=0.8",
                                      fc=c_kernel, ec=c_kernel_border, linewidth=2)
    ax.add_patch(b_kernel)
    ax.text(50, 62, "3. SHARED SCORING KERNEL & DETERMINISTIC COMPARISON",
            ha='center', va='center', fontsize=12, fontweight='bold', color='#6b46c1')

    ax.text(25, 56.5, "Feasibility Checking\n- Latency <= Request Latency Limit\n- Server Avail_Res >= Demand_Res\n(Checked across CPU, GPU, RAM, BW)",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#d6bcfa"))
    ax.text(75, 56.5, "Deterministic Score Metric\nScore = 0.5 * (Latency / Limit) + 0.5 * Max_Utilization\nTie-Breaker: Lowest Score wins;\nExact ties break by smallest Global Server ID",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#d6bcfa"))

    # Arrow Down
    ax.annotate("", xy=(50, 48), xytext=(50, 52),
                arrowprops=dict(arrowstyle="->", lw=2.5, color="#2d3748"))

    # Box 4: 4 Execution Backends
    b_backend = patches.FancyBboxPatch((5, 18), 90, 30, boxstyle="round,pad=0.8",
                                       fc=c_backend, ec=c_backend_border, linewidth=2)
    ax.add_patch(b_backend)
    ax.text(50, 46, "4. MULTI-MODEL PARALLEL SEARCH BACKENDS",
            ha='center', va='center', fontsize=12, fontweight='bold', color='#276749')

    # 4 Columns for Backends
    # Sequential
    ax.text(16, 32, "SEQUENTIAL C\n(Reference Baseline)\n\n- Single-threaded scan\n- Iterates 0 to N-1\n- First-Fit early exit\n- Correctness oracle\n- Common denominator\n  for speedup analysis",
            ha='center', va='center', fontsize=8.5, bbox=dict(boxstyle="round,pad=0.5", fc="white", ec="#9ae6b4"))
    # OpenMP
    ax.text(39, 32, "C + OPENMP\n(Shared Memory)\n\n- Static loop scheduling\n  #pragma omp for\n- Thread-private best\n  candidate search\n- Lock-free evaluation\n- Single merge at end\n- Zero false sharing",
            ha='center', va='center', fontsize=8.5, bbox=dict(boxstyle="round,pad=0.5", fc="white", ec="#9ae6b4"))
    # MPI
    ax.text(62, 32, "C + MPI\n(Message Passing)\n\n- Contiguous N/P split\n- Disjoint server ownership\n- MPI_Bcast request data\n- Local partition search\n- MPI_Allreduce\n  (MPI_MINLOC reduction)\n- Only owner reserves",
            ha='center', va='center', fontsize=8.5, bbox=dict(boxstyle="round,pad=0.5", fc="white", ec="#9ae6b4"))
    # Hybrid
    ax.text(84, 32, "HYBRID MPI+OpenMP\n(Hierarchical)\n\n- MPI_THREAD_FUNNELED\n- MPI partitions clusters\n- OpenMP threads evaluate\n  rank's local partition\n- Collective MPI_Allreduce\n- Fixed budget comparisons\n  (P x T = W)",
            ha='center', va='center', fontsize=8.5, bbox=dict(boxstyle="round,pad=0.5", fc="white", ec="#9ae6b4"))

    # Arrow Down
    ax.annotate("", xy=(50, 14), xytext=(50, 18),
                arrowprops=dict(arrowstyle="->", lw=2.5, color="#2d3748"))

    # Box 5: Outputs, Verification, and Performance Metrics
    b_output = patches.FancyBboxPatch((5, 2), 90, 12, boxstyle="round,pad=0.8",
                                      fc=c_output, ec=c_output_border, linewidth=2)
    ax.add_patch(b_output)
    ax.text(50, 12, "5. DECISION LOGGING, CORRECTNESS GATES & RESULTS ANALYSIS",
            ha='center', va='center', fontsize=12, fontweight='bold', color='#2d3748')

    ax.text(25, 6.5, "[Bitwise Verification Suite]\n- 12 Correctness Gates Passed\n- 100% Identical Decision Logs\n(Sequential == OpenMP == MPI == Hybrid)",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#a0aec0"))
    ax.text(75, 6.5, "[Performance & Publication Outputs]\n- Core Wall Time & Speedup Curves\n- Latency & Resource Utilization CSVs\n- IEEE & Springer Conference Reports",
            ha='center', va='center', fontsize=9, bbox=dict(boxstyle="square,pad=0.4", fc="white", ec="#a0aec0"))

    plt.tight_layout()
    os.makedirs('plots', exist_ok=True)
    out_path = 'plots/architecture_flowchart.png'
    plt.savefig(out_path, dpi=300)
    plt.close()
    print(f"Flowchart successfully saved to {out_path}")

if __name__ == '__main__':
    draw_flowchart()
