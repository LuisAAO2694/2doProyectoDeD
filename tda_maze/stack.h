#ifndef STACK_H
#define STACK_H

typedef struct stack Stack;

//Constructor y destructor
Stack* stackCrear(void);
void stackDestruir(Stack* stack);

// Operaciones básicas
void stackPush(Stack* stack, void* data);
void* stackPop(Stack* stack);
int stackIsEmpty(const Stack* stack);
void* stackPeek(const Stack* stack);

#endif