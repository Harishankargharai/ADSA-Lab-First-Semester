#include <stdio.h>
#include <stdlib.h>

#define MIN_DEGREE 3   // Minimum degree (t)

// B-Tree node structure
struct BTreeNode {
    int keys[2 * MIN_DEGREE - 1];           // An array of keys
    struct BTreeNode *children[2 * MIN_DEGREE]; // Array of child pointers
    int n;                                  // Current number of keys
    int leaf;                               // 1 if node is leaf
};

// Function to create a new B-Tree node
struct BTreeNode *createNode(int leaf) {
    struct BTreeNode *newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode));
    newNode->leaf = leaf;
    newNode->n = 0;
    return newNode;
}

// Function to traverse the B-Tree (Inorder)
void traverse(struct BTreeNode *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->n; i++) {
            if (!root->leaf)
                traverse(root->children[i]);
            printf("%d ", root->keys[i]);
        }
        if (!root->leaf)
            traverse(root->children[i]);
    }
}

// Function to search a key in the B-Tree
struct BTreeNode *search(struct BTreeNode *root, int k) {
    int i = 0;
    while (i < root->n && k > root->keys[i])
        i++;

    if (i < root->n && root->keys[i] == k)
        return root;

    if (root->leaf)
        return NULL;

    return search(root->children[i], k);
}

// Split the child y of node x at index i
void splitChild(struct BTreeNode *x, int i, struct BTreeNode *y) {
    struct BTreeNode *z = createNode(y->leaf);
    z->n = MIN_DEGREE - 1;

    // Copy last (t-1) keys of y to z
    for (int j = 0; j < MIN_DEGREE - 1; j++)
        z->keys[j] = y->keys[j + MIN_DEGREE];

    // Copy the last t children of y to z
    if (!y->leaf) {
        for (int j = 0; j < MIN_DEGREE; j++)
            z->children[j] = y->children[j + MIN_DEGREE];
    }

    y->n = MIN_DEGREE - 1;

    // Create space for new child
    for (int j = x->n; j >= i + 1; j--)
        x->children[j + 1] = x->children[j];

    x->children[i + 1] = z;

    // Move middle key up
    for (int j = x->n - 1; j >= i; j--)
        x->keys[j + 1] = x->keys[j];

    x->keys[i] = y->keys[MIN_DEGREE - 1];
    x->n += 1;
}

// Insert key into non-full node
void insertNonFull(struct BTreeNode *x, int k) {
    int i = x->n - 1;

    if (x->leaf) {
        while (i >= 0 && k < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = k;
        x->n += 1;
    } else {
        while (i >= 0 && k < x->keys[i])
            i--;

        i++;
        if (x->children[i]->n == 2 * MIN_DEGREE - 1) {
            splitChild(x, i, x->children[i]);
            if (k > x->keys[i])
                i++;
        }
        insertNonFull(x->children[i], k);
    }
}

// Insert a key into the B-Tree
struct BTreeNode *insert(struct BTreeNode *root, int k) {
    if (root == NULL) {
        root = createNode(1);
        root->keys[0] = k;
        root->n = 1;
    } else {
        if (root->n == 2 * MIN_DEGREE - 1) {
            struct BTreeNode *s = createNode(0);
            s->children[0] = root;
            splitChild(s, 0, root);

            int i = 0;
            if (s->keys[0] < k)
                i++;
            insertNonFull(s->children[i], k);

            root = s;
        } else {
            insertNonFull(root, k);
        }
    }
    return root;
}

/* ---------------------- DELETION FUNCTIONS ---------------------- */

// Get predecessor of a key
int getPredecessor(struct BTreeNode *x, int idx) {
    struct BTreeNode *cur = x->children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->n];
    return cur->keys[cur->n - 1];
}

// Get successor of a key
int getSuccessor(struct BTreeNode *x, int idx) {
    struct BTreeNode *cur = x->children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];
    return cur->keys[0];
}

// Merge two children of a node
void merge(struct BTreeNode *x, int idx) {
    struct BTreeNode *child = x->children[idx];
    struct BTreeNode *sibling = x->children[idx + 1];

    child->keys[MIN_DEGREE - 1] = x->keys[idx];

    for (int i = 0; i < sibling->n; ++i)
        child->keys[i + MIN_DEGREE] = sibling->keys[i];

    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i)
            child->children[i + MIN_DEGREE] = sibling->children[i];
    }

    for (int i = idx + 1; i < x->n; ++i)
        x->keys[i - 1] = x->keys[i];

    for (int i = idx + 2; i <= x->n; ++i)
        x->children[i - 1] = x->children[i];

    child->n += sibling->n + 1;
    x->n--;

    free(sibling);
}

// Borrow a key from the previous child
void borrowFromPrev(struct BTreeNode *x, int idx) {
    struct BTreeNode *child = x->children[idx];
    struct BTreeNode *sibling = x->children[idx - 1];

    for (int i = child->n - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = x->keys[idx - 1];
    if (!x->leaf)
        child->children[0] = sibling->children[sibling->n];

    x->keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

// Borrow a key from the next child
void borrowFromNext(struct BTreeNode *x, int idx) {
    struct BTreeNode *child = x->children[idx];
    struct BTreeNode *sibling = x->children[idx + 1];

    child->keys[child->n] = x->keys[idx];

    if (!child->leaf)
        child->children[child->n + 1] = sibling->children[0];

    x->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; ++i)
        sibling->keys[i - 1] = sibling->keys[i];

    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->n += 1;
    sibling->n -= 1;
}

// Fill child if it has less than t-1 keys
void fill(struct BTreeNode *x, int idx) {
    if (idx != 0 && x->children[idx - 1]->n >= MIN_DEGREE)
        borrowFromPrev(x, idx);
    else if (idx != x->n && x->children[idx + 1]->n >= MIN_DEGREE)
        borrowFromNext(x, idx);
    else {
        if (idx != x->n)
            merge(x, idx);
        else
            merge(x, idx - 1);
    }
}

// Delete key from node
struct BTreeNode *deleteKey(struct BTreeNode *x, int k) {
    int idx = 0;
    while (idx < x->n && k > x->keys[idx])
        ++idx;

    if (idx < x->n && x->keys[idx] == k) {
        if (x->leaf) {
            for (int i = idx + 1; i < x->n; ++i)
                x->keys[i - 1] = x->keys[i];
            x->n--;
        } else {
            if (x->children[idx]->n >= MIN_DEGREE) {
                int pred = getPredecessor(x, idx);
                x->keys[idx] = pred;
                x->children[idx] = deleteKey(x->children[idx], pred);
            } else if (x->children[idx + 1]->n >= MIN_DEGREE) {
                int succ = getSuccessor(x, idx);
                x->keys[idx] = succ;
                x->children[idx + 1] = deleteKey(x->children[idx + 1], succ);
            } else {
                merge(x, idx);
                x = deleteKey(x->children[idx], k);
            }
        }
    } else {
        if (x->leaf)
            return x;

        int flag = (idx == x->n);
        if (x->children[idx]->n < MIN_DEGREE)
            fill(x, idx);

        if (flag && idx > x->n)
            x->children[idx - 1] = deleteKey(x->children[idx - 1], k);
        else
            x->children[idx] = deleteKey(x->children[idx], k);
    }
    return x;
}

// Main driver
int main() {
    struct BTreeNode *root = NULL;
    int choice, key;

    while (1) {
        printf("\n--- B-Tree Menu ---\n");
        printf("1. Insert\n2. Delete\n3. Display (Inorder)\n4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            printf("Enter key to insert: ");
            scanf("%d", &key);
            root = insert(root, key);
            printf("Inserted %d\n", key);
            break;
        case 2:
            printf("Enter key to delete: ");
            scanf("%d", &key);
            if (root)
                root = deleteKey(root, key);
            printf("Deleted %d (if existed)\n", key);
            break;
        case 3:
            printf("B-Tree contents: ");
            traverse(root);
            printf("\n");
            break;
        case 4:
            exit(0);
        default:
            printf("Invalid choice!\n");
        }
    }
    return 0;
}
