#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_NAME_LEN 50
#define MAX_LINE_LEN 200

// Structure to represent an edge
typedef struct Edge {
    int to;
    char interaction[10];
    struct Edge *next;
} Edge;

// Structure to represent a graph node
typedef struct Node {
    char name[MAX_NAME_LEN];
    Edge *edges;
} Node;

Node graph[MAX_NODES];
int nodeCount = 0;

// Find node index by name; if not found, create new node
int getNodeIndex(const char *name) {
    for (int i = 0; i < nodeCount; i++) {
        if (strcmp(graph[i].name, name) == 0)
            return i;
    }
    // New node
    strcpy(graph[nodeCount].name, name);
    graph[nodeCount].edges = NULL;
    return nodeCount++;
}

// Add edge to adjacency list
void addEdge(int from, int to, const char *interaction) {
    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->to = to;
    strcpy(newEdge->interaction, interaction);
    newEdge->next = graph[from].edges;
    graph[from].edges = newEdge;
}

int main() {
    char line[MAX_LINE_LEN];
    char node1[MAX_NAME_LEN], node2[MAX_NAME_LEN], interaction[10];
    char filename[100];

    printf("Enter SIF filename: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%s %s %s", node1, interaction, node2) == 3) {
            int idx1 = getNodeIndex(node1);
            int idx2 = getNodeIndex(node2);
            addEdge(idx1, idx2, interaction);
        }
    }
    fclose(file);

    printf("\nGraph loaded successfully!\n\n");

    // Print adjacency list
    for (int i = 0; i < nodeCount; i++) {
        printf("%s -> ", graph[i].name);
        Edge *edge = graph[i].edges;
        while (edge) {
            printf("%s (%s) ", graph[edge->to].name, edge->interaction);
            edge = edge->next;
        }
        printf("\n");
    }

    return 0;
}
