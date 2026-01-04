#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 100

// Stack structure
struct Stack {
    int top;
    int items[SIZE];
};

// Initialize stack
void initStack(struct Stack *s) {
    s->top = -1;
}

// Check if stack is empty
int isEmpty(struct Stack *s) {
    return s->top == -1;
}

// Check if stack is full
int isFull(struct Stack *s) {
    return s->top == SIZE - 1;
}

// Push operation
void push(struct Stack *s, int value) {
    if (isFull(s)) {
        printf("Stack Overflow!\n");
        exit(1);
    }
    s->items[++(s->top)] = value;
}

// Pop operation
int pop(struct Stack *s) {
    if (isEmpty(s)) {
        printf("Stack Underflow!\n");
        exit(1);
    }
    return s->items[(s->top)--];
}

// Function to perform arithmetic operations
int operate(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                printf("Division by zero error!\n");
                exit(1);
            }
            return a / b;
        case '%': return a % b;
        case '^': {
            int res = 1;
            for (int i = 0; i < b; i++)
                res *= a;
            return res;
        }
        default:
            printf("Invalid operator: %c\n", op);
            exit(1);
    }
}

// Function to evaluate postfix expression
int evaluatePostfix(char postfix[]) {
    struct Stack s;
    initStack(&s);
    char c;
    int i, a, b, result;

    for (i = 0; postfix[i] != '\0'; i++) {
        c = postfix[i];

        // If operand (digit), push to stack
        if (isdigit(c)) {
            push(&s, c - '0'); // convert char to int
        }
        // If operator, pop two operands and perform operation
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^') {
            b = pop(&s);
            a = pop(&s);
            result = operate(a, b, c);
            push(&s, result);
        }
        else {
            printf("Invalid character in expression: %c\n", c);
            exit(1);
        }
    }

    // Final result
    return pop(&s);
}

// Main function
int main() {
    char postfix[SIZE];

    printf("Enter postfix expression (use single-digit operands): ");
    scanf("%s", postfix);

    int result = evaluatePostfix(postfix);

    printf("Result of postfix expression: %d\n", result);

    return 0;
}
