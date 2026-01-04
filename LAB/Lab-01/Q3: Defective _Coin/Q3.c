#include <stdio.h>

// Function to simulate weighing two groups of coins
// Returns: -1 if left < right, 1 if right < left, 0 if equal
int weigh(int coins[], int l1, int r1, int l2, int r2) {
    int sum1 = 0, sum2 = 0;
    for (int i = l1; i <= r1; i++) sum1 += coins[i];
    for (int i = l2; i <= r2; i++) sum2 += coins[i];

    if (sum1 < sum2) return -1; 
    else if (sum1 > sum2) return 1; 
    else return 0;
}

// Divide and Conquer to find defective coin
int findDefective(int coins[], int left, int right) {
    if (left == right) {
        // Only one coin remains, must be defective
        return left;
    }

    int mid = (left + right) / 2;
    int sizeLeft = mid - left + 1;
    int sizeRight = right - mid;

    // Compare left half with right half (use only equal size parts)
    int minSize = sizeLeft < sizeRight ? sizeLeft : sizeRight;
    int result = weigh(coins, left, left + minSize - 1, right - minSize + 1, right);

    if (result == -1) {
        // left half lighter → defective is in left
        return findDefective(coins, left, left + minSize - 1);
    } else if (result == 1) {
        // right half lighter → defective is in right
        return findDefective(coins, right - minSize + 1, right);
    } else {
        // both halves equal → defective (if any) is in the middle leftover
        if ((sizeLeft != sizeRight)) {
            return findDefective(coins, left + minSize, right - minSize);
        } else {
            // No defective coin
            return -1;
        }
    }
}

int main() {
    int n;
    printf("Enter number of coins: ");
    scanf("%d", &n);

    int coins[n];
    printf("Enter weights of coins:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &coins[i]);
    }

    int idx = findDefective(coins, 0, n - 1);
    if (idx == -1)
        printf("No defective coin found (all are equal).\n");
    else
        printf("Defective (lighter) coin is at index %d )\n", idx);

    return 0;
}
