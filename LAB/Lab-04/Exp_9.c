#include <stdio.h>
#include <string.h>
#include <math.h>

#define d 256       // number of characters in input alphabet (for Rabin–Karp)
#define q 101       // a prime number (for modulo hash)

// ------------------------- KMP ALGORITHM -------------------------

void computeLPSArray(char *pat, int M, int *lps) {
    int len = 0;  // length of the previous longest prefix suffix
    lps[0] = 0;
    int i = 1;

    while (i < M) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0)
                len = lps[len - 1];
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

void KMPSearch(char *pat, char *txt) {
    int M = strlen(pat);
    int N = strlen(txt);

    int lps[M];
    computeLPSArray(pat, M, lps);

    int i = 0; // index for txt
    int j = 0; // index for pat
    int found = 0;

    while (i < N) {
        if (pat[j] == txt[i]) {
            j++;
            i++;
        }

        if (j == M) {
            printf("Pattern found at index %d\n", i - j);
            found = 1;
            j = lps[j - 1];
        } else if (i < N && pat[j] != txt[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }

    if (!found)
        printf("Pattern not found.\n");
}

// ------------------------- RABIN-KARP ALGORITHM -------------------------

void RabinKarpSearch(char *pat, char *txt) {
    int M = strlen(pat);
    int N = strlen(txt);
    int i, j;
    int p = 0; // hash value for pattern
    int t = 0; // hash value for text
    int h = 1;
    int found = 0;

    // The value of h = pow(d, M-1) % q
    for (i = 0; i < M - 1; i++)
        h = (h * d) % q;

    // Calculate the hash value of pattern and first window of text
    for (i = 0; i < M; i++) {
        p = (d * p + pat[i]) % q;
        t = (d * t + txt[i]) % q;
    }

    // Slide the pattern over text
    for (i = 0; i <= N - M; i++) {
        // Check if hash values match
        if (p == t) {
            for (j = 0; j < M; j++) {
                if (txt[i + j] != pat[j])
                    break;
            }

            if (j == M) {
                printf("Pattern found at index %d\n", i);
                found = 1;
            }
        }

        // Calculate hash value for next window
        if (i < N - M) {
            t = (d * (t - txt[i] * h) + txt[i + M]) % q;
            if (t < 0)
                t = (t + q);
        }
    }

    if (!found)
        printf("Pattern not found.\n");
}

// ------------------------- MAIN FUNCTION -------------------------

int main() {
    char text[200], pattern[50];
    int choice;

    printf("Enter the text: ");
    scanf(" %[^\n]", text);
    printf("Enter the pattern to search: ");
    scanf(" %[^\n]", pattern);

    printf("\nChoose Algorithm:\n");
    printf("1. Knuth-Morris-Pratt (KMP)\n");
    printf("2. Rabin-Karp Fingerprinting\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    printf("\n--- Output ---\n");

    switch (choice) {
        case 1:
            KMPSearch(pattern, text);
            break;
        case 2:
            RabinKarpSearch(pattern, text);
            break;
        default:
            printf("Invalid choice!\n");
    }

    return 0;
}
