#include <stdio.h>
#include <stdlib.h>

// Function prototypes
void quickSortMedian(int arr[], int low, int high);
int partitionMedian(int arr[], int low, int high);
int findMedian(int arr[], int low, int high);
void swap(int *a, int *b);
void printArray(int arr[], int n);

int main() {
    int arr[100], n;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    quickSortMedian(arr, 0, n - 1);

    printf("Sorted array using Quick Sort (Median Pivot):\n");
    printArray(arr, n);

    return 0;
}
void quickSortMedian(int arr[], int low, int high) {
    if (low < high) {
        int pi = partitionMedian(arr, low, high);
        quickSortMedian(arr, low, pi - 1);
        quickSortMedian(arr, pi + 1, high);
    }
}

int partitionMedian(int arr[], int low, int high) {
    // Find median index and swap it with last element to use as pivot
    int medianIndex = findMedian(arr, low, high);
    swap(&arr[medianIndex], &arr[high]);

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}
int findMedian(int arr[], int low, int high) {
    int size = high - low + 1;
    int temp[100];

    // Copy the subarray
    for (int i = 0; i < size; i++)
        temp[i] = arr[low + i];

    // Sort the temp array
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (temp[j] > temp[j + 1]) {
                int t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    // Find median value
    int medianValue = temp[size / 2];

    // Find its index in the original array (first occurrence)
    for (int i = low; i <= high; i++) {
        if (arr[i] == medianValue)
            return i;
    }

    return low; // fallback
}
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
