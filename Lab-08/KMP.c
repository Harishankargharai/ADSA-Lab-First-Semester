// kmp_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Read a full line from stdin, strip trailing newline and CR, return malloc'd string. */
char *read_line_trim(const char *prompt) {
    if (prompt) printf("%s", prompt);
    size_t cap = 8192;
    char *buf = (char*) malloc(cap);
    if (!buf) { perror("malloc"); exit(1); }
    if (!fgets(buf, (int)cap, stdin)) { free(buf); return NULL; }
    size_t len = strlen(buf);
    while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r')) {
        buf[--len] = '\0';
    }
    /* shrink */
    char *sh = (char*) realloc(buf, len+1);
    return sh ? sh : buf;
}

void compute_lps(const char *pat, int m, int *lps) {
    lps[0] = 0;
    int len = 0;
    int i = 1;
    while (i < m) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

/* KMP search: returns count and sets out_matches to malloc'd array */
int kmp_search(const char *text, int n, const char *pat, int m, int **out_matches) {
    if (m == 0) {
        *out_matches = (int*) malloc((n+1)*sizeof(int));
        for (int i = 0; i <= n; ++i) (*out_matches)[i] = i;
        return n+1;
    }
    int *lps = (int*) malloc(m * sizeof(int));
    if (!lps) { perror("malloc"); exit(1); }
    compute_lps(pat, m, lps);

    // debug: print lps
    printf("LPS: [");
    for (int i=0;i<m;i++){
        if (i) printf(", ");
        printf("%d", lps[i]);
    }
    printf("]\n");

    int cap = 16, cnt = 0;
    int *matches = (int*) malloc(cap * sizeof(int));
    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pat[j]) {
            i++; j++;
            if (j == m) {
                if (cnt >= cap) { cap *= 2; matches = (int*) realloc(matches, cap * sizeof(int)); }
                matches[cnt++] = i - j;
                j = lps[j-1];
            }
        } else {
            if (j != 0) j = lps[j-1];
            else i++;
        }
    }
    free(lps);
    *out_matches = matches;
    return cnt;
}

/* run one pair and print result */
void run_case(const char *text, const char *pat) {
    printf("----\nPattern: \"%s\"\nText: \"%s\"\n", pat, text);
    int n = (int)strlen(text), m = (int)strlen(pat);
    int *matches = NULL;
    int cnt = kmp_search(text, n, pat, m, &matches);
    if (cnt == 0) {
        printf("No matches found.\n");
    } else {
        printf("Matches found: %d\nPositions (0-based): ", cnt);
        for (int i=0;i<cnt;i++) {
            if (i) printf(", ");
            printf("%d", matches[i]);
        }
        printf("\n");
    }
    free(matches);
}

int main(void) {
    printf("KMP fixed tester\n\n");

    // Run the standard test cases automatically:
    run_case("ababcabcabababd", "ababd");     // expected: 10
    run_case("aaaaa", "aa");                 // expected: 0,1,2,3
    run_case("abcdef", "gh");                // expected: no match
    run_case("abcabcabcabc", "abcabc");      // expected: 0,3,6
    run_case("hello", "hello");              // expected: 0
    run_case("abcxabcdabxabcdabcdabcy", "abcdabcy"); // expected: 15

    // Now let user try interactive inputs (pattern then text)
    char *pat = read_line_trim("\nEnter pattern (or empty line to quit): ");
    if (!pat) return 0;
    if (strlen(pat) == 0) { free(pat); return 0; }
    char *text = read_line_trim("Enter text: ");
    if (!text) { free(pat); return 0; }

    run_case(text, pat);
    free(pat); free(text);
    return 0;
}
