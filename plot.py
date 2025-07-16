import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# --- Setup ---
plot_dir = "plots"
os.makedirs(plot_dir, exist_ok=True)

# Clear old plots
for f in os.listdir(plot_dir):
    os.remove(os.path.join(plot_dir, f))

# Load data
df = pd.read_csv("timings.csv")
df["Time"] = pd.to_numeric(df["Time"], errors='coerce')
df["Result"] = pd.to_numeric(df["Result"], errors='coerce')
df = df.dropna(subset=["Time", "Result"])

# Labels
functions = ["Func1", "Func2", "Func3"]
function_labels = {
    "Func1": "exp(sin(x²)) * log(x + 2)",
    "Func2": "sin(x²)/(x+1)",
    "Func3": "sin(x³)"
}
methods = ["Serial", "OpenMP", "MPI", "Hybrid"]

# Colors (from tab10)
color_map = plt.get_cmap("tab10")
method_colors = {method: color_map(i) for i, method in enumerate(methods)}

# --- Plot Helpers ---
def add_labels(ax, bars, fmt="{:.2f}"):
    for bar in bars:
        height = bar.get_height()
        if not np.isnan(height):
            ax.annotate(fmt.format(height), xy=(bar.get_x() + bar.get_width()/2, height),
                        xytext=(0, 3), textcoords="offset points",
                        ha='center', va='bottom', fontsize=8)

def plot_bar(x, y, ylabel, title, filename, color_map=None, label_fmt="{:.6f}"):
    plt.figure(figsize=(8, 6))
    colors_list = [color_map.get(m, "gray") if color_map else "blue" for m in x]
    bars = plt.bar(x, y, color=colors_list)
    for bar, val in zip(bars, y):
        if not pd.isna(val):
            plt.text(bar.get_x() + bar.get_width()/2, val * 1.01, label_fmt.format(val),
                     ha='center', fontsize=9)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(axis='y', linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(filename)
    plt.close()

# --- 1. Individual Plots ---
for func_id in functions:
    label = function_labels[func_id]
    subset = df[df["Function"] == func_id].set_index("Method").reindex(methods).reset_index()

    # Time
    plot_bar(
        subset["Method"],
        subset["Time"],
        ylabel="Execution Time (seconds)",
        title=f"Execution Time - {label}",
        filename=os.path.join(plot_dir, f"{func_id}_time.png"),
        color_map=method_colors
    )

    # Result
    plot_bar(
        subset["Method"],
        subset["Result"],
        ylabel="Integral Result",
        title=f"Integral Result - {label}",
        filename=os.path.join(plot_dir, f"{func_id}_result.png"),
        color_map=method_colors
    )

    # Speedup
    serial_time_vals = subset[subset["Method"] == "Serial"]["Time"].values
    if len(serial_time_vals) == 1:
        serial_time = serial_time_vals[0]
        speedups = serial_time / subset["Time"]
        plot_bar(
            subset["Method"],
            speedups,
            ylabel="Speedup (relative to Serial)",
            title=f"Speedup - {label}",
            filename=os.path.join(plot_dir, f"{func_id}_speedup.png"),
            color_map=method_colors,
            label_fmt="{:.2f}x"
        )

# --- 2. Combined Execution Time ---
fig, ax = plt.subplots(figsize=(10, 6))
bar_width = 0.2
x = np.arange(len(functions))

for i, method in enumerate(methods):
    times = []
    for func in functions:
        val = df[(df["Method"] == method) & (df["Function"] == func)]["Time"]
        times.append(val.iloc[0] if not val.empty else np.nan)
    bars = ax.bar(x + i * bar_width, times, width=bar_width, label=method, color=method_colors[method])
    add_labels(ax, bars)

ax.set_xticks(x + bar_width * 1.5)
ax.set_xticklabels([function_labels[f] for f in functions], rotation=20, ha='right')
ax.set_ylabel("Execution Time (seconds)")
ax.set_title("Execution Time Comparison for Each Function")
ax.legend()
ax.grid(axis='y', linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig(os.path.join(plot_dir, "combined_execution_time.png"))
plt.close()

# --- 3. Combined Speedup ---
fig, ax = plt.subplots(figsize=(10, 6))

for i, method in enumerate(methods):
    speedups = []
    for func in functions:
        serial_time = df[(df["Method"] == "Serial") & (df["Function"] == func)]["Time"].values
        method_time = df[(df["Method"] == method) & (df["Function"] == func)]["Time"].values
        if serial_time.size > 0 and method_time.size > 0:
            speedup = serial_time[0] / method_time[0]
            speedups.append(speedup)
        else:
            speedups.append(np.nan)
    bars = ax.bar(x + i * bar_width, speedups, width=bar_width, label=method, color=method_colors[method])
    add_labels(ax, bars, fmt="{:.2f}x")

ax.set_xticks(x + bar_width * 1.5)
ax.set_xticklabels([function_labels[f] for f in functions], rotation=20, ha='right')
ax.set_ylabel("Speedup (relative to Serial)")
ax.set_title("Speedup Comparison Across Methods")
ax.legend()
ax.grid(axis='y', linestyle='--', alpha=0.6)
plt.tight_layout()
plt.savefig(os.path.join(plot_dir, "combined_speedup.png"))
plt.close()

print("All plots saved in 'plots/' folder.")
