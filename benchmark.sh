#!/bin/bash

FUNC_ID=2
echo "Method,Workers,Time" > perf_threads.csv

# Update FUNC_ID in all source files
for file in adaptive_simpson_*.c; do
    sed -i "s/^#define FUNC_ID .*/#define FUNC_ID $FUNC_ID/" "$file"
done

# Build
gcc -fopenmp -o bin/omp adaptive_simpson_omp.c -lm
mpicc -o bin/mpi adaptive_simpson_mpi.c -lm
mpicc -fopenmp -o bin/hybrid adaptive_simpson_hybrid.c -lm

# Test OpenMP
for T in 1 2 4 6; do
    export OMP_NUM_THREADS=$T
    TIME=$(./bin/omp | grep "Execution Time" | awk '{print $(NF-1)}')
    echo "OpenMP,$T,$TIME" >> perf_threads.csv
done

# Test MPI
for P in 1 2 4 6; do
    TIME=$(mpirun -np $P ./bin/mpi | grep "Execution Time" | awk '{print $(NF-1)}')
    echo "MPI,$P,$TIME" >> perf_threads.csv
done

# Test Hybrid (proc × threads)
for P in 2 4; do
    for T in 2 4; do
        export OMP_NUM_THREADS=$T
        WORKERS=$((P*T))
        TIME=$(mpirun -np $P ./bin/hybrid | grep "Execution Time" | awk '{print $(NF-1)}')
        echo "Hybrid,$WORKERS,$TIME" >> perf_threads.csv
    done
done

echo "Benchmarking complete."
