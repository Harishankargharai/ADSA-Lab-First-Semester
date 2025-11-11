/*
  simplex_two_phase.c
  Advanced Simplex implementation with two-phase method.
  Supports <=, >=, = constraints, maximization/minimization.
  Compile: gcc -O2 -lm simplex_two_phase.c -o simplex
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define EPS 1e-9
#define MAXV 200   /* max variables (original) */
#define MAXC 200   /* max constraints */
#define MAXCOL 1000
#define MAXROW 1000

typedef enum {LEQ, GEQ, EQ} relop_t;
typedef enum {MAXIMIZE, MINIMIZE} obj_t;

/* Tableau structure:
   tableau is (rows = m + 1) x (cols = total_vars + 1 for RHS)
   Row 0 is objective row, rows 1..m are constraints.
*/
typedef struct {
    int rows, cols; /* rows = m + 1, cols = total_vars + 1 (last col RHS) */
    double a[MAXROW][MAXCOL];
    int basic[MAXROW];    /* basic var index for each row (1..m) */
    int nonbasic[MAXCOL]; /* variable index mapping for columns (excluding RHS) */
} tableau_t;

/* Global problem description */
int n_vars, n_cons;
obj_t objective_type;
double c[MAXV+1]; /* 1..n original objective coefficients */
double A[MAXC+1][MAXV+1];
double b_rhs[MAXC+1];
relop_t rels[MAXC+1];

/* Utility prints */
void print_row(double *row, int cols) {
    for (int j=0;j<cols;j++){
        printf("%10.6f ", row[j]);
    }
    printf("\n");
}
void print_tableau(tableau_t *T) {
    printf("Tableau (rows=%d cols=%d):\n", T->rows, T->cols);
    for (int i=0;i<T->rows;i++){
        print_row(T->a[i], T->cols);
    }
    printf("Basic vars per row: ");
    for(int i=1;i<T->rows;i++) printf("%d ", T->basic[i]);
    printf("\n");
}

/* Build initial two-phase tableau */
tableau_t *build_two_phase() {
    /* Count slack, surplus, artificial */
    int slack=0, surplus=0, art=0;
    for (int i=1;i<=n_cons;i++){
        if (rels[i]==LEQ) slack++;
        else if (rels[i]==GEQ) { surplus++; art++; }
        else if (rels[i]==EQ) { art++; }
    }
    int total = n_vars + slack + surplus + art;
    int cols = total + 1; /* +1 for RHS */
    int rows = n_cons + 1; /* +1 for objective row */

    tableau_t *T = malloc(sizeof(tableau_t));
    T->rows = rows; T->cols = cols;
    /* zero */
    for (int i=0;i<rows;i++) for (int j=0;j<cols;j++) T->a[i][j]=0.0;
    for (int i=0;i<rows;i++) T->basic[i]= -1;
    for (int j=0;j<cols;j++) T->nonbasic[j]= -1;

    /* Column indexing: 1..total are variables; last column index = cols-1 is RHS.
       We'll map:
         original vars: 1..n_vars
         slack vars:   n_vars+1 .. n_vars+slack
         surplus vars: next...
         artificial vars: last group
    */
    int col = 1;
    for (int j=1;j<=n_vars;j++) T->nonbasic[col++] = j; /* original var index = j */
    int slack_idx = col;
    for (int i=1;i<=n_cons;i++){
        if (rels[i]==LEQ){ T->nonbasic[col++] = n_vars + (col - slack_idx + 1); }
    }
    int surplus_idx = col;
    for (int i=1;i<=n_cons;i++){
        if (rels[i]==GEQ){ T->nonbasic[col++] = n_vars + (col - slack_idx + 1); }
    }
    int art_idx = col;
    for (int i=1;i<=n_cons;i++){
        if (rels[i]==GEQ || rels[i]==EQ){ T->nonbasic[col++] = 1000 + i; /* artificial id = 1000 + i */ }
    }
    /* Now fill constraint rows (1..m) */
    int s_col = slack_idx;
    int su_col = surplus_idx;
    int a_col = art_idx;
    for (int i=1;i<=n_cons;i++){
        /* Original variables */
        for (int j=1;j<=n_vars;j++){
            T->a[i][j] = A[i][j];
        }
        /* Slack / surplus / artificials */
        if (rels[i]==LEQ){
            T->a[i][s_col++] = 1.0;
            /* basic var is this slack (column index-1) */
        } else if (rels[i]==GEQ){
            T->a[i][su_col++] = -1.0;
            T->a[i][a_col] = 1.0;
            a_col++;
        } else { /* EQ */
            T->a[i][a_col] = 1.0;
            a_col++;
        }
        T->a[i][cols-1] = b_rhs[i];
    }
    /* Set basic variable indexes for each constraint row */
    /* We mark basis by matching columns with identity columns; we simply store nonbasic var index for each col and assign */
    for (int i=1;i<T->rows;i++){
        /* find a column with a 1 in row i and zeros in other constraint rows -> basic */
        int found = -1;
        for (int j=1;j<cols-1;j++){
            if (fabs(T->a[i][j]-1.0) < EPS){
                int otherZeros = 1;
                for (int r=1;r<T->rows;r++){
                    if (r==i) continue;
                    if (fabs(T->a[r][j]) > EPS) { otherZeros = 0; break; }
                }
                if (otherZeros) { found = j; break; }
            }
        }
        if (found!=-1) T->basic[i] = T->nonbasic[found]; else T->basic[i] = -1;
    }

    /* Build Phase I objective: minimize sum of artificials -> in tableau we put objective row as sum of artificial rows (we'll perform row operations) */
    /* Initialize objective row to zero */
    for (int j=0;j<cols;j++) T->a[0][j]=0.0;

    /* For each artificial variable column, add its constraint row to objective row (because objective = sum of artificials) */
    for (int j=1;j<cols-1;j++){
        int varid = T->nonbasic[j];
        if (varid>=1000){ /* artificial variable corresponding to constraint i where varid = 1000 + i */
            /* find the constraint row that contains that artificial as basic (has 1) */
            for (int i=1;i<T->rows;i++){
                if (fabs(T->a[i][j] - 1.0) < EPS){
                    /* objective row = objective row + row_i */
                    for (int k=0;k<cols;k++) T->a[0][k] += T->a[i][k];
                    break;
                }
            }
        }
    }
    /* Phase I objective currently is sum of artificials (we will minimize). For simplex implementation we keep objective row as (z - sum) = 0; to use simplex pivot rules we will use T->a[0] as the "reduced cost" row directly. */
    return T;
}

/* Find entering column (most negative coeff in objective row for minimization; for phase I we minimize sum of artificials)
   We implement Bland's rule fallback: choose smallest index with negative reduced cost
*/
int find_entering(tableau_t *T, int phase_is_min) {
    int best = -1;
    double bestVal = 0.0;
    for (int j=1;j<T->cols-1;j++){
        double val = T->a[0][j];
        if (phase_is_min){
            if (val < -EPS){
                if (best==-1 || val < bestVal - EPS || (fabs(val-bestVal) < EPS && T->nonbasic[j] < T->nonbasic[best])){
                    best = j; bestVal = val;
                }
            }
        } else { /* for maximization in phase II we look for positive reduced cost */
            if (val > EPS){
                if (best==-1 || val > bestVal + EPS || (fabs(val-bestVal) < EPS && T->nonbasic[j] < T->nonbasic[best])){
                    best = j; bestVal = val;
                }
            }
        }
    }
    return best;
}

/* Find leaving row using minimum ratio test */
int find_leaving(tableau_t *T, int entering_col) {
    int leaving = -1;
    double minratio = 0.0;
    for (int i=1;i<T->rows;i++){
        double aij = T->a[i][entering_col];
        double rhs = T->a[i][T->cols-1];
        if (aij > EPS){
            double ratio = rhs / aij;
            if (leaving == -1 || ratio < minratio - EPS || (fabs(ratio - minratio) < EPS && T->basic[i] < T->basic[leaving])){
                leaving = i; minratio = ratio;
            }
        }
    }
    return leaving;
}

/* Pivot on (row r, col s) */
void pivot(tableau_t *T, int r, int s) {
    double pivot = T->a[r][s];
    /* scale pivot row to make pivot = 1 */
    for (int j=0;j<T->cols;j++) T->a[r][j] /= pivot;
    /* eliminate other rows */
    for (int i=0;i<T->rows;i++){
        if (i==r) continue;
        double factor = T->a[i][s];
        if (fabs(factor) > EPS){
            for (int j=0;j<T->cols;j++){
                T->a[i][j] -= factor * T->a[r][j];
            }
        }
    }
    /* swap basic var */
    int entering_var = T->nonbasic[s];
    T->basic[r] = entering_var;
}

/* Run simplex on given tableau. phase_is_min indicates objective row is for minimization (true for Phase I).
   phase2_mode: if 1 means we are in Phase II where we stop when objective has no positive reduced costs (for maximizing).
*/
int run_simplex(tableau_t *T, int phase_is_min, int verbose) {
    int iter = 0;
    while (1) {
        iter++;
        if (iter > 5000) { /* safe-guard */
            if (verbose) printf("Iteration limit reached\n");
            return -2; /* fail/loop */
        }
        int entering = find_entering(T, phase_is_min);
        if (entering == -1) {
            /* optimal for this objective */
            return 0;
        }
        int leaving = find_leaving(T, entering);
        if (leaving == -1) {
            /* unbounded */
            return 1;
        }
        pivot(T, leaving, entering);
        if (verbose){
            printf("Pivot on row %d col %d (var %d)\n", leaving, entering, T->basic[leaving]);
            print_tableau(T);
        }
    }
}

/* Remove artificial columns and rebuild Phase II tableau with original objective */
tableau_t *build_phase2(tableau_t *T_phase1) {
    /* Determine which columns are artificial: nonbasic[] with id >=1000 */
    int cols = T_phase1->cols;
    int rows = T_phase1->rows;
    int keep_col[cols];
    for (int j=0;j<cols;j++) keep_col[j] = 1;
    /* Mark artificial variable columns to remove if present */
    for (int j=1;j<cols-1;j++){
        int varid = T_phase1->nonbasic[j];
        if (varid >= 1000) keep_col[j] = 0;
    }
    /* However, if an artificial is basic, it must have been driven to zero for feasible. If any basic artificial has nonzero RHS -> infeasible should have been detected in Phase I */
    /* Count kept columns */
    int newcols = 1; /* RHS included later */
    for (int j=1;j<cols-1;j++) if (keep_col[j]) newcols++;
    newcols++; /* RHS */
    /* Build new tableau */
    tableau_t *T = malloc(sizeof(tableau_t));
    T->rows = rows;
    T->cols = newcols;
    for (int i=0;i<T->rows;i++) for (int j=0;j<T->cols;j++) T->a[i][j]=0.0;
    for (int i=0;i<T->rows;i++) T->basic[i] = -1;
    for (int j=0;j<T->cols;j++) T->nonbasic[j] = -1;

    /* fill nonbasic mapping */
    int nc = 1;
    for (int j=1;j<cols-1;j++){
        if (keep_col[j]) {
            T->nonbasic[nc++] = T_phase1->nonbasic[j];
        }
    }
    T->nonbasic[nc] = -1; /* RHS placeholder */

    /* Copy rows */
    for (int i=0;i<T->rows;i++){
        nc = 1;
        for (int j=1;j<cols-1;j++){
            if (keep_col[j]) {
                T->a[i][nc++] = T_phase1->a[i][j];
            }
        }
        T->a[i][T->cols-1] = T_phase1->a[i][cols-1];
    }
    /* Copy basic var mapping but if a basic var was artificial we should have replaced it earlier; prefer to leave basic var id as is if its col kept */
    for (int i=1;i<T->rows;i++){
        T->basic[i] = T_phase1->basic[i];
    }
    return T;
}

/* Set Phase II objective row from original c[] and run */
void set_phase2_objective(tableau_t *T, double orig_c[], int maximize) {
    /* Set objective row as reduced costs: z - c^T x = 0 form. We'll set T->a[0][j] = -c_j initially (so that entering rule picks >0 for maximization) then do row operations to account for current basis */
    for (int j=0;j<T->cols;j++) T->a[0][j]=0.0;
    /* Build mapping from var id to column index in T */
    int col_of_var[2000]; for(int i=0;i<2000;i++) col_of_var[i] = -1;
    for (int j=1;j<T->cols-1;j++){
        int vid = T->nonbasic[j];
        if (vid >= 1 && vid <= n_vars) col_of_var[vid] = j;
    }
    /* If original problem is maximization, standard simplex uses objective row as (z - c) so entering criteria is positive reduced cost. We will put a sign accordingly */
    for (int vid=1; vid<=n_vars; vid++){
        int j = col_of_var[vid];
        if (j!=-1) {
            /* Put -c for z - c x = 0. For minimization we can convert by negating objective coefficients or flip rule later */
            if (maximize) T->a[0][j] = -orig_c[vid];
            else T->a[0][j] = orig_c[vid]; /* for minimization we will treat objective row as minimizing */
        }
    }
    /* Now adjust objective by subtracting rows for current basic variables (since z - c_B*B^{-1}*N) */
    for (int i=1;i<T->rows;i++){
        int bid = T->basic[i];
        if (bid >=1 && bid <= n_vars){
            double coef = (maximize ? orig_c[bid] : -orig_c[bid]); /* if maximize, subtract c_B * row_i */
            for (int j=0;j<T->cols;j++){
                T->a[0][j] += coef * T->a[i][j];
            }
        } else {
            /* If basic var is not original (e.g., slack), treat c_B = 0 -> no change */
        }
    }
}

/* Extract solution */
void extract_solution(tableau_t *T, double sol[]) {
    for (int j=1;j<=n_vars;j++) sol[j]=0.0;
    for (int i=1;i<T->rows;i++){
        int bid = T->basic[i];
        if (bid>=1 && bid<=n_vars){
            /* find col where that var appears */
            int col=-1;
            for (int j=1;j<T->cols-1;j++){
                if (T->nonbasic[j]==bid) { col=j; break;}
            }
            if (col!=-1){
                sol[bid] = T->a[i][T->cols-1];
            }
        }
    }
}

/* Main driver */
int main(){
    printf("Advanced Simplex (Two-Phase)\n");
    printf("Enter number of variables (n) and number of constraints (m):\n");
    if (scanf("%d %d", &n_vars, &n_cons)!=2){ printf("Invalid input\n"); return 0; }
    printf("Objective: 1 = MAXIMIZE, 2 = MINIMIZE: ");
    int ot; scanf("%d",&ot); objective_type = (ot==1?MAXIMIZE:MINIMIZE);
    printf("Enter objective coefficients (c1..cn) separated by spaces:\n");
    for (int j=1;j<=n_vars;j++) scanf("%lf",&c[j]);
    printf("Now enter %d constraints in format:\n", n_cons);
    printf("a1 a2 ... an  op  b   where op is <= , >= , or =\n");
    for (int i=1;i<=n_cons;i++){
        for (int j=1;j<=n_vars;j++) scanf("%lf",&A[i][j]);
        char op[4]; scanf("%s", op);
        if (strcmp(op,"<=")==0) rels[i]=LEQ;
        else if (strcmp(op,">=")==0) rels[i]=GEQ;
        else rels[i]=EQ;
        scanf("%lf",&b_rhs[i]);
        if (b_rhs[i] < -EPS){
            /* Multiply row by -1 and flip relation */
            for (int j=1;j<=n_vars;j++) A[i][j] = -A[i][j];
            b_rhs[i] = -b_rhs[i];
            if (rels[i]==LEQ) rels[i] = GEQ;
            else if (rels[i]==GEQ) rels[i] = LEQ;
        }
    }

    /* Build Phase I tableau */
    tableau_t *T1 = build_two_phase();
    printf("Initial Phase I tableau:\n");
    print_tableau(T1);

    /* Run Phase I (minimize sum of artificials). We consider objective row as currently sum of artificial rows, and we need to minimize it. Our run_simplex uses 'phase_is_min' = 1 */
    int res = run_simplex(T1, 1, 1);
    if (res == 1){
        printf("Phase I found unbounded (shouldn't happen). Exiting.\n"); return 0;
    } else if (res == -2){
        printf("Phase I reached iteration limit. Exiting.\n"); return 0;
    }
    /* Check Phase I objective value (RHS at a0) */
    double phase1_obj = T1->a[0][T1->cols-1];
    printf("Phase I objective value = %f\n", phase1_obj);
    if (phase1_obj > EPS){
        printf("LP is infeasible (Phase I objective > 0). No feasible solution.\n");
        return 0;
    }

    /* Remove artificials and build Phase II tableau */
    tableau_t *T2 = build_phase2(T1);
    /* Set Phase II objective using original c[] */
    set_phase2_objective(T2, c, (objective_type==MAXIMIZE)?1:0);
    printf("Phase II initial tableau:\n");
    print_tableau(T2);

    /* Run Phase II. If maximize -> phase_is_min = 0 to look for positive reduced costs; if minimize -> phase_is_min = 1 */
    res = run_simplex(T2, (objective_type==MINIMIZE)?1:0, 1);
    if (res == 1){
        printf("LP is unbounded.\n");
        return 0;
    } else if (res == -2){
        printf("Phase II iteration limit reached.\n");
        return 0;
    }
    /* Extract solution */
    double sol[MAXV+1];
    extract_solution(T2, sol);
    printf("Optimal solution found:\n");
    for (int j=1;j<=n_vars;j++){
        printf("x%d = %10.6f\n", j, sol[j]);
    }
    double objval = T2->a[0][T2->cols-1];
    if (objective_type==MAXIMIZE) objval = objval; /* z is in tableau as z - cN... = RHS */
    else objval = objval; /* For minimization the sign handling already set */
    printf("Objective value: %f\n", objval);
    return 0;
}
