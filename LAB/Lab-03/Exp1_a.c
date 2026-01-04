#include <stdio.h>
#include <stdbool.h>

// Function prototypes
bool isSorted(int arr[], int n);
int binarySearchRecursive(int arr[], int low, int high, int key);
int binarySearchIterative(int arr[], int n, int key);

int main() {
    int arr[100], n, key, result;

    printf("Enter number of elements in array: ");
    scanf("%d", &n);

    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Check if array is sorted
    if (!isSorted(arr, n)) {
        printf("The array is not sorted. Binary search requires a sorted array.\n");
        return 0;
    }

    printf("Enter key to search: ");
    scanf("%d", &key);

    // Iterative Binary Search
    result = binarySearchIterative(arr, n, key);
    if (result != -1)
        printf("Key found at index %d using iterative binary search.\n", result);
    else
        printf("Key not found using iterative binary search.\n");

    // Recursive Binary Search
    result = binarySearchRecursive(arr, 0, n - 1, key);
    if (result != -1)
        printf("Key found at index %d using recursive binary search.\n", result);
    else
        printf("Key not found using recursive binary search.\n");

    return 0;
}

// Function to check if array is sorted in ascending order
bool isSorted(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1])
            return false;
    }
    return true;
}

// Iterative Binary Search
int binarySearchIterative(int arr[], int n, int key) {
    int low = 0, high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;

        if (arr[mid] == key)
            return mid;
        else if (arr[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }

    return -1;
}

// Recursive Binary Search
int binarySearchRecursive(int arr[], int low, int high, int key) {
    if (low > high)
        return -1;

    int mid = low + (high - low) / 2;

    if (arr[mid] == key)
        return mid;
    else if (arr[mid] < key)
        return binarySearchRecursive(arr, mid + 1, high, key);
    else
        return binarySearchRecursive(arr, low, mid - 1, key);
}
