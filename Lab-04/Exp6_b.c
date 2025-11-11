#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 100
#define MAX_EDGES 500

typedef struct {
    char name[50];
} Node;

typedef struct {
    int src, dest;
    int weight;
} Edge;

Node nodes[MAX_VERTICES];
Edge edges[MAX_EDGES];
int parent[MAX_VERTICES];
int nodeCount = 0;
int edgeCount = 0;

// ---------- Helper Functions ----------

// Find node index; create if not exists
int getNodeIndex(char *name) {
    for (int i = 0; i < nodeCount; i++) {
        if (strcmp(nodes[i].name, name) == 0)
            return i;
    }
    strcpy(nodes[nodeCount].name, name);
    return nodeCount++;
}

// Find set of an element (with path compression)
int find(int i) {
    if (parent[i] == i)
        return i;
    return parent[i] = find(parent[i]);
}

// Union of two sets
void unionSets(int x, int y) {
    int xroot = find(x);
    int yroot = find(y);
    parent[yroot] = xroot;
}

// Compare two edges based on weight
int compareEdges(const void *a, const void *b) {
    return ((Edge *)a)->weight - ((Edge *)b)->weight;
}

// Print MST
void printMST(Edge result[], int eCount) {
    int total = 0;
    printf("\nEdges in the Minimum Spanning Tree:\n");
    for (int i = 0; i < eCount; i++) {
        printf("%s - %s \tWeight: %d\n",
               nodes[result[i].src].name,
               nodes[result[i].dest].name,
               result[i].weight);
        total += result[i].weight;
    }
    printf("\nTotal weight of MST = %d\n", total);
}

// ---------- Kruskal’s Algorithm ----------
void kruskalMST() {
    Edge result[MAX_VERTICES]; // Store MST edges
    int e = 0;                 // Edge count in MST
    int i = 0;                 // Index for sorted edges

    qsort(edges, edgeCount, sizeof(Edge), compareEdges);

    for (int v = 0; v < nodeCount; v++)
        parent[v] = v;

    while (e < nodeCount - 1 && i < edgeCount) {
        Edge next = edges[i++];

        int x = find(next.src);
        int y = find(next.dest);

        // Include edge if it doesn't form a cycle
        if (x != y) {
            result[e++] = next;
            unionSets(x, y);
        }
    }

    printMST(result, e);
}

// ---------- Main Program ----------
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

    // Read edges
    while (fscanf(file, "%s %s %s %d", node1, type, node2, &weight) == 4) {
        int u = getNodeIndex(node1);
        int v = getNodeIndex(node2);

        edges[edgeCount].src = u;
        edges[edgeCount].dest = v;
        edges[edgeCount].weight = weight;
        edgeCount++;
    }

    fclose(file);

    printf("\nGraph loaded successfully with %d vertices and %d edges.\n", nodeCount, edgeCount);
    kruskalMST();

    return 0;
}
