#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define SIZE 100

// Stack structure
struct Stack {
    int top;
    char items[SIZE];
};

// Function to initialize the stack
void initStack(struct Stack *s) {
    s->top = -1;
}

// Function to check if stack is empty
int isEmpty(struct Stack *s) {
    return s->top == -1;
}

// Function to check if stack is full
int isFull(struct Stack *s) {
    return s->top == SIZE - 1;
}

// Push operation
void push(struct Stack *s, char c) {
    if (isFull(s)) {
        printf("Stack Overflow!\n");
        exit(1);
    }
    s->items[++(s->top)] = c;
}

// Pop operation
char pop(struct Stack *s) {
    if (isEmpty(s)) {
        printf("Stack Underflow!\n");
        exit(1);
    }
    return s->items[(s->top)--];
}

// Peek operation
char peek(struct Stack *s) {
    if (isEmpty(s))
        return '\0';
    return s->items[s->top];
}

// Function to return precedence of operators
int precedence(char op) {
    switch (op) {
        case '^': return 3;
        case '*':
        case '/':
        case '%': return 2;
        case '+':
        case '-': return 1;
        default: return 0;
    }
}

// Function to check if character is operator
int isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%');
}

// Function to convert infix to postfix
void infixToPostfix(char infix[], char postfix[]) {
    struct Stack s;
    initStack(&s);
    int i, j = 0;
    char c;

    for (i = 0; infix[i] != '\0'; i++) {
        c = infix[i];

        // If operand, add to output
        if (isalnum(c)) {
            postfix[j++] = c;
        }
        // If '(', push to stack
        else if (c == '(') {
            push(&s, c);
        }
        // If ')', pop until '('
        else if (c == ')') {
            while (!isEmpty(&s) && peek(&s) != '(')
                postfix[j++] = pop(&s);
            if (!isEmpty(&s) && peek(&s) == '(')
                pop(&s); // remove '('
            else {
                printf("Invalid Expression!\n");
                exit(1);
            }
        }
        // If operator, pop higher or equal precedence operators
        else if (isOperator(c)) {
            while (!isEmpty(&s) && precedence(peek(&s)) >= precedence(c))
                postfix[j++] = pop(&s);
            push(&s, c);
        }
    }

    // Pop remaining operators
    while (!isEmpty(&s))
        postfix[j++] = pop(&s);

    postfix[j] = '\0'; // Null terminate
}

int main() {
    char infix[SIZE], postfix[SIZE];

    printf("Enter infix expression: ");
    scanf("%s", infix);

    infixToPostfix(infix, postfix);

    printf("Postfix expression: %s\n", postfix);

    return 0;
}
