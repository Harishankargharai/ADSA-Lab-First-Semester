#include <stdio.h>
#include <stdlib.h>

// ---------- Utility functions ----------

// Allocate n×n matrix
int** allocateMatrix(int n) {
    int** mat = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++)
        mat[i] = (int*)calloc(n, sizeof(int));  // calloc → initialized to 0
    return mat;
}

// Free matrix
void freeMatrix(int** mat, int n) {
    for (int i = 0; i < n; i++)
        free(mat[i]);
    free(mat);
}

// Copy matrix block (src → dest)
void copyBlock(int** src, int** dest, int r, int c, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            dest[i][j] = src[i + r][j + c];
}

// Paste block (src → dest)
void pasteBlock(int** src, int** dest, int r, int c, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            dest[i + r][j + c] = src[i][j];
}

// Add matrices
void addMatrix(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] + B[i][j];
}

// Subtract matrices
void subMatrix(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            C[i][j] = A[i][j] - B[i][j];
}

// ---------- Strassen recursive ----------
void strassen(int** A, int** B, int** C, int n) {
    if (n == 1) {  // base case
        C[0][0] = A[0][0] * B[0][0];
        return;
    }

    int k = n / 2;

    // Submatrices
    int** A11 = allocateMatrix(k); int** A12 = allocateMatrix(k);
    int** A21 = allocateMatrix(k); int** A22 = allocateMatrix(k);
    int** B11 = allocateMatrix(k); int** B12 = allocateMatrix(k);
    int** B21 = allocateMatrix(k); int** B22 = allocateMatrix(k);

    // Split into quadrants
    copyBlock(A, A11, 0,   0,   k);
    copyBlock(A, A12, 0,   k,   k);
    copyBlock(A, A21, k,   0,   k);
    copyBlock(A, A22, k,   k,   k);

    copyBlock(B, B11, 0,   0,   k);
    copyBlock(B, B12, 0,   k,   k);
    copyBlock(B, B21, k,   0,   k);
    copyBlock(B, B22, k,   k,   k);

    // Allocate temp matrices
    int** M1 = allocateMatrix(k); int** M2 = allocateMatrix(k); int** M3 = allocateMatrix(k);
    int** M4 = allocateMatrix(k); int** M5 = allocateMatrix(k); int** M6 = allocateMatrix(k);
    int** M7 = allocateMatrix(k);
    int** T1 = allocateMatrix(k); int** T2 = allocateMatrix(k);

    // M1 = (A11 + A22)(B11 + B22)
    addMatrix(A11, A22, T1, k);
    addMatrix(B11, B22, T2, k);
    strassen(T1, T2, M1, k);

    // M2 = (A21 + A22)B11
    addMatrix(A21, A22, T1, k);
    strassen(T1, B11, M2, k);

    // M3 = A11(B12 - B22)
    subMatrix(B12, B22, T1, k);
    strassen(A11, T1, M3, k);

    // M4 = A22(B21 - B11)
    subMatrix(B21, B11, T1, k);
    strassen(A22, T1, M4, k);

    // M5 = (A11 + A12)B22
    addMatrix(A11, A12, T1, k);
    strassen(T1, B22, M5, k);

    // M6 = (A21 - A11)(B11 + B12)
    subMatrix(A21, A11, T1, k);
    addMatrix(B11, B12, T2, k);
    strassen(T1, T2, M6, k);

    // M7 = (A12 - A22)(B21 + B22)
    subMatrix(A12, A22, T1, k);
    addMatrix(B21, B22, T2, k);
    strassen(T1, T2, M7, k);

    // Compute C quadrants
    int** C11 = allocateMatrix(k); int** C12 = allocateMatrix(k);
    int** C21 = allocateMatrix(k); int** C22 = allocateMatrix(k);

    addMatrix(M1, M4, T1, k);
    subMatrix(T1, M5, T2, k);
    addMatrix(T2, M7, C11, k);       // C11 = M1 + M4 - M5 + M7

    addMatrix(M3, M5, C12, k);       // C12 = M3 + M5
    addMatrix(M2, M4, C21, k);       // C21 = M2 + M4

    subMatrix(M1, M2, T1, k);
    addMatrix(T1, M3, T2, k);
    addMatrix(T2, M6, C22, k);       // C22 = M1 - M2 + M3 + M6

    // Combine into result C
    pasteBlock(C11, C, 0,   0,   k);
    pasteBlock(C12, C, 0,   k,   k);
    pasteBlock(C21, C, k,   0,   k);
    pasteBlock(C22, C, k,   k,   k);

    // Free memory
    freeMatrix(A11,k); freeMatrix(A12,k); freeMatrix(A21,k); freeMatrix(A22,k);
    freeMatrix(B11,k); freeMatrix(B12,k); freeMatrix(B21,k); freeMatrix(B22,k);
    freeMatrix(M1,k);  freeMatrix(M2,k);  freeMatrix(M3,k);  freeMatrix(M4,k);
    freeMatrix(M5,k);  freeMatrix(M6,k);  freeMatrix(M7,k);
    freeMatrix(C11,k); freeMatrix(C12,k); freeMatrix(C21,k); freeMatrix(C22,k);
    freeMatrix(T1,k);  freeMatrix(T2,k);
}

// ---------- Helper: next power of 2 ----------
int nextPowerOf2(int n) {
    int p = 1;
    while (p < n) p *= 2;
    return p;
}

// ---------- Main ----------
int main() {
    int n;
    printf("Enter size of matrix (n x n): ");
    scanf("%d", &n);

    int** A = allocateMatrix(n);
    int** B = allocateMatrix(n);

    printf("\nEnter elements of matrix A (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("A[%d][%d] = ", i, j);
            scanf("%d", &A[i][j]);
        }
    }

    printf("\nEnter elements of matrix B (%d x %d):\n", n, n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("B[%d][%d] = ", i, j);
            scanf("%d", &B[i][j]);
        }
    }

    // Pad matrices to next power of 2
    int m = nextPowerOf2(n);
    int** A_pad = allocateMatrix(m);
    int** B_pad = allocateMatrix(m);
    int** C_pad = allocateMatrix(m);

    // Copy into padded matrices
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            A_pad[i][j] = A[i][j];
            B_pad[i][j] = B[i][j];
        }

    // Multiply
    strassen(A_pad, B_pad, C_pad, m);

    // Print result (only top-left n×n)
    printf("\nResultant matrix C (A x B):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%5d ", C_pad[i][j]);
        printf("\n");
    }

    // Free
    freeMatrix(A, n); freeMatrix(B, n);
    freeMatrix(A_pad, m); freeMatrix(B_pad, m); freeMatrix(C_pad, m);

    return 0;
}
