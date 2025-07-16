#include <stdio.h>
#include <math.h>
#include <omp.h>

#define FUNC_ID 2
#define MIN_INTERVAL_SIZE 1e-3 // Prevent too many tasks

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

// Simpson's rule
double simpson(double a, double b)
{
    double c = (a + b) / 2.0;
    return (b - a) / 6.0 * (f(a) + 4.0 * f(c) + f(b));
}

// Adaptive Simpson with optional tasks
double adaptive_simpson(double a, double b, double eps, double whole)
{
    double c = (a + b) / 2.0;
    double left = simpson(a, c);
    double right = simpson(c, b);

    if (fabs(left + right - whole) <= 15.0 * eps)
        return left + right + (left + right - whole) / 15.0;

    double res_left = 0.0, res_right = 0.0;

    if (b - a > MIN_INTERVAL_SIZE)
    {
#pragma omp task shared(res_left)
        res_left = adaptive_simpson(a, c, eps / 2.0, left);

#pragma omp task shared(res_right)
        res_right = adaptive_simpson(c, b, eps / 2.0, right);

#pragma omp taskwait
        return res_left + res_right;
    }
    else
    {
        // No more tasks, recurse sequentially
        return adaptive_simpson(a, c, eps / 2.0, left) +
               adaptive_simpson(c, b, eps / 2.0, right);
    }
}

int main()
{
#if FUNC_ID == 1
    double a = 0.0, b = 1000.0;
#elif FUNC_ID == 2
    double a = 0.0, b = 5000.0;
#elif FUNC_ID == 3
    double a = 0.0, b = 200.0;
#endif

    double eps = 1e-6;
    double result = 0.0;

    omp_set_num_threads(6);
    double start = omp_get_wtime();

#pragma omp parallel
    {
#pragma omp single
        {
            double whole = simpson(a, b);
            result = adaptive_simpson(a, b, eps, whole);
        }
    }

    double end = omp_get_wtime();

    printf("\nIntegral = %.12f\n", result);
    printf("Execution Time: %.6f seconds\n", end - start);
    printf("Number of threads used : %d \n\n", omp_get_max_threads());
    return 0;
}
