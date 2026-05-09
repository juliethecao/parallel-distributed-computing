import csv
from pathlib import Path

import matplotlib.pyplot as plt

BASE = Path(__file__).resolve().parent
RESULTS = BASE / "results"
PLOTS = BASE / "plots"
PLOTS.mkdir(exist_ok=True)

plt.style.use("seaborn-v0_8-whitegrid")
plt.rcParams.update(
    {
        "figure.dpi": 160,
        "savefig.dpi": 160,
        "axes.titlesize": 14,
        "axes.labelsize": 11,
        "xtick.labelsize": 10,
        "ytick.labelsize": 10,
        "legend.fontsize": 10,
        "font.family": "DejaVu Sans",
    }
)


def read_csv(path):
    with path.open(newline="") as f:
        return list(csv.DictReader(f))


def to_float(value):
    return float(value) if value not in ("", None) else None


def savefig(name):
    plt.tight_layout()
    out = PLOTS / name
    plt.savefig(out, bbox_inches="tight")
    plt.close()
    print(f"saved {out}")


# Figure 1: Sequential time vs matrix size
rows = read_csv(RESULTS / "exp1_sequential.csv")
n = [int(r["n"]) for r in rows]
time_seq = [to_float(r["time"]) for r in rows]

plt.figure(figsize=(7.5, 4.6))
plt.plot(n, time_seq, marker="o", linewidth=2.2, color="#1f77b4")
plt.title("Sequential Transpose Time vs Matrix Size")
plt.xlabel("Matrix size n")
plt.ylabel("Execution time (s)")
plt.xscale("log")
plt.yscale("log")
plt.grid(True, which="both", alpha=0.25)
savefig("fig1_sequential_time_vs_size.png")

# Figure 2: Fine-grain time vs threads
rows = read_csv(RESULTS / "exp2_finegrain.csv")
threads = [int(r["threads"]) for r in rows]
fine = [to_float(r["time"]) for r in rows]

plt.figure(figsize=(7.5, 4.6))
plt.plot(threads, fine, marker="o", linewidth=2.2, color="#d62728")
plt.title("Fine-Grain Parallel Time vs Thread Count (n = 7000, grain = 1)")
plt.xlabel("Threads")
plt.ylabel("Execution time (s)")
plt.xscale("log", base=2)
plt.yscale("log")
plt.grid(True, which="both", alpha=0.25)
savefig("fig2_finegrain_time_vs_threads.png")

# Figure 3: Coarse-grain time vs threads
rows = read_csv(RESULTS / "exp3_coarsegrain.csv")
threads = [int(r["threads"]) for r in rows]
grain_n = [to_float(r["time_grain_n"]) for r in rows]
grain_bal = [to_float(r["time_grain_n2_div_t"]) for r in rows]

plt.figure(figsize=(7.5, 4.6))
plt.plot(threads, grain_n, marker="o", linewidth=2.2, label="grain = n", color="#2ca02c")
plt.plot(threads, grain_bal, marker="s", linewidth=2.2, label="grain = (n²)/threads", color="#ff7f0e")
plt.title("Coarse-Grain Parallel Time vs Thread Count (n = 7000)")
plt.xlabel("Threads")
plt.ylabel("Execution time (s)")
plt.xscale("log", base=2)
plt.yscale("log")
plt.grid(True, which="both", alpha=0.25)
plt.legend(frameon=True)
savefig("fig3_coarsegrain_time_vs_threads.png")

# Figure 4: Speedup vs threads
rows = read_csv(RESULTS / "exp4_speedup.csv")
threads = [int(r["threads"]) for r in rows]
fine_speed = [to_float(r["speedup_fine"]) for r in rows]
coarse_speed = [to_float(r["speedup_coarse"]) for r in rows]

plt.figure(figsize=(7.5, 4.6))
plt.plot(threads, fine_speed, marker="o", linewidth=2.2, label="Fine-grain speedup", color="#d62728")
plt.plot(threads, coarse_speed, marker="s", linewidth=2.2, label="Coarse-grain speedup", color="#2ca02c")
plt.axhline(1.0, linestyle="--", color="gray", linewidth=1.2)
plt.title("Speedup vs Thread Count (n = 7000)")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.xscale("log", base=2)
plt.grid(True, which="both", alpha=0.25)
plt.legend(frameon=True)
savefig("fig4_speedup_vs_threads.png")

# Figure 5: Speedup vs matrix size
rows = read_csv(RESULTS / "exp5_size_speedup.csv")
n = [int(r["n"]) for r in rows]
speed = [to_float(r["speedup"]) for r in rows]
seq = [to_float(r["time_seq"]) for r in rows]
coarse = [to_float(r["time_coarse"]) for r in rows]

plt.figure(figsize=(7.5, 4.6))
plt.plot(n, speed, marker="o", linewidth=2.2, color="#9467bd")
plt.axhline(1.0, linestyle="--", color="gray", linewidth=1.2)
plt.title("Parallel Speedup vs Matrix Size (grain = n, 4 threads)")
plt.xlabel("Matrix size n")
plt.ylabel("Speedup")
plt.xscale("log")
plt.grid(True, which="both", alpha=0.25)
savefig("fig5_speedup_vs_size.png")

# Optional summary plot: time comparison for speedup experiment
plt.figure(figsize=(7.5, 4.6))
plt.plot(n, seq, marker="o", linewidth=2.2, label="Sequential", color="#1f77b4")
plt.plot(n, coarse, marker="s", linewidth=2.2, label="Coarse-grain parallel", color="#2ca02c")
plt.title("Execution Time vs Matrix Size (Sequential vs Parallel)")
plt.xlabel("Matrix size n")
plt.ylabel("Execution time (s)")
plt.xscale("log")
plt.yscale("log")
plt.grid(True, which="both", alpha=0.25)
plt.legend(frameon=True)
savefig("fig6_time_vs_size_comparison.png")
