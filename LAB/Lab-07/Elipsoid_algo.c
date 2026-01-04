
#include <stdio.h>
#include <math.h>

#define MAXN 20
#define MAXM 200
#define MAX_ITER 1000
#define EPS 1e-6

// Matrix-vector multiplication: y = A*x
void matvec(double y[], double A[][MAXN], double x[], int m, int n) {
    for (int i = 0; i < m; i++) {
        y[i] = 0;
        for (int j = 0; j < n; j++)
            y[i] += A[i][j] * x[j];
    }
}

// Matrix-vector product: y = M*x
void matvec2(double y[], double M[][MAXN], double x[], int n) {
    for (int i = 0; i < n; i++) {
        y[i] = 0;
        for (int j = 0; j < n; j++)
            y[i] += M[i][j] * x[j];
    }
}

// Outer product: C = a*b^T
void outer(double C[][MAXN], double a[], double b[], int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = a[i] * b[j];
}

// Print vector
void print_vec(double x[], int n) {
    printf("[");
    for (int i = 0; i < n; i++)
        printf(" %.5f ", x[i]);
    printf("]\n");
}

int main() {
    int n, m;

    double A[MAXM][MAXN], b[MAXM];
    double x[MAXN];              // ellipsoid center
    double P[MAXN][MAXN];        // shape matrix

    printf("Enter number of variables (n): ");
    scanf("%d", &n);

    printf("Enter number of constraints (m): ");
    scanf("%d", &m);

    printf("\nEnter matrix A (%d x %d):\n", m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%lf", &A[i][j]);

    printf("\nEnter vector b (%d elements):\n", m);
    for (int i = 0; i < m; i++)
        scanf("%lf", &b[i]);

    printf("\nEnter initial point (center of ellipsoid):\n");
    for (int i = 0; i < n; i++)
        scanf("%lf", &x[i]);

    printf("\nEnter initial ellipsoid radius R: ");
    double R;
    scanf("%lf", &R);

    // Initial ellipsoid P = R^2 * I
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            P[i][j] = (i == j ? R*R : 0);

    printf("\nRunning Ellipsoid Method...\n");

    for (int iter = 1; iter <= MAX_ITER; iter++) {

        // evaluate Ax
        double Ax[MAXM];
        matvec(Ax, A, x, m, n);

        // find violated constraint
        int violated = -1;
        for (int i = 0; i < m; i++) {
            if (Ax[i] > b[i] + EPS) {
                violated = i;
                break;
            }
        }

        // If no violation → feasible point found
        if (violated == -1) {
            printf("\nFeasible solution found at iteration %d:\n", iter);
            print_vec(x, n);
            return 0;
        }

        // Get violated constraint normal vector
        double a[MAXN];
        for (int j = 0; j < n; j++)
            a[j] = A[violated][j];

        // Compute P*a
        double Pa[MAXN];
        matvec2(Pa, P, a, n);

        // Compute scalar a^T P a
        double aPa = 0;
        for (int i = 0; i < n; i++)
            aPa += a[i] * Pa[i];

        // Update formulas
        double gamma = 1.0 / (n + 1);
        double beta = 2.0 / (n + 1);

        // New center: x = x - gamma * (P*a) / sqrt(a^T P a)
        double scale = gamma / sqrt(aPa);
        for (int i = 0; i < n; i++)
            x[i] -= scale * Pa[i];

        // New shape matrix:
        double temp[MAXN][MAXN], outerPa[MAXN][MAXN];
        outer(outerPa, Pa, Pa, n);

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                temp[i][j] = P[i][j] - beta * (outerPa[i][j] / aPa);

        double factor = ((double)n * n) / (n*n - 1);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                P[i][j] = factor * temp[i][j];

        if (iter % 50 == 0) {
            printf("Iter %d: center = ", iter);
            print_vec(x, n);
        }
    }

    printf("\nFailed: No feasible point found after %d iterations.\n", MAX_ITER);
    return 0;
}
