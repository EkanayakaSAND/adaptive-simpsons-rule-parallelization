#include <stdio.h>
#include <math.h>
#include <time.h>

#define FUNC_ID 2

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

// Simple Simpson's Rule on interval [a, b]
double simpson(double a, double b)
{
    double c = (a + b) / 2.0;
    return (b - a) / 6.0 * (f(a) + 4.0 * f(c) + f(b));
}

// Recursive Adaptive Simpson's Rule
double adaptive_simpson(double a, double b, double eps, double whole)
{
    double c = (a + b) / 2.0;
    double left = simpson(a, c);
    double right = simpson(c, b);

    // Check if the approximation meets the accuracy requirement
    if (fabs(left + right - whole) <= 15.0 * eps)
        return left + right + (left + right - whole) / 15.0;

    // Recursively apply to left and right halves
    return adaptive_simpson(a, c, eps / 2.0, left) +
           adaptive_simpson(c, b, eps / 2.0, right);
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
    double eps = 1e-6; // Desired precision

    // Start timing
    clock_t start = clock();

    // Calculate the integral
    double result = adaptive_simpson(a, b, eps, simpson(a, b));

    // End timing
    clock_t end = clock();

    // Compute execution time in seconds
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Display the result and timing
    printf("\nIntegral = %.12f\n", result);
    printf("\nExecution Time = %.6f seconds\n\n", time_taken);

    return 0;
}

/*
 * gcc -o serial adaptive_simpson_serial.c -lm
 * ./serial
 */