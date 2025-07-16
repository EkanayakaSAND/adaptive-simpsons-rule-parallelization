#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>

#define FUNC_ID 2

// Function to integrate
double f(double x)
{
#if FUNC_ID == 1
    return exp(sin(x * x)) * log(x + 2);

#elif FUNC_ID == 2
    return sin(x * x) / (x + 1);

#elif FUNC_ID == 3
    return sin(x * x * x);

#endif
}

// Simpson's rule over interval [a, b]
double simpson(double a, double b)
{
    double c = (a + b) / 2.0;
    return (b - a) / 6.0 * (f(a) + 4.0 * f(c) + f(b));
}

// Recursive Adaptive Simpson’s Method
double adaptive_simpson(double a, double b, double eps, double whole)
{
    double c = (a + b) / 2.0;
    double left = simpson(a, c);
    double right = simpson(c, b);

    if (fabs(left + right - whole) <= 15 * eps)
        return left + right + (left + right - whole) / 15.0;

    return adaptive_simpson(a, c, eps / 2.0, left) +
           adaptive_simpson(c, b, eps / 2.0, right);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv); // Initialize MPI

    int rank, size;
#if FUNC_ID == 1
    double a = 0.0, b = 1000.0;
#elif FUNC_ID == 2
    double a = 0.0, b = 5000.0;
#elif FUNC_ID == 3
    double a = 0.0, b = 200.0;
#endif
    double eps = 1e-6;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int threads = 4;     // Number of OpenMP threads per MPI process
    double result = 0.0; // Local result for each MPI process

    // Determine local subinterval [local_a, local_b] for this MPI process
    double local_a = a + rank * (b - a) / size;
    double local_b = a + (rank + 1) * (b - a) / size;

    // Start timing (only root process will report it)
    double start = MPI_Wtime();

    // Parallelize over OpenMP threads within each MPI process
#pragma omp parallel for reduction(+ : result) num_threads(threads)
    for (int i = 0; i < threads; i++)
    {
        // Divide [local_a, local_b] into thread-local subintervals
        double sub_a = local_a + i * (local_b - local_a) / threads;
        double sub_b = local_a + (i + 1) * (local_b - local_a) / threads;

        double whole = simpson(sub_a, sub_b);
        result += adaptive_simpson(sub_a, sub_b, eps / (size * threads), whole);
    }

    // Reduce all partial results to root process (rank 0)
    double global_result = 0.0;
    MPI_Reduce(&result, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    // Only root process prints the final result
    if (rank == 0)
    {
        printf("\nIntegral = %.12f\n", global_result);
        printf("Execution Time: %.6f seconds\n", end - start);
        printf("Number of MPI Processes Used: %d\n", size);
        printf("Number of OpenMP Threads per Process: %d\n\n", threads);
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}

/*
 * mpicc -fopenmp adaptive_simpson_hybrid.c -o hybrid -lm
 * mpirun -np 4 ./hybrid
 */