import pandas as pd
import matplotlib.pyplot as plt
import os

# Load the data
df = pd.read_csv("perf_threads.csv")

# Clean duplicate entries by averaging them
df = df.groupby(['Method', 'Workers'], as_index=False).mean()

# Create output folder for plots
output_dir = "thread_plots"
os.makedirs(output_dir, exist_ok=True)

# Plot Execution Time vs Workers
plt.figure(figsize=(10, 6))

methods = df["Method"].unique()
for method in methods:
    subset = df[df["Method"] == method]
    x = subset["Workers"].to_numpy()
    y = subset["Time"].to_numpy()
    plt.plot(x, y, marker='o', label=method)

plt.title("Execution Time vs Number of Threads / Processes")
plt.xlabel("Number of Threads / MPI Processes")
plt.ylabel("Execution Time (seconds)")
plt.grid(True)
plt.legend()

# Save and show plot
plt.savefig(os.path.join(output_dir, "threads_vs_time.png"))
# plt.show()

print(f"\nPlot saved to ./{output_dir}/threads_vs_time.png")
