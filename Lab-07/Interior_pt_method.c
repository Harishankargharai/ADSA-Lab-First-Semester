#include <stdio.h>
#include <math.h>

#define MAXN 20             /* max variables */
#define MAXM 20             /* max equality constraints */
#define MAXKKT (MAXN+MAXM)
#define EPS 1e-12

/* Solve linear system K x = rhs, K is N x N, using Gaussian elimination with partial pivoting */
int solve_linear_system(int N, double K[MAXKKT][MAXKKT], double rhs[MAXKKT], double x[MAXKKT])
{
    int i, j, k, pivot;
    double maxv, tmp, factor;

    /* Forward elimination */
    for (k = 0; k < N; k++) {
        /* Find pivot row */
        pivot = k;
        maxv = fabs(K[k][k]);
        for (i = k + 1; i < N; i++) {
            if (fabs(K[i][k]) > maxv) {
                maxv = fabs(K[i][k]);
                pivot = i;
            }
        }
        if (maxv < EPS) {
            /* Singular matrix */
            return -1;
        }

        /* Swap rows k and pivot in K and rhs */
        if (pivot != k) {
            for (j = 0; j < N; j++) {
                tmp = K[k][j];
                K[k][j] = K[pivot][j];
                K[pivot][j] = tmp;
            }
            tmp = rhs[k];
            rhs[k] = rhs[pivot];
            rhs[pivot] = tmp;
        }

        /* Eliminate entries below pivot */
        for (i = k + 1; i < N; i++) {
            factor = K[i][k] / K[k][k];
            rhs[i] -= factor * rhs[k];
            for (j = k; j < N; j++) {
                K[i][j] -= factor * K[k][j];
            }
        }
    }

    /* Back substitution */
    for (i = N - 1; i >= 0; i--) {
        tmp = rhs[i];
        for (j = i + 1; j < N; j++) {
            tmp -= K[i][j] * x[j];
        }
        x[i] = tmp / K[i][i];
    }

    return 0;
}

int main(void)
{
    int m, n;
    double A[MAXM][MAXN];
    double b[MAXM];
    double c[MAXN];
    double x[MAXN];      /* current primal */
    double lambda[MAXM]; /* dual variables (for equality constraints) */

    int i, j, k;
    int N;               /* size of KKT system = n + m */
    double t, mu;
    double grad[MAXN];
    double rp[MAXM];
    double KKT[MAXKKT][MAXKKT];
    double rhs[MAXKKT];
    double sol[MAXKKT];
    double norm_r, alpha, alpha_max;
    int outer_iter, inner_iter;

    printf("Interior Point Method for LP: min c^T x  s.t. A x = b, x >= 0\n");
    printf("Enter number of constraints m (<=%d) and variables n (<=%d): ", MAXM, MAXN);
    if (scanf("%d %d", &m, &n) != 2) {
        printf("Input error.\n");
        return 1;
    }
    if (m > MAXM || n > MAXN) {
        printf("m or n too large.\n");
        return 1;
    }

    printf("Enter matrix A (m x n), row by row:\n");
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            if (scanf("%lf", &A[i][j]) != 1) {
                printf("Input error.\n");
                return 1;
            }
        }
    }

    printf("Enter vector b (size m):\n");
    for (i = 0; i < m; i++) {
        if (scanf("%lf", &b[i]) != 1) {
            printf("Input error.\n");
            return 1;
        }
    }

    printf("Enter cost vector c (size n):\n");
    for (j = 0; j < n; j++) {
        if (scanf("%lf", &c[j]) != 1) {
            printf("Input error.\n");
            return 1;
        }
    }

    printf("Enter strictly feasible starting point x0 (size n), with A x0 = b and x0 > 0:\n");
    for (j = 0; j < n; j++) {
        if (scanf("%lf", &x[j]) != 1) {
            printf("Input error.\n");
            return 1;
        }
        if (x[j] <= 0.0) {
            printf("x0 must be strictly positive.\n");
            return 1;
        }
    }

    /* Initialize lambda = 0 */
    for (i = 0; i < m; i++) lambda[i] = 0.0;

    N = n + m;

    /* Barrier parameters */
    t = 1.0;      /* initial barrier parameter */
    mu = 10.0;    /* factor to increase t */

    for (outer_iter = 0; outer_iter < 30; outer_iter++) {
        /* Inner Newton iterations for current t */
        for (inner_iter = 0; inner_iter < 50; inner_iter++) {

            /* Gradient of barrier-augmented objective: t*c - 1./x */
            for (j = 0; j < n; j++) {
                grad[j] = t * c[j] - 1.0 / x[j];
            }

            /* Primal residual: A x - b */
            for (i = 0; i < m; i++) {
                rp[i] = -b[i];
                for (j = 0; j < n; j++) {
                    rp[i] += A[i][j] * x[j];
                }
            }

            /* Compute norm of residuals (simple Euclidean) */
            norm_r = 0.0;
            for (j = 0; j < n; j++) norm_r += grad[j] * grad[j];
            for (i = 0; i < m; i++) norm_r += rp[i] * rp[i];
            norm_r = sqrt(norm_r);

            if (norm_r < 1e-8) {
                /* Centered enough for this t */
                break;
            }

            /* Build KKT matrix:
               [ H   A^T ]
               [ A    0  ]

               where H = diag(1/x_j^2)
            */
            for (i = 0; i < N; i++) {
                for (j = 0; j < N; j++) {
                    KKT[i][j] = 0.0;
                }
            }

            /* H block */
            for (j = 0; j < n; j++) {
                KKT[j][j] = 1.0 / (x[j] * x[j]); /* Hessian of -sum log(x_j) */
            }

            /* A^T and A blocks */
            for (i = 0; i < m; i++) {
                for (j = 0; j < n; j++) {
                    KKT[j][n + i] = A[i][j];      /* A^T block */
                    KKT[n + i][j] = A[i][j];      /* A block   */
                }
            }

            /* RHS = - [grad; rp] */
            for (j = 0; j < n; j++) {
                rhs[j] = -grad[j];
            }
            for (i = 0; i < m; i++) {
                rhs[n + i] = -rp[i];
            }

            /* Solve KKT * [dx; dlambda] = rhs */
            if (solve_linear_system(N, KKT, rhs, sol) != 0) {
                printf("Linear system solve failed (singular KKT).\n");
                return 1;
            }

            /* Extract dx and dlambda */
            /* dx = sol[0..n-1], dlambda = sol[n..n+m-1] */
            /* Step size to maintain positivity of x */
            alpha_max = 1.0;
            for (j = 0; j < n; j++) {
                double dxj = sol[j];
                if (dxj < 0.0) {
                    double candidate = -0.99 * x[j] / dxj;
                    if (candidate < alpha_max) alpha_max = candidate;
                }
            }
            if (alpha_max > 1.0) alpha_max = 1.0;
            if (alpha_max <= 0.0) alpha_max = 1e-3; /* very small step if needed */

            alpha = alpha_max*0.5;

            /* Update x and lambda */
            for (j = 0; j < n; j++) {
                x[j] += alpha * sol[j];
                if (x[j] <= 0) x[j] = 1e-8; /* safety */
            }
            for (i = 0; i < m; i++) {
                lambda[i] += alpha * sol[n + i];
            }
        }

        /* Check approximate duality gap: n / t */
        if (norm_r < 1e-8 && (double)n / t < 1e-6) {
            break;  // Both gradient and duality gap must be small.
        }


        t *= mu; /* increase barrier parameter */
    }

    /* Compute objective value c^T x */
    double obj = 0.0;
    for (j = 0; j < n; j++) obj += c[j] * x[j];

    printf("\nApproximate optimal solution:\n");
    for (j = 0; j < n; j++) {
        printf("x[%d] = %.30f\n", j, x[j]);
    }
    printf("Objective value c^T x = %.20f\n", obj);

    return 0;
}
