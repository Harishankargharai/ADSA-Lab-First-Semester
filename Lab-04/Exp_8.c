#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 100

// -------------------- LCS Length (Dynamic Programming) --------------------
int LCSLength(char *X, char *Y, int m, int n, int dp[MAX][MAX]) {
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                dp[i][j] = 0;
            else if (X[i - 1] == Y[j - 1])
                dp[i][j] = 1 + dp[i - 1][j - 1];
            else
                dp[i][j] = (dp[i - 1][j] > dp[i][j - 1]) ? dp[i - 1][j] : dp[i][j - 1];
        }
    }
    return dp[m][n];
}

// -------------------- Print One LCS --------------------
void printOneLCS(char *X, char *Y, int m, int n, int dp[MAX][MAX]) {
    char lcs[MAX];
    int index = dp[m][n];
    lcs[index] = '\0'; // Null-terminate

    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            lcs[index - 1] = X[i - 1];
            i--;
            j--;
            index--;
        } else if (dp[i - 1][j] > dp[i][j - 1])
            i--;
        else
            j--;
    }

    printf("\nOne Longest Common Subsequence: %s\n", lcs);
    printf("Length of LCS: %d\n", dp[m][n]);
}

// -------------------- Print All Common Subsequences --------------------
void printAllCommonSubsequences(char *X, char *Y, int m, int n, char *curr, int index) {
    if (m == 0 || n == 0) {
        if (index > 0) {
            curr[index] = '\0';
            printf("%s\n", curr);
        }
        return;
    }

    if (X[m - 1] == Y[n - 1]) {
        curr[index] = X[m - 1];
        printAllCommonSubsequences(X, Y, m - 1, n - 1, curr, index + 1);
    } else {
        printAllCommonSubsequences(X, Y, m - 1, n, curr, index);
        printAllCommonSubsequences(X, Y, m, n - 1, curr, index);
    }
}

// Utility function to print all unique subsequences (with duplicates handled)
void printAllCommonSubsequencesWrapper(char *X, char *Y, int m, int n) {
    printf("\nAll Common Subsequences (may include duplicates, reversed order):\n");
    char curr[MAX];
    printAllCommonSubsequences(X, Y, m, n, curr, 0);
}

// -------------------- Main Function --------------------
int main() {
    char X[MAX], Y[MAX];
    int dp[MAX][MAX];

    printf("Enter first string: ");
    scanf("%s", X);
    printf("Enter second string: ");
    scanf("%s", Y);

    int m = strlen(X);
    int n = strlen(Y);

    int length = LCSLength(X, Y, m, n, dp);
    printOneLCS(X, Y, m, n, dp);
    printAllCommonSubsequencesWrapper(X, Y, m, n);

    return 0;
}
