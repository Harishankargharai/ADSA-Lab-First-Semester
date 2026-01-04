#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-9   // tolerance for pivoting

// Allocate n×n matrix
double** allocateMatrix(int n) {
    double** mat = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++)
        mat[i] = (double*)calloc(n, sizeof(double));
    return mat;
}

// Free matrix
void freeMatrix(double** mat, int n) {
    for (int i = 0; i < n; i++)
        free(mat[i]);
    free(mat);
}

// ---------- LUP Decomposition ----------
int lupDecomposition(double **A, int *P, int n) {
    for (int i = 0; i < n; i++)
        P[i] = i;   // identity permutation

    for (int k = 0; k < n; k++) {
        double p = 0.0;
        int k_prime = -1;
        for (int i = k; i < n; i++) {
            if (fabs(A[i][k]) > p) {
                p = fabs(A[i][k]);
                k_prime = i;
            }
        }
        if (p < EPSILON) {
            printf("Matrix is singular or near-singular.\n");
            return 0; // failure
        }

        // Swap permutation
        int temp = P[k];
        P[k] = P[k_prime];
        P[k_prime] = temp;

        // Swap rows in A
        double *row_temp = A[k];
        A[k] = A[k_prime];
        A[k_prime] = row_temp;

        // Elimination
        for (int i = k + 1; i < n; i++) {
            A[i][k] /= A[k][k];
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
    }
    return 1; // success
}

// ---------- Forward substitution (Ly = Pb) ----------
void forwardSubstitution(double **L, double *b, double *y, int *P, int n) {
    for (int i = 0; i < n; i++) {
        y[i] = b[P[i]];
        for (int j = 0; j < i; j++) {
            y[i] -= L[i][j] * y[j];
        }
    }
}

// ---------- Backward substitution (Ux = y) ----------
void backwardSubstitution(double **U, double *y, double *x, int n) {
    for (int i = n - 1; i >= 0; i--) {
        x[i] = y[i];
        for (int j = i + 1; j < n; j++) {
            x[i] -= U[i][j] * x[j];
        }
        x[i] /= U[i][i];
    }
}

// ---------- Solve Ax = b ----------
int lupSolve(double **A, int *P, double *b, double *x, int n) {
    // Extract L and U from A
    double **L = allocateMatrix(n);
    double **U = allocateMatrix(n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i > j)
                L[i][j] = A[i][j];
            else if (i == j)
                L[i][j] = 1.0;
            else
                L[i][j] = 0.0;

            if (i <= j)
                U[i][j] = A[i][j];
            else
                U[i][j] = 0.0;
        }
    }

    double *y = (double*)malloc(n * sizeof(double));
    forwardSubstitution(L, b, y, P, n);
    backwardSubstitution(U, y, x, n);

    freeMatrix(L, n);
    freeMatrix(U, n);
    free(y);

    return 1;
}

// ---------- Main ----------
int main() {
    int n;
    printf("Enter size of matrix (n): ");
    scanf("%d", &n);

    double **A = allocateMatrix(n);
    double *b = (double*)malloc(n * sizeof(double));
    double *x = (double*)malloc(n * sizeof(double));
    int *P = (int*)malloc(n * sizeof(int));

    printf("Enter matrix A (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &A[i][j]);
        }
    }

    printf("Enter vector b (%d elements):\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%lf", &b[i]);
    }

    if (!lupDecomposition(A, P, n)) {
        freeMatrix(A, n);
        free(b);
        free(x);
        free(P);
        return 0;
    }

    lupSolve(A, P, b, x, n);

    printf("\nSolution vector x:\n");
    for (int i = 0; i < n; i++) {
        printf("x[%d] = %.6f\n", i, x[i]);
    }

    freeMatrix(A, n);
    free(b);
    free(x);
    free(P);

    return 0;
}
