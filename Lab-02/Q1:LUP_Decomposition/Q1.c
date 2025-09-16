#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-9   // tolerance for detecting singular matrices

// Function to perform LUP decomposition
int lupDecomposition(double **A, int *P, int n) {
    for (int i = 0; i < n; i++)
        P[i] = i;   // initialize permutation to identity

    for (int k = 0; k < n; k++) {
        // Find pivot (largest element in column k)
        double p = 0.0;
        int k_prime = -1;
        for (int i = k; i < n; i++) {
            if (fabs(A[i][k]) > p) {
                p = fabs(A[i][k]);
                k_prime = i;
            }
        }

        if (p < EPSILON) {
            printf("Matrix is singular (cannot decompose).\n");
            return 0; // failure
        }

        // Swap P[k] and P[k']
        int temp = P[k];
        P[k] = P[k_prime];
        P[k_prime] = temp;

        // Swap rows A[k] and A[k']
        double *row_temp = A[k];
        A[k] = A[k_prime];
        A[k_prime] = row_temp;

        // Elimination step
        for (int i = k + 1; i < n; i++) {
            A[i][k] /= A[k][k];
            for (int j = k + 1; j < n; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
    }
    return 1; // success
}

// Print matrix
void printMatrix(double **M, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%8.4f ", M[i][j]);
        printf("\n");
    }
}

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

int main() {
    int n;
    printf("Enter size of square matrix (n): ");
    scanf("%d", &n);

    double **A = allocateMatrix(n);

    printf("Enter elements of matrix A (%d x %d), row by row:\n", n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%lf", &A[i][j]);
        }
    }

    int *P = (int*)malloc(n * sizeof(int));

    if (!lupDecomposition(A, P, n)) {
        freeMatrix(A, n);
        free(P);
        return 0;
    }

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

    printf("\nPermutation vector P:\n");
    for (int i = 0; i < n; i++)
        printf("%d ", P[i]);
    printf("\n");

    printf("\nLower triangular matrix L:\n");
    printMatrix(L, n);

    printf("\nUpper triangular matrix U:\n");
    printMatrix(U, n);

    // Free memory
    freeMatrix(A, n);
    freeMatrix(L, n);
    freeMatrix(U, n);
    free(P);

    return 0;
}
