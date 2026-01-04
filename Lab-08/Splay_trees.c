#include <stdio.h>
#include <stdlib.h>

/*
 Splay Tree - single-file C implementation
 - First asks for number of nodes and reads that many keys to build the tree.
 - Then provides the interactive menu (insert/search/delete/traversals/print).
 Compile:
   gcc -o splay_tree splay_tree.c
 Run:
   ./splay_tree
*/

typedef struct Node {
    int key;
    struct Node *left, *right, *parent;
} Node;

/* helper to create node */
Node* new_node(int key) {
    Node *n = (Node*) malloc(sizeof(Node));
    n->key = key;
    n->left = n->right = n->parent = NULL;
    return n;
}

/* Right rotate x (x becomes right child of its left) */
void rotate_right(Node **root, Node *x) {
    Node *y = x->left;
    if (!y) return;
    x->left = y->right;
    if (y->right) y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->right = x;
    x->parent = y;
}

/* Left rotate x (x becomes left child of its right) */
void rotate_left(Node **root, Node *x) {
    Node *y = x->right;
    if (!y) return;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

/* Splay x to root */
void splay(Node **root, Node *x) {
    if (!x) return;
    while (x->parent) {
        Node *p = x->parent;
        Node *g = p->parent;
        if (!g) {
            /* Zig step */
            if (x == p->left) rotate_right(root, p);
            else rotate_left(root, p);
        } else if ((x == p->left) && (p == g->left)) {
            /* Zig-Zig (right-right) */
            rotate_right(root, g);
            rotate_right(root, p);
        } else if ((x == p->right) && (p == g->right)) {
            /* Zig-Zig (left-left) */
            rotate_left(root, g);
            rotate_left(root, p);
        } else if ((x == p->right) && (p == g->left)) {
            /* Zig-Zag */
            rotate_left(root, p);
            rotate_right(root, g);
        } else {
            /* Zig-Zag other */
            rotate_right(root, p);
            rotate_left(root, g);
        }
    }
}

/* BST insert then splay newly inserted node */
Node* insert(Node **root, int key) {
    if (*root == NULL) {
        *root = new_node(key);
        return *root;
    }
    Node *cur = *root;
    Node *parent = NULL;
    while (cur) {
        parent = cur;
        if (key < cur->key) cur = cur->left;
        else if (key > cur->key) cur = cur->right;
        else {
            /* key already present: splay the existing node */
            splay(root, cur);
            return cur;
        }
    }
    Node *n = new_node(key);
    n->parent = parent;
    if (key < parent->key) parent->left = n;
    else parent->right = n;
    splay(root, n);
    return n;
}

/* Find node with key. If found splay it. If not found, splay last accessed node. */
Node* search(Node **root, int key) {
    Node *cur = *root;
    Node *last = NULL;
    while (cur) {
        last = cur;
        if (key == cur->key) {
            splay(root, cur);
            return cur;
        } else if (key < cur->key) cur = cur->left;
        else cur = cur->right;
    }
    if (last) splay(root, last); /* splay last accessed node */
    return NULL;
}

/* Join two trees: all keys in L <= keys in R. Return new root. */
Node* join_trees(Node *L, Node *R) {
    if (!L) {
        if (R) R->parent = NULL;
        return R;
    }
    /* Find max in L (splay it to root of L) */
    Node *cur = L;
    while (cur->right) cur = cur->right;
    splay(&L, cur); /* splay max of L to root */
    /* Now L has no right child */
    L->right = R;
    if (R) R->parent = L;
    return L;
}

/* Delete key: search and remove root if matches */
void delete_key(Node **root, int key) {
    Node *node = search(root, key);
    if (!node || node->key != key) {
        printf("Key %d not found, cannot delete.\n", key);
        return;
    }
    /* node is at root after splay */
    Node *L = node->left;
    Node *R = node->right;
    if (L) L->parent = NULL;
    if (R) R->parent = NULL;
    free(node);
    *root = join_trees(L, R);
}

/* Traversals */
void inorder(Node *root) {
    if (!root) return;
    inorder(root->left);
    printf("%d ", root->key);
    inorder(root->right);
}

void preorder(Node *root) {
    if (!root) return;
    printf("%d ", root->key);
    preorder(root->left);
    preorder(root->right);
}

/* Print tree sideways (root at left). depth controls indentation */
void print_tree(Node *root, int depth) {
    if (!root) return;
    print_tree(root->right, depth + 1);
    for (int i = 0; i < depth; ++i) printf("    ");
    printf("%d\n", root->key);
    print_tree(root->left, depth + 1);
}

/* Free all nodes */
void free_tree(Node *root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

/* Robust read integer from stdin using fgets + sscanf */
int read_int(const char *prompt, int *out) {
    char buf[128];
    if (prompt) printf("%s", prompt);
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    if (sscanf(buf, "%d", out) == 1) return 1;
    return -1;
}

int main() {
    Node *root = NULL;

    /* --- New: read number of nodes and build tree --- */
    int n;
    int r = read_int("Enter number of initial nodes: ", &n);
    if (r == 0) { printf("EOF, exiting.\n"); return 0; }
    if (r < 0 || n <= 0) {
        printf("No initial nodes (or invalid number). Continuing with empty tree.\n");
    } else {
        printf("Enter %d integer keys (one per line or space-separated):\n", n);
        for (int i = 0; i < n; ++i) {
            int k;
            int rr = read_int(NULL, &k);
            if (rr == 0) { printf("Input ended unexpectedly.\n"); break; }
            if (rr < 0) { printf("Invalid input, skipping.\n"); i--; continue; }
            insert(&root, k); /* each insert splays the inserted node to root */
        }
        printf("Initial tree built. Current root: %s\n", root ? (char[32]){0} : "NULL");
        if (root) {
            /* avoid calling sprintf into compound literal; print separately */
            printf("Root key after initial build: %d\n", root->key);
        }
    }

    /* --- interactive menu --- */
    while (1) {
        printf("\nMenu:\n");
        printf("1. Insert key\n");
        printf("2. Search key\n");
        printf("3. Delete key\n");
        printf("4. Inorder traversal\n");
        printf("5. Preorder traversal\n");
        printf("6. Print tree (sideways)\n");
        printf("7. Exit\n");
        int opt;
        int res = read_int("Choose option: ", &opt);
        if (res == 0) { printf("EOF, exiting.\n"); break; }
        if (res < 0) { printf("Invalid input, try again.\n"); continue; }

        if (opt == 1) {
            int k; if (read_int("Enter key to insert: ", &k) == 1) {
                insert(&root, k);
                printf("Inserted %d and splayed to root.\n", k);
            } else printf("Invalid input.\n");
        } else if (opt == 2) {
            int k; if (read_int("Enter key to search: ", &k) == 1) {
                Node *n = search(&root, k);
                if (n && n->key == k) printf("Found %d and splayed to root.\n", k);
                else printf("Key %d not found. Last accessed node splayed to root.\n", k);
            } else printf("Invalid input.\n");
        } else if (opt == 3) {
            int k; if (read_int("Enter key to delete: ", &k) == 1) {
                delete_key(&root, k);
            } else printf("Invalid input.\n");
        } else if (opt == 4) {
            printf("Inorder: ");
            inorder(root);
            printf("\n");
        } else if (opt == 5) {
            printf("Preorder: ");
            preorder(root);
            printf("\n");
        } else if (opt == 6) {
            if (!root) printf("Tree is empty.\n");
            else {
                printf("Tree (sideways):\n");
                print_tree(root, 0);
            }
        } else if (opt == 7) {
            break;
        } else {
            printf("Invalid option.\n");
        }
    }

    free_tree(root);
    printf("Goodbye.\n");
    return 0;
}
