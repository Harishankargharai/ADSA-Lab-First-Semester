#include <stdio.h>
#include <stdlib.h>

// Binary Search (with comparison count)
int binarySearch(int arr[], int n, int key, int *comparisons) {
    int low = 0, high = n - 1;
    *comparisons = 0;

    while (low <= high) {
        (*comparisons)++;  // for checking mid
        int mid = low + (high - low) / 2;

        if (arr[mid] == key)
            return mid;
        else if (arr[mid] < key)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1; // not found
}

// Ternary Search (with comparison count)
int ternarySearch(int arr[], int n, int key, int *comparisons) {
    int low = 0, high = n - 1;
    *comparisons = 0;

    while (low <= high) {
        int mid1 = low + (high - low) / 3;
        int mid2 = high - (high - low) / 3;

        (*comparisons)++;  // compare with mid1
        if (arr[mid1] == key) return mid1;

        (*comparisons)++;  // compare with mid2
        if (arr[mid2] == key) return mid2;

        if (key < arr[mid1])
            high = mid1 - 1;
        else if (key > arr[mid2])
            low = mid2 + 1;
        else {
            low = mid1 + 1;
            high = mid2 - 1;
        }
    }
    return -1; // not found
}

int main() {
    int n, key;

    printf("Enter size of sorted array: ");
    scanf("%d", &n);

    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Enter %d elements in sorted order:\n", n);
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    printf("Enter element to search: ");
    scanf("%d", &key);

    int comp_bin = 0, comp_ter = 0;

    int idx_bin = binarySearch(arr, n, key, &comp_bin);
    int idx_ter = ternarySearch(arr, n, key, &comp_ter);

    if (idx_bin != -1)
        printf("Binary Search: Element found at index %d\n", idx_bin);
    else
        printf("Binary Search: Element not found\n");

    if (idx_ter != -1)
        printf("Ternary Search: Element found at index %d\n", idx_ter);
    else
        printf("Ternary Search: Element not found\n");

    printf("\nNumber of comparisons in Binary Search: %d\n", comp_bin);
    printf("Number of comparisons in Ternary Search: %d\n", comp_ter);

    free(arr);
    return 0;
}
