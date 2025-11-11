#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 100
#define INF 99999

// Structure for storing graph nodes
typedef struct {
    char name[50];
} Node;

Node nodes[MAX_VERTICES];
int graph[MAX_VERTICES][MAX_VERTICES];
int nodeCount = 0;

// Find index of node; if not found, create new
int getNodeIndex(char *name) {
    for (int i = 0; i < nodeCount; i++) {
        if (strcmp(nodes[i].name, name) == 0)
            return i;
    }
    strcpy(nodes[nodeCount].name, name);
    for (int j = 0; j < MAX_VERTICES; j++)
        graph[nodeCount][j] = INF; // initialize with no edge
    return nodeCount++;
}

// Find vertex with minimum key value
int minKey(int key[], int mstSet[], int n) {
    int min = INF, min_index = -1;
    for (int v = 0; v < n; v++)
        if (!mstSet[v] && key[v] < min)
            min = key[v], min_index = v;
    return min_index;
}

// Print MST edges and total weight
void printMST(int parent[], int n) {
    int total = 0;
    printf("\nEdges in the Minimum Spanning Tree:\n");
    for (int i = 1; i < n; i++) {
        printf("%s - %s \tWeight: %d\n",
               nodes[parent[i]].name, nodes[i].name, graph[i][parent[i]]);
        total += graph[i][parent[i]];
    }
    printf("\nTotal weight of MST = %d\n", total);
}

// Prim's Algorithm
void primMST(int n) {
    int parent[MAX_VERTICES];
    int key[MAX_VERTICES];
    int mstSet[MAX_VERTICES];

    for (int i = 0; i < n; i++) {
        key[i] = INF;
        mstSet[i] = 0;
    }

    key[0] = 0;     // Start from the first vertex
    parent[0] = -1; // First node is root of MST

    for (int count = 0; count < n - 1; count++) {
        int u = minKey(key, mstSet, n);
        if (u == -1) break;
        mstSet[u] = 1;

        for (int v = 0; v < n; v++) {
            if (graph[u][v] != INF && !mstSet[v] && graph[u][v] < key[v]) {
                parent[v] = u;
                key[v] = graph[u][v];
            }
        }
    }

    printMST(parent, n);
}

int main() {
    char filename[100];
    char node1[50], node2[50], type[10];
    int weight;

    printf("Enter SIF filename: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Initialize graph
    for (int i = 0; i < MAX_VERTICES; i++)
        for (int j = 0; j < MAX_VERTICES; j++)
            graph[i][j] = INF;

    while (fscanf(file, "%s %s %s %d", node1, type, node2, &weight) == 4) {
        int u = getNodeIndex(node1);
        int v = getNodeIndex(node2);
        graph[u][v] = weight;
        graph[v][u] = weight; // undirected
    }

    fclose(file);

    printf("\nGraph loaded successfully with %d vertices.\n", nodeCount);
    primMST(nodeCount);

    return 0;
}
