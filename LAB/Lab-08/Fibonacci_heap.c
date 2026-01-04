#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

/*
 Single-file C implementation of a Fibonacci Heap with an interactive menu.
 Features implemented:
  - insert
  - get minimum
  - extract minimum
  - decrease key
  - delete node
  - union (meld) - used internally
  - display (root list)

 Usage:
  Compile: gcc -o fib_heap fibonacci_heap.c -lm
  Run:     ./fib_heap

 This program accepts initial node values from the user and provides a menu
 to run operations on the heap.
*/

typedef struct FibNode {
    int key;
    int degree;
    int mark; /* 0 or 1 */
    struct FibNode *parent;
    struct FibNode *child;
    struct FibNode *left;
    struct FibNode *right;
} FibNode;

typedef struct FibHeap {
    FibNode *min;
    int n; /* number of nodes */
} FibHeap;

/* helper: create a new node */
FibNode* make_node(int key) {
    FibNode *node = (FibNode*) malloc(sizeof(FibNode));
    node->key = key;
    node->degree = 0;
    node->mark = 0;
    node->parent = NULL;
    node->child = NULL;
    node->left = node;
    node->right = node;
    return node;
}

/* create an empty heap */
FibHeap* create_heap() {
    FibHeap *H = (FibHeap*) malloc(sizeof(FibHeap));
    H->min = NULL;
    H->n = 0;
    return H;
}

/* insert node into root list */
void insert_node(FibHeap *H, FibNode *x) {
    if (H->min == NULL) {
        H->min = x;
    } else {
        /* insert x into root list (to the right of min) */
        x->right = H->min->right;
        x->left = H->min;
        H->min->right->left = x;
        H->min->right = x;
        if (x->key < H->min->key) H->min = x;
    }
    H->n++;
}

FibNode* heap_insert(FibHeap *H, int key) {
    FibNode *x = make_node(key);
    insert_node(H, x);
    return x;
}

/* merge two root lists; returns resulting heap (H1 modified) */
FibHeap* heap_union(FibHeap *H1, FibHeap *H2) {
    FibHeap *H = create_heap();
    H->min = H1->min;
    if (H->min == NULL) {
        H->min = H2->min;
    } else if (H2->min != NULL) {
        /* concatenate root lists H1 and H2 */
        FibNode *a = H->min->right;
        FibNode *b = H2->min->left;

        H->min->right = H2->min->right;
        H2->min->right->left = H->min;

        b->right = a;
        a->left = b;

        if (H2->min->key < H->min->key) H->min = H2->min;
    }
    H->n = H1->n + H2->n;
    /* free H1 and H2 struct headers, but not nodes */
    free(H1);
    free(H2);
    return H;
}

/* internal: link y under x (remove y from root list and make it child of x) */
void fib_link(FibHeap *H, FibNode *y, FibNode *x) {
    /* remove y from root list */
    y->left->right = y->right;
    y->right->left = y->left;
    /* make y a child of x */
    y->parent = x;
    if (x->child == NULL) {
        x->child = y;
        y->right = y;
        y->left = y;
    } else {
        y->right = x->child->right;
        y->left = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
    y->mark = 0;
}

/* consolidate root list after extract-min */
void consolidate(FibHeap *H) {
    if (H->min == NULL) return;

    int maxDegree = (int) (log(H->n) / log(2)) + 5; /* safe upper bound */
    FibNode **A = (FibNode**) calloc(maxDegree, sizeof(FibNode*));
    for (int i = 0; i < maxDegree; ++i) A[i] = NULL;

    /* gather root nodes into an array to iterate because we will mutate root list */
    int rootsCount = 0;
    FibNode *w = H->min;
    if (w != NULL) {
        rootsCount = 1;
        w = w->right;
        while (w != H->min) {
            rootsCount++;
            w = w->right;
        }
    }

    FibNode *x = H->min;
    for (int i = 0; i < rootsCount; ++i) {
        FibNode *next = x->right;
        int d = x->degree;
        while (d >= maxDegree) {
            /* safety: reallocate A if needed */
            int old = maxDegree;
            maxDegree *= 2;
            A = (FibNode**) realloc(A, maxDegree * sizeof(FibNode*));
            for (int j = old; j < maxDegree; ++j) A[j] = NULL;
        }
        while (A[d] != NULL) {
            FibNode *y = A[d];
            if (x->key > y->key) {
                FibNode *tmp = x; x = y; y = tmp;
            }
            /* link y under x */
            fib_link(H, y, x);
            A[d] = NULL;
            d = x->degree;
        }
        A[d] = x;
        x = next;
    }

    /* rebuild root list from A */
    H->min = NULL;
    for (int i = 0; i < maxDegree; ++i) {
        if (A[i] != NULL) {
            /* isolate node */
            A[i]->left = A[i];
            A[i]->right = A[i];
            A[i]->parent = NULL;
            if (H->min == NULL) {
                H->min = A[i];
            } else {
                /* insert into root list */
                A[i]->right = H->min->right;
                A[i]->left = H->min;
                H->min->right->left = A[i];
                H->min->right = A[i];
                if (A[i]->key < H->min->key) H->min = A[i];
            }
        }
    }

    free(A);
}

/* extract and return min node */
FibNode* extract_min(FibHeap *H) {
    FibNode *z = H->min;
    if (z != NULL) {
        /* for each child of z, add to root list */
        FibNode *child = z->child;
        if (child != NULL) {
            /* iterate over children and add to root list */
            FibNode *start = child;
            FibNode *cur = start;
            do {
                FibNode *next = cur->right;
                /* add cur to root list */
                cur->parent = NULL;
                /* splice into root list */
                cur->left = H->min;
                cur->right = H->min->right;
                H->min->right->left = cur;
                H->min->right = cur;
                cur = next;
            } while (cur != start);
        }

        /* remove z from root list */
        z->left->right = z->right;
        z->right->left = z->left;
        if (z == z->right) {
            H->min = NULL;
        } else {
            H->min = z->right;
            consolidate(H);
        }
        H->n--;
    }
    return z;
}

/* cut node x from its parent y and move x to root list */
void cut(FibHeap *H, FibNode *x, FibNode *y) {
    /* remove x from child list of y */
    if (y->child == x) {
        if (x->right != x) y->child = x->right;
        else y->child = NULL;
    }
    x->left->right = x->right;
    x->right->left = x->left;
    y->degree--;

    /* add x to root list */
    x->parent = NULL;
    x->left = H->min;
    x->right = H->min->right;
    H->min->right->left = x;
    H->min->right = x;
    x->mark = 0;
}

/* cascading cut: if parent is marked, cut it too */
void cascading_cut(FibHeap *H, FibNode *y) {
    FibNode *z = y->parent;
    if (z != NULL) {
        if (y->mark == 0) {
            y->mark = 1;
        } else {
            cut(H, y, z);
            cascading_cut(H, z);
        }
    }
}

/* decrease key of node x to k (k must be <= current key) */
void decrease_key(FibHeap *H, FibNode *x, int k) {
    if (k > x->key) {
        printf("New key is greater than current key. Decrease aborted.\n");
        return;
    }
    x->key = k;
    FibNode *y = x->parent;
    if (y != NULL && x->key < y->key) {
        cut(H, x, y);
        cascading_cut(H, y);
    }
    if (x->key < H->min->key) H->min = x;
}

/* delete node x: decrease its key to -infinity and extract-min */
void heap_delete(FibHeap *H, FibNode *x) {
    decrease_key(H, x, INT_MIN);
    FibNode *r = extract_min(H);
    if (r) free(r);
}

/* utility: print root list and children counts (not exhaustive) */
void print_root_list(FibHeap *H) {
    if (H->min == NULL) {
        printf("Heap is empty.\n");
        return;
    }
    printf("Root list (key:degree):\n");
    FibNode *w = H->min;
    FibNode *cur = w;
    do {
        printf(" %d:%d", cur->key, cur->degree);
        cur = cur->right;
    } while (cur != w);
    printf("\n");
}

/* find a node by key in the heap (DFS). Note: keys may not be unique.
   We return the first match we encounter. For general use, it's better to
   keep external pointers to nodes. */
FibNode* find_node_recursive(FibNode *start, int key) {
    if (start == NULL) return NULL;
    FibNode *cur = start;
    do {
        if (cur->key == key) return cur;
        FibNode *res = find_node_recursive(cur->child, key);
        if (res != NULL) return res;
        cur = cur->right;
    } while (cur != start);
    return NULL;
}

FibNode* find_node(FibHeap *H, int key) {
    if (H->min == NULL) return NULL;
    return find_node_recursive(H->min, key);
}

/* free all nodes in heap recursively */
void free_nodes(FibNode *start) {
    if (start == NULL) return;
    FibNode *cur = start;
    do {
        FibNode *next = cur->right;
        if (cur->child) free_nodes(cur->child);
        free(cur);
        cur = next;
    } while (cur != start);
}

void free_heap(FibHeap *H) {
    if (H == NULL) return;
    if (H->min) free_nodes(H->min);
    free(H);
}

/* interactive demo */
int main() {
    FibHeap *H = create_heap();
    int n;
    printf("Enter number of initial nodes: ");
    if (scanf("%d", &n) != 1) {
        printf("Invalid input. Exiting.\n");
        return 0;
    }
    printf("Enter %d integer keys (space or newline separated):\n", n);
    for (int i = 0; i < n; ++i) {
        int k; scanf("%d", &k);
        heap_insert(H, k);
    }

    while (1) {
        printf("\n--- Fibonacci Heap Menu ---\n");
        printf("1. Insert key\n");
        printf("2. Get minimum\n");
        printf("3. Extract minimum\n");
        printf("4. Decrease key (find by key)\n");
        printf("5. Delete key (find by key)\n");
        printf("6. Print root list\n");
        printf("7. Exit\n");
        printf("Choose option: ");
        int opt; if (scanf("%d", &opt) != 1) break;
        if (opt == 1) {
            int k; printf("Enter key to insert: "); scanf("%d", &k);
            heap_insert(H, k);
            printf("Inserted %d.\n", k);
        } else if (opt == 2) {
            if (H->min) printf("Minimum: %d\n", H->min->key);
            else printf("Heap is empty.\n");
        } else if (opt == 3) {
            FibNode *m = extract_min(H);
            if (m) {
                printf("Extracted min: %d\n", m->key);
                free(m);
            } else printf("Heap is empty.\n");
        } else if (opt == 4) {
            int oldk, newk;
            printf("Enter current key to find (first match): "); scanf("%d", &oldk);
            FibNode *node = find_node(H, oldk);
            if (!node) { printf("Key %d not found.\n", oldk); }
            else {
                printf("Enter new (smaller) key: "); scanf("%d", &newk);
                decrease_key(H, node, newk);
                printf("Key decreased.\n");
            }
        } else if (opt == 5) {
            int key; printf("Enter key to delete (first match): "); scanf("%d", &key);
            FibNode *node = find_node(H, key);
            if (!node) { printf("Key %d not found.\n", key); }
            else {
                heap_delete(H, node);
                printf("Key %d deleted.\n", key);
            }
        } else if (opt == 6) {
            print_root_list(H);
        } else if (opt == 7) {
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    free_heap(H);
    printf("Exiting.\n");
    return 0;
} 
