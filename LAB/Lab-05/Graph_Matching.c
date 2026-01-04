#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX 100

int n, m;                   // n = number of vertices in left set, m = right set
int graph[MAX][MAX];         // adjacency matrix: graph[u][v] = 1 if edge u->v exists
int matchR[MAX];             // matchR[v] = u means v in right is matched with u in left
bool seen[MAX];              // visited array for DFS

// DFS to find augmenting path
bool bpm(int u) {
    for (int v = 0; v < m; v++) {
        if (graph[u][v] && !seen[v]) {
            seen[v] = true;

            if (matchR[v] < 0 || bpm(matchR[v])) {
                matchR[v] = u;
                return true;
            }
        }
    }
    return false;
}

// Find maximum bipartite matching
int maxBPM() {
    memset(matchR, -1, sizeof(matchR));
    int result = 0;

    for (int u = 0; u < n; u++) {
        memset(seen, 0, sizeof(seen));
        if (bpm(u))
            result++;
    }
    return result;
}

int main() {
    printf("Enter number of vertices in left set: ");
    scanf("%d", &n);
    printf("Enter number of vertices in right set: ");
    scanf("%d", &m);

    printf("Enter adjacency matrix (left vertices x right vertices):\n");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &graph[i][j]);

    int maxMatch = maxBPM();
    printf("\nMaximum number of matching: %d\n", maxMatch);

    printf("Matched pairs (Right vertex <- Left vertex):\n");
    for (int v = 0; v < m; v++) {
        if (matchR[v] != -1)
            printf(" %d <- %d\n", v, matchR[v]);
    }

    return 0;
}
