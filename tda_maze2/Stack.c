#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"

typedef struct stack_node 
{
    void* data;
    struct stack_node* prev;
}stack_node;

struct stack
{
    stack_node* top;
    int count;
};

stack* stack_create() 
{
    stack* s = malloc(sizeof(stack));

    if (s == NULL) 
    {
        return NULL;
    }
    
    s->top = NULL;
    s->count = 0;
    return s;
}

void stack_destroy(stack* s) 
{
    if (s == NULL) 
    {
        return;
    }
    
    while (!stack_is_empty(s)) 
    {
        stack_pop(s);
    }
    free(s);
}

void stack_push(stack* s, void* data) 
{
    if (s == NULL) 
    {
        return;
    }
    
    stack_node* new_node = malloc(sizeof(stack_node));
    if (new_node == NULL) 
    {
        return;
    } 
    
    new_node->data = data;
    new_node->prev = s->top;
    s->top = new_node;
    s->count++;
}

void* stack_pop(stack* s) 
{
    if (s == NULL || s->top == NULL) 
    {
        return NULL;
    }
    
    stack_node* temp = s->top;
    void* data = temp->data;
    s->top = temp->prev;

    free(temp);
    s->count--;
    return data;
}

int stack_is_empty(stack* s) 
{
    return (s == NULL || s->top == NULL);
}

int stack_size(stack* s) 
{
    if (s == NULL) 
    {
        return 0;
    }
    
    return s->count;
}

void display(stack* s)
{
    if(s == NULL)
    {
        printf("El stack es null\n");
        return;
    }

    stack_node* current = s->top;
    if (current == NULL)
    {
        printf("El stack esta vacio\n");
        return;
    }

    printf("Stack contents (%d elements): ", s->count);
    while (current != NULL) 
    {
        //Como guardamos void*, necesitamos saber qué tipo de dato imprimir
        //Para nodos del laberinto, podemos imprimir coordenadas
        printf("[%p] --> ", current->data); // Dirección de memoria del nodo
        current = current->prev;
    }
    printf("NULL\n");
}

