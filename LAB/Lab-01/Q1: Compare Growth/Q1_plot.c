#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Functions
double fun1(double a) { return a * (log(a) / log(2)); }
double fun2(double a) { return 12 * sqrt(a); }
double fun3(double a) { return 1.0 / a; }
double fun4(double a) { return pow(a, log(a) / log(2)); }
double fun5(double a) { return 100 * pow(a,2) + 6*a; }
double fun6(double a) { return pow(a, 0.51); }
double fun7(double a) { return pow(a,2) - 324; }
double fun8(double a) { return 50 * sqrt(a); }
double fun9(double a) { return 2 * pow(a,3); }
double fun10(double a) { return pow(3,a); }
double fun11(double a) { return ((double)pow(2,20)) * a; } // scaled down for plotting
double fun12(double a) { return log(a) / log(2); }

int main() {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (gp == NULL) {
        printf("Could not open gnuplot.\n");
        return 1;
    }

    // Setup gnuplot
    fprintf(gp, "set title 'Growth Comparison of Functions'\n");
    fprintf(gp, "set xlabel 'n'\n");
    fprintf(gp, "set ylabel 'f(n)'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set logscale y\n");   // log scale for fair comparison
    fprintf(gp, "set key outside\n");

    // Tell gnuplot to expect 12 datasets
    fprintf(gp,
        "plot '-' with lines title 'n*log2(n)', "
        "'-' with lines title '12*sqrt(n)', "
        "'-' with lines title '1/n', "
        "'-' with lines title 'n^log2(n)', "
        "'-' with lines title '100*n^2+6n', "
        "'-' with lines title 'n^0.51', "
        "'-' with lines title 'n^2-324', "
        "'-' with lines title '50*sqrt(n)', "
        "'-' with lines title '2*n^3', "
        "'-' with lines title '3^n', "
        "'-' with lines title '(2^20)*n', "
        "'-' with lines title 'log2(n)'\n"
    );

    // Generate values of n automatically
    int maxn = 20;   // number of data points
    int step = 5;    // step size for n
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun1(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun2(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun3(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun4(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun5(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun6(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun7(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun8(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun9(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= 15; n++) fprintf(gp, "%d %f\n", n, fun10(n)); fprintf(gp, "e\n"); // smaller range (3^n grows fast)
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun11(n)); fprintf(gp, "e\n");
    for (int n = 1; n <= maxn*step; n += step) fprintf(gp, "%d %f\n", n, fun12(n)); fprintf(gp, "e\n");

    pclose(gp);
    return 0;
}
