import os
import pandas as pd
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

os.environ['MPLCONFIGDIR'] = '/tmp/matplotlib'

def generate_plots():
    csv_path = 'results/benchmark_results.csv'
    if not os.path.exists(csv_path):
        print(f"Error: {csv_path} does not exist.")
        return

    df = pd.read_csv(csv_path)
    os.makedirs('plots', exist_ok=True)
    plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')

    # Aggregate medians over repeats
    agg_df = df.groupby(['backend', 'policy', 'servers', 'total_workers']).agg({
        'core_seconds': 'median',
        'accepted': 'median',
        'rejected': 'median',
        'avg_latency_ms': 'median',
        'peak_cpu_pct': 'median',
        'peak_gpu_pct': 'median'
    }).reset_index()

    print("Data summary:")
    print(agg_df[['backend', 'policy', 'servers', 'total_workers', 'core_seconds']])

    # -------------------------------------------------------------
    # Figure 1: Core Runtime vs Worker Budget (10k servers, scored)
    # -------------------------------------------------------------
    plt.figure(figsize=(8, 5), dpi=300)
    sub10k = agg_df[(agg_df['servers'] == 10000) & (agg_df['policy'] == 'scored')]
    seq_time = sub10k[sub10k['backend'] == 'Sequential']['core_seconds'].values[0]

    omp_data = sub10k[sub10k['backend'] == 'OpenMP'].sort_values('total_workers')
    mpi_data = sub10k[sub10k['backend'] == 'MPI'].sort_values('total_workers')
    hyb_data = sub10k[sub10k['backend'] == 'Hybrid'].sort_values('total_workers')

    plt.axhline(y=seq_time, color='black', linestyle='--', label=f'Sequential Baseline ({seq_time:.4f}s)')
    plt.plot(omp_data['total_workers'], omp_data['core_seconds'], marker='o', linewidth=2, color='#1f77b4', label='OpenMP (Shared Memory)')
    plt.plot(mpi_data['total_workers'], mpi_data['core_seconds'], marker='s', linewidth=2, color='#d62728', label='MPI (Distributed Ranks)')
    if not hyb_data.empty:
        plt.scatter(hyb_data['total_workers'], hyb_data['core_seconds'], color='#2ca02c', s=100, zorder=5, label='Hybrid (2 Ranks x 2 Threads)')

    plt.xlabel('Total Worker Budget (Threads / Ranks)', fontsize=12)
    plt.ylabel('Core Simulation Wall Time (seconds)', fontsize=12)
    plt.title('Figure 1: Core Runtime vs. Equal Worker Budget (10,000 Servers)', fontsize=13, fontweight='bold')
    plt.xticks([1, 2, 4])
    plt.legend(fontsize=10)
    plt.tight_layout()
    plt.savefig('plots/fig1_runtime_vs_workers.png')
    plt.close()
    print("Saved plots/fig1_runtime_vs_workers.png")

    # -------------------------------------------------------------
    # Figure 2: Speedup and Parallel Efficiency
    # -------------------------------------------------------------
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5), dpi=300)

    workers = [1, 2, 4]
    ideal_speedup = [1, 2, 4]
    ax1.plot(workers, ideal_speedup, 'k--', label='Ideal Linear Speedup')

    omp_speedup = [seq_time / t for t in omp_data['core_seconds'].values]
    mpi_speedup = [seq_time / t for t in mpi_data['core_seconds'].values]

    ax1.plot(omp_data['total_workers'], omp_speedup, marker='o', linewidth=2, color='#1f77b4', label='OpenMP')
    ax1.plot(mpi_data['total_workers'], mpi_speedup, marker='s', linewidth=2, color='#d62728', label='MPI')
    if not hyb_data.empty:
        hyb_sp = seq_time / hyb_data['core_seconds'].values[0]
        ax1.scatter(4, hyb_sp, color='#2ca02c', s=100, label=f'Hybrid ({hyb_sp:.2f}x)')

    ax1.set_xlabel('Worker Budget', fontsize=11)
    ax1.set_ylabel('Speedup (vs. Sequential C)', fontsize=11)
    ax1.set_title('(a) Speedup Comparison (10,000 Servers)', fontsize=12, fontweight='bold')
    ax1.set_xticks(workers)
    ax1.legend(fontsize=10)

    omp_eff = [sp / w * 100 for sp, w in zip(omp_speedup, workers)]
    mpi_eff = [sp / w * 100 for sp, w in zip(mpi_speedup, workers)]

    ax2.axhline(y=100, color='k', linestyle='--', label='100% Ideal Efficiency')
    ax2.plot(workers, omp_eff, marker='o', linewidth=2, color='#1f77b4', label='OpenMP Efficiency')
    ax2.plot(workers, mpi_eff, marker='s', linewidth=2, color='#d62728', label='MPI Efficiency')
    if not hyb_data.empty:
        ax2.scatter(4, hyb_sp / 4 * 100, color='#2ca02c', s=100, label=f'Hybrid ({hyb_sp/4*100:.1f}%)')

    ax2.set_xlabel('Worker Budget', fontsize=11)
    ax2.set_ylabel('Parallel Efficiency (%)', fontsize=11)
    ax2.set_title('(b) Parallel Efficiency (10,000 Servers)', fontsize=12, fontweight='bold')
    ax2.set_xticks(workers)
    ax2.legend(fontsize=10)

    plt.tight_layout()
    plt.savefig('plots/fig2_speedup_efficiency.png')
    plt.close()
    print("Saved plots/fig2_speedup_efficiency.png")

    # -------------------------------------------------------------
    # Figure 3: Runtime vs Server Count (Scaling & Crossover)
    # -------------------------------------------------------------
    plt.figure(figsize=(9, 5), dpi=300)
    servers_list = [100, 1000, 10000]

    seq_scaling = [agg_df[(agg_df['servers'] == s) & (agg_df['backend'] == 'Sequential') & (agg_df['policy'] == 'scored')]['core_seconds'].values[0] for s in servers_list]
    omp_scaling_4 = [agg_df[(agg_df['servers'] == s) & (agg_df['backend'] == 'OpenMP') & (agg_df['total_workers'] == 4)]['core_seconds'].values[0] for s in servers_list]
    mpi_scaling_4 = [agg_df[(agg_df['servers'] == s) & (agg_df['backend'] == 'MPI') & (agg_df['total_workers'] == 4)]['core_seconds'].values[0] for s in servers_list]

    plt.plot(servers_list, seq_scaling, marker='^', linewidth=2, color='black', label='Sequential Baseline')
    plt.plot(servers_list, omp_scaling_4, marker='o', linewidth=2, color='#1f77b4', label='OpenMP (4 Threads)')
    plt.plot(servers_list, mpi_scaling_4, marker='s', linewidth=2, color='#d62728', label='MPI (4 Ranks)')

    plt.xscale('log')
    plt.yscale('log')
    plt.xlabel('Number of Servers in Pool (log scale)', fontsize=12)
    plt.ylabel('Core Runtime (seconds, log scale)', fontsize=12)
    plt.title('Figure 3: Runtime Scaling vs. Server Pool Size', fontsize=13, fontweight='bold')
    plt.legend(fontsize=10)
    plt.tight_layout()
    plt.savefig('plots/fig3_scaling_servers.png')
    plt.close()
    print("Saved plots/fig3_scaling_servers.png")

    # -------------------------------------------------------------
    # Figure 4: Policy Comparison: Scored vs First-Fit
    # -------------------------------------------------------------
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(13, 5), dpi=300)

    pol_df = agg_df[agg_df['backend'] == 'Sequential']
    scored_sub = pol_df[pol_df['policy'] == 'scored'].sort_values('servers')
    ff_sub = pol_df[pol_df['policy'] == 'first_fit'].sort_values('servers')

    x = np.arange(len(servers_list))
    width = 0.35

    ax1.bar(x - width/2, scored_sub['accepted'], width, label='Scored Allocation', color='#2ca02c')
    ax1.bar(x + width/2, ff_sub['accepted'], width, label='First-Fit Baseline', color='#ff7f0e')
    ax1.set_xlabel('Server Pool Size', fontsize=11)
    ax1.set_ylabel('Accepted Requests (out of 1000)', fontsize=11)
    ax1.set_title('(a) Admission Success Rate', fontsize=12, fontweight='bold')
    ax1.set_xticks(x)
    ax1.set_xticklabels([str(s) for s in servers_list])
    ax1.legend(fontsize=10)

    ax2.bar(x - width/2, scored_sub['avg_latency_ms'], width, label='Scored Allocation', color='#2ca02c')
    ax2.bar(x + width/2, ff_sub['avg_latency_ms'], width, label='First-Fit Baseline', color='#ff7f0e')
    ax2.set_xlabel('Server Pool Size', fontsize=11)
    ax2.set_ylabel('Average Modeled Latency (ms)', fontsize=11)
    ax2.set_title('(b) Average Network Latency for Admitted Sessions', fontsize=12, fontweight='bold')
    ax2.set_xticks(x)
    ax2.set_xticklabels([str(s) for s in servers_list])
    ax2.legend(fontsize=10)

    plt.tight_layout()
    plt.savefig('plots/fig4_policy_comparison.png')
    plt.close()
    print("Saved plots/fig4_policy_comparison.png")

if __name__ == '__main__':
    generate_plots()
