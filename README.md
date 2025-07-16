# Adaptive Simpson's Rule Parallelization

A high-performance implementation and analysis of the **Adaptive Simpson's Rule** for numerical integration using **Serial**, **OpenMP**, **MPI**, and **Hybrid (MPI + OpenMP)** parallel programming techniques.

---

## 📌 Project Overview

This project evaluates the performance and accuracy of various parallel programming models applied to the recursive Adaptive Simpson's Rule. It includes:

- Recursive integration of complex functions.
- Parallel implementations using:
  - OpenMP (shared memory)
  - MPI (distributed memory)
  - Hybrid MPI + OpenMP (combined parallelism)
- Performance evaluation (execution time, speedup)
- Accuracy validation (RMSE compared to serial)

---

## 📈 Functions Integrated

The following computationally heavy mathematical functions were used:

- **Func1**: exp(sin(x²)) · log(x + 2)
- **Func2**: sin(x²) / (x + 1)
- **Func3**: sin(x³)

Each function was integrated over large intervals to observe performance benefits.

---

## 🛠️ Build and Run Instructions

### 🔧 Requirements

- GCC and `mpicc` (for MPI)
- OpenMP support
- Bash shell
- Python 3 + `matplotlib`, `pandas`, `numpy`

### 🔨 Build All Versions

```bash
bash run.sh
```
### This script:

- Sets the function ID (1, 2, or 3)
- Compiles all versions: serial, OpenMP, MPI, Hybrid
- Executes and benchmarks them
- Outputs results to `timings.csv`
- Saves executables to `bin/`

## 📊 Benchmarking with Varying Threads

To evaluate performance by changing number of threads/processes:

```bash
bash benchmark.sh
```
Outputs results to `perf_threads.csv`.

## 📊 Visualization
Python scripts are provided to generate performance plots:

```bash
python3 plot_all.py
```
This generates:

- Per-function execution time, result, and speedup plots
- Combined plots comparing all methods across functions
- Saved under: `plots/`

## 📁 Repository Structure

```
.
├── adaptive_simpson_serial.c        # Serial implementation
├── adaptive_simpson_omp.c           # OpenMP implementation
├── adaptive_simpson_mpi.c           # MPI implementation
├── adaptive_simpson_hybrid.c        # Hybrid (MPI + OpenMP)
├── run_all.sh                       # Compile & benchmark all
├── benchmark_threads.sh             # Scalability testing
├── timings.csv                      # Benchmark results (method vs func)
├── perf_threads.csv                 # Threads/processes performance data
├── plot.py                          # Visualization script
├── plots/                           # Output plots
└── README.md                        # You're here
```

---

## 📈 Sample Results

| Method | Func1 Time (s) | Func2 Time (s) | Func3 Time (s) |
|--------|----------------|----------------|----------------|
| Serial | 11.81          | 54.00          | 1.73           |
| OpenMP | 4.31           | 21.96          | 0.66           |
| MPI    | 6.45           | 27.53          | 1.15           |
| Hybrid | **2.76**       | **13.22**      | **0.54**       |

---

## ✅ Key Findings

- ✅ All parallel implementations achieved **identical accuracy** (`RMSE = 0`).
- 🚀 **Hybrid MPI + OpenMP** delivered the **best speedup**, up to **4.28×**.
- 🔬 Ideal for performance-critical **scientific integration problems**.

---

## 👨‍💻 Author

**S.A.N.D. Ekanayaka**  
Undergraduate | Faculty of Engineering  
**University of Ruhuna**


