#!/bin/bash

# Number of MPI processes and OpenMP threads
NPROCS=4
NTHREADS=4

# Output folders
BIN_DIR="bin"
OUTPUT="timings.csv"

# Create bin directory if not exist and clean it
mkdir -p "$BIN_DIR"
rm -f "$BIN_DIR"/*

# Header for CSV
echo "Method,Function,Time,Result" > $OUTPUT

# Loop through 3 function variants
for FUNC_ID in 1 2 3
do
    echo "Running for FUNC_ID=$FUNC_ID..."

    # Update FUNC_ID in all C files
    sed -i "s/^#define FUNC_ID .*/#define FUNC_ID $FUNC_ID/" adaptive_simpson_serial.c
    sed -i "s/^#define FUNC_ID .*/#define FUNC_ID $FUNC_ID/" adaptive_simpson_omp.c
    sed -i "s/^#define FUNC_ID .*/#define FUNC_ID $FUNC_ID/" adaptive_simpson_mpi.c
    sed -i "s/^#define FUNC_ID .*/#define FUNC_ID $FUNC_ID/" adaptive_simpson_hybrid.c

    # Compile and move to bin directory
    gcc -o "$BIN_DIR/serial" adaptive_simpson_serial.c -lm
    gcc -fopenmp -o "$BIN_DIR/omp" adaptive_simpson_omp.c -lm
    mpicc -o "$BIN_DIR/mpi" adaptive_simpson_mpi.c -lm
    mpicc -fopenmp -o "$BIN_DIR/hybrid" adaptive_simpson_hybrid.c -lm

    # Run Serial
    out=$("$BIN_DIR/serial")
    time=$(echo "$out" | grep "Execution Time" | awk '{print $(NF-1)}')
    result=$(echo "$out" | grep "Integral" | awk '{print $3}')
    echo "Serial,Func$FUNC_ID,$time,$result" >> $OUTPUT

    # Run OpenMP
    export OMP_NUM_THREADS=$NTHREADS
    out=$("$BIN_DIR/omp")
    time=$(echo "$out" | grep "Execution Time" | awk '{print $(NF-1)}')
    result=$(echo "$out" | grep "Integral" | awk '{print $3}')
    echo "OpenMP,Func$FUNC_ID,$time,$result" >> $OUTPUT

    # Run MPI
    out=$(mpirun -np $NPROCS "$BIN_DIR/mpi")
    time=$(echo "$out" | grep "Execution Time" | awk '{print $(NF-1)}')
    result=$(echo "$out" | grep "Integral" | awk '{print $3}')
    echo "MPI,Func$FUNC_ID,$time,$result" >> $OUTPUT

    # Run Hybrid
    out=$(mpirun -np $NPROCS "$BIN_DIR/hybrid")
    time=$(echo "$out" | grep "Execution Time" | awk '{print $(NF-1)}')
    result=$(echo "$out" | grep "Integral" | awk '{print $3}')
    echo "Hybrid,Func$FUNC_ID,$time,$result" >> $OUTPUT

done

echo "All runs complete. Results saved to $OUTPUT"
echo "Binaries saved in ./$BIN_DIR/"
