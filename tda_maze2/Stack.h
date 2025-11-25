#ifndef STACK_M
#define STACK_M

typedef struct stack stack;

//Bien, cambios en el TDA
//TDA Stack - sin variables globales
stack* stack_create();
void stack_destroy(stack* s);
void stack_push(stack* s, void* data);
void* stack_pop(stack* s);
int stack_is_empty(stack* s);
int stack_size(stack* s);

#endif