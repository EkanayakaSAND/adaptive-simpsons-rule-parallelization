#include <stdio.h>
#include <math.h>
#include <mpi.h>

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

// Standard Simpson’s rule for interval [a, b]
double simpson(double a, double b)
{
    double c = (a + b) / 2.0;
    return (b - a) / 6.0 * (f(a) + 4.0 * f(c) + f(b));
}

// Recursive Adaptive Simpson’s Rule
double adaptive_simpson(double a, double b, double eps, double whole)
{
    double c = (a + b) / 2.0;
    double left = simpson(a, c);
    double right = simpson(c, b);

    if (fabs(left + right - whole) <= 15.0 * eps)
        return left + right + (left + right - whole) / 15.0;

    return adaptive_simpson(a, c, eps / 2.0, left) +
           adaptive_simpson(c, b, eps / 2.0, right);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv); // Initialize the MPI environment

    int rank, size;
#if FUNC_ID == 1
    double a = 0.0, b = 1000.0;
#elif FUNC_ID == 2
    double a = 0.0, b = 5000.0;
#elif FUNC_ID == 3
    double a = 0.0, b = 200.0;
#endif
    double eps = 1e-6; // Desired accuracy

    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get current process rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

    // Calculate local subinterval for each process
    double local_a = a + rank * (b - a) / size;
    double local_b = a + (rank + 1) * (b - a) / size;

    // Start timing (per process)
    double start = MPI_Wtime();

    // Compute local integral
    double whole = simpson(local_a, local_b);
    double local_result = adaptive_simpson(local_a, local_b, eps / size, whole);

    // Reduce results to the root process (rank 0)
    double result = 0.0;
    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // End timing (only root process reports)
    double end = MPI_Wtime();

    if (rank == 0)
    {
        printf("\nIntegral = %.12f\n", result);
        printf("Execution Time: %.6f seconds\n", end - start);
        printf("Number of MPI Processes Used: %d\n\n", size);
    }

    MPI_Finalize(); // Finalize the MPI environment
    return 0;
}

/*
 * mpicc adaptive_simpson_mpi.c -o mpi -lm
 * mpirun -np 4 ./mpi
 */