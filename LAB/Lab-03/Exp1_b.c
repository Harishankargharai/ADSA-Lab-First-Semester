#include <stdio.h>

// Function prototypes
void quickSort(int arr[], int low, int high);
int partition(int arr[], int low, int high);

void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int m, int r);

void heapSort(int arr[], int n);
void heapify(int arr[], int n, int i);

void printArray(int arr[], int n);

int main() {
    int arr[100], n, choice;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter %d elements:\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    printf("\nChoose Sorting Algorithm:\n");
    printf("1. Quick Sort\n");
    printf("2. Merge Sort\n");
    printf("3. Heap Sort\n");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            quickSort(arr, 0, n - 1);
            printf("\nSorted array using Quick Sort:\n");
            break;
        case 2:
            mergeSort(arr, 0, n - 1);
            printf("\nSorted array using Merge Sort:\n");
            break;
        case 3:
            heapSort(arr, n);
            printf("\nSorted array using Heap Sort:\n");
            break;
        default:
            printf("Invalid choice!\n");
            return 1;
    }

    printArray(arr, n);
    return 0;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pivotIndex = partition(arr, low, high);
        quickSort(arr, low, pivotIndex - 1);
        quickSort(arr, pivotIndex + 1, high);
    }
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            // swap arr[i] and arr[j]
            int temp = arr[i]; arr[i] = arr[j]; arr[j] = temp;
        }
    }
    // swap arr[i+1] and arr[high]
    int temp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = temp;
    return i + 1;
}


void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[100], R[100];  // temporary arrays

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}


void heapSort(int arr[], int n) {
    // Build max-heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements from heap one by one
    for (int i = n - 1; i > 0; i--) {
        // Swap root (max) with last element
        int temp = arr[0]; arr[0] = arr[i]; arr[i] = temp;

        // Heapify reduced heap
        heapify(arr, i, 0);
    }
}

void heapify(int arr[], int n, int i) {
    int largest = i;      // Initialize largest as root
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        // Swap and continue heapifying
        int temp = arr[i]; arr[i] = arr[largest]; arr[largest] = temp;
        heapify(arr, n, largest);
    }
}
void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
