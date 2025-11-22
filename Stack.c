#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

struct stack_node {
int info;
struct stack_node *prev;
};

/* STACK GLOBAL */
stack_node *stack_top = NULL;
int count = 0;


void push(int data)
{
    stack_node *temp = NULL;

    if (stack_top == NULL)
    {
        stack_top = (stack_node *)malloc(sizeof(struct stack_node));
        stack_top->prev = NULL;
        stack_top->info = data;
    }
    else
    {
        temp = (stack_node *)malloc(sizeof(struct stack_node));
        temp->prev = stack_top;
        temp->info = data;
        stack_top = temp;
    }
    count++;
    printf("\tNode is Inserted\n");
}


int pop()
{
    stack_node *nptr = stack_top;
    if (nptr == NULL)
    {
        printf("\nStack Underflow\n");
        return -1;
    }
    else
    {
        nptr = nptr->prev;
    }
    int popped = stack_top->info;
    free(stack_top);
    stack_top = nptr;
    count--;
    return popped;
}

void display()
{
// Display the elements of the stack
stack_node * nptr = stack_top;
if (nptr == NULL)
{
    printf("\nStack Underflow\n");
    return;
}
printf("The stack is \n");
while (nptr != NULL)
{
    printf("%d--->", nptr->info);
    nptr = nptr->prev;
}
printf("NULL\n\n");
}

