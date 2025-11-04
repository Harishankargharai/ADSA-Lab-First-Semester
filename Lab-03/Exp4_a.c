#include <stdio.h>
#include <stdlib.h>

// Structure for AVL Tree Node
struct Node {
    int key;
    struct Node *left;
    struct Node *right;
    int height;
};

// Function to get height of a node
int height(struct Node *N) {
    if (N == NULL)
        return 0;
    return N->height;
}

// Function to get maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Create a new node
struct Node* newNode(int key) {
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->key = key;
    node->left = node->right = NULL;
    node->height = 1;  // new node is initially added at leaf
    return node;
}

// Right rotate subtree rooted with y
struct Node* rightRotate(struct Node *y) {
    struct Node *x = y->left;
    struct Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x
struct Node* leftRotate(struct Node *x) {
    struct Node *y = x->right;
    struct Node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    // Return new root
    return y;
}

// Get balance factor of node N
int getBalance(struct Node *N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Function to insert a key in AVL tree
struct Node* insert(struct Node* node, int key) {
    // Perform normal BST insertion
    if (node == NULL)
        return newNode(key);

    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node; // duplicate keys not allowed

    // Update height of this ancestor node
    node->height = 1 + max(height(node->left), height(node->right));

    // Get balance factor to check whether this node became unbalanced
    int balance = getBalance(node);

    // 4 rotation cases

    // Left Left Case
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    // return the (unchanged) node pointer
    return node;
}

// Function to find the node with minimum key value
struct Node *minValueNode(struct Node* node) {
    struct Node* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Function to delete a node from AVL Tree
struct Node* deleteNode(struct Node* root, int key) {
    // Perform standard BST delete
    if (root == NULL)
        return root;

    if (key < root->key)
        root->left = deleteNode(root->left, key);
    else if (key > root->key)
        root->right = deleteNode(root->right, key);
    else {
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL)) {
            struct Node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else // One child case
                *root = *temp; // Copy contents of non-empty child

            free(temp);
        } else {
            // node with two children
            struct Node* temp = minValueNode(root->right);

            // Copy the inorder successor's data
            root->key = temp->key;

            // Delete the inorder successor
            root->right = deleteNode(root->right, temp->key);
        }
    }

    // If the tree had only one node
    if (root == NULL)
        return root;

    // Update height
    root->height = 1 + max(height(root->left), height(root->right));

    // Get balance factor
    int balance = getBalance(root);

    // Balance the tree
    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

// Preorder traversal
void preOrder(struct Node *root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preOrder(root->left);
        preOrder(root->right);
    }
}

// Main Function
int main() {
    struct Node *root = NULL;
    int choice, key;

    while (1) {
        printf("\n--- AVL Tree Menu ---\n");
        printf("1. Insert\n2. Delete\n3. Display (Preorder)\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter key to insert: ");
                scanf("%d", &key);
                root = insert(root, key);
                printf("Inserted %d successfully.\n", key);
                break;
            case 2:
                printf("Enter key to delete: ");
                scanf("%d", &key);
                root = deleteNode(root, key);
                printf("Deleted %d (if existed).\n", key);
                break;
            case 3:
                printf("Preorder traversal: ");
                preOrder(root);
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
