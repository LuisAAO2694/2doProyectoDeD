#include <stdio.h>
#include <stdlib.h>
#include "tda_maze2/Stack.h"

typedef struct node
{
    char value; // Camino = '0' y Pared = '1'.
    struct node * up; // apunta al nodo de arriba.
    struct node * down; // apunta al nodo de abajo.
    struct node * left; // apunta al nodo de la izquierda.
    struct node * right; // apunta al nodo de la derecha.

    int visited;
}node;

// Funcion para crear los nodos e inicializando los valores de cada nuevo nodo.
node * create_node(char value)
{
    node * new = malloc(sizeof(node));
    if(new == NULL) return NULL; // Hacemos un catcher del error, por si malloc no funciono correctamente.

    new->value = value;
    new->up = new->down = new->right = new->left = NULL;
    new->visited = 0;
    return new;
}

// Funcion que recibe el nombre del archivo como parametro para poder armar el laberinto, retorna el apuntador a la esquina superior izquierda.
node * build_maze(char * filename)
{
    FILE * fptr = fopen(filename, "r");
    if(fptr == NULL)
    {
        printf("Error abriendo el archivo.");
        fclose(fptr);
        return NULL;
    }

    int rows, column;
    if(fscanf(fptr, "%d %d", &rows, &column) != 2)
    {
        printf("Error, la primera linea del archivo debe contener las filas y columnas.\n");
        fclose(fptr);
        return NULL;
    }

    printf("Filas: %d, Columnas: %d\n", rows, column); //Para debug

    node ** prev_row = calloc(column, sizeof(node *)); // Creamos prev_row para recordar por cada columna, cuál fue el nodo de la fila anterior, y así poder conectar cada nodo nuevo con su vecino de arriba.
    if(prev_row == NULL)
    {
        printf("Error reservando memoria para prev_row.\n");
        fclose(fptr);
        return NULL;
    }

    node * top_left = NULL; // Va a contener la direccion del nodo esquina superior izquierda.

    char buffer[1024];

    for(int r = 0; r < rows; r++)
    {
        if(fscanf(fptr, "%s", buffer) != 1){
            printf("Error, faltan filas, se esperaban %d filas y solo hay %d filas\n", rows, r);
            free(prev_row);
            fclose(fptr);
            return NULL;
        }

        int len = 0;
        while (buffer[len] != '\0'){
            len++;
        }

        if(len != column){
            printf("Error, la fila %d tiene %d columnas y se esperan %d\n", r, len, column);
            free(prev_row);
            fclose(fptr);
            return NULL;
        }

        node * anterior_samefila = NULL; // Nodo anterior en la misma fila.

        for(int c = 0; c < column; c++)
        {
            char ch = buffer[c];

            if(ch != '0' && ch != '1')
            { // Checamos que los caracteres del archivo sean validos, solo podemos recibir '0' o '1'.
                printf("Error, hay un caracter invalido en el archivo.");
                free(prev_row);
                fclose(fptr);
                return NULL;
            }

            node * current = create_node(ch);
            if(current == NULL)
            {
                printf("Error reservando nodo.\n");
                free(prev_row);
                fclose(fptr);
                return NULL;
            }

            if(anterior_samefila)
            {
                current->left = anterior_samefila;
                anterior_samefila->right = current;
            }

            if(prev_row[c])
            {
                current->up = prev_row[c];
                prev_row[c]->down = current;
            }

            prev_row[c] = current;
            anterior_samefila = current;

            // Guardamos el nodo esquina superiro izquierda.
            if(r == 0 && c == 0)
            {
                top_left = current;
            }
        }
    }

    if(fscanf(fptr, "%s", buffer) == 1){
        printf("Error, el archivo tiene mas filas de las mencionadas.\n");
        free(prev_row);
        fclose(fptr);
        return NULL;
    }

    free(prev_row);
    fclose(fptr);

    return top_left;
}

// Funcion para imprimir el laberinto, recibe como parametro el apuntador a la esquina superior izquierda.
void print_maze(node * top_left){
    node * inicio = top_left;

    while(inicio != NULL)
    {
        node * n = inicio;
        while(n)
        {
            printf("%c", n->value);
            n = n->right;
        }
        printf("\n");
        inicio = inicio->down;
    }

}

// Funcion para liberar todo el laberinto.
void destroy_maze(node * top_left)
{
    node * inicio = top_left;
    
    while(inicio != NULL)
    {
        node * n = inicio->down;
        node * m = inicio;
        
        while(m)
        {
            node * next = m->right;
            free(m);
            m = next;
        }
        inicio = n;
    }
}

//Funcion para encontrar la entrada (ultima fila *desde abajo xd)
node *encontrarEntrada(node* top_left)
{
    //Aqui primeramente checamos si el laberinto existe
    if(top_left == NULL)
    {
        return NULL;
    }

    //Ahora bien, navegamos hastta la ultima fila del laberinto
    //Este es nuestro apuntador auxiliar que apuenta a la esquina superior izquierda
    node* ultima_fila = top_left;
    //En este while empezamos desde la esquina superior izquierda y bajamos hasta el final
    //Recorremos, ya que la entrada siempre esta hasta el fondo
    while(ultima_fila->down != NULL)
    {
        //Avanzamos un nivel hahcia abajo
        ultima_fila = ultima_fila->down;
    }

    //Creamos otro nodo temp para recorrernos horizontalemnte
    node* current = ultima_fila;

    //En este bucle recorre horizontalmente la ultima fila
    //Mientras current no sea NULL 
    while(current != NULL)
    {
        //Checamos si el nodo es un camino o una pared
        //caminos "0" y paredes "1"
        if(current->value == '0')
        {
            //Si encontramos el camino, retornamos el puntero a ese nodo que sera nuestra inicio
            //Espero haberme dado a entender
            return current;
        }
        //Si no es el camino le sigo a la derecha xd
        current = current->right;
    }
    return NULL;
}

//Funcion para checar si un nodo es la salida (la primera fila de arriba)
int isExit(node* n)
{
    //Checamos que el puntero al nodo no sea NULL
    if(n == NULL)
    {
        return 0;
    }

    //Un nodo es considerado salida si se cumple:
    //1. n->up == NULL: No tiene nodo arriba, lo que significa que está en la primera fila
    //2. n->value == '0'
    return (n->up == NULL && n->value == '0');
}

//Version beta del algoritmo para resolver el algoritmo (usando el stack)
int solve_maze(node* entrada)
{
    //Checo si la entrada es valida,
    //Si es null no se puede resolver
    if(entrada == NULL)
    {
        return 0;
    }

    stack* rutaDeLaPila = stack_create();
    
    //Insertamos el nodo en la pila 
    //Aqui empieza todo
    stack_push(rutaDeLaPila, entrada);
    //Marcamos el nodo de entrada como visitado
    //Esto previene que volvamos a. visitar el mismo nodo ;)
    entrada->visited = 1;

    //Mientrtas haya nodos que explorar seguimos buscando la salida
    while(!stack_is_empty(rutaDeLaPila))
    {
        //Convierto el void* a node* ya que stack_pop retorna void*
        node* current = (node*)stack_pop(rutaDeLaPila);

        //Checamos si el nodo actual es la salida del laberinto
        if(isExit(current))
        {
            printf("Se encontro la salida!!!");
            stack_destroy(rutaDeLaPila);
            return 1;
        }

        //Aqui checamos los "vecinos xd" (osea sus alrededores)
        //Arriba, Derecha, Abajo, Izquierda
        node* vecinos[] = {current->up, current->right, current->down, current->left};

        //Aqui checamos cada uno de los posibles vecinos 
        for(int ay = 0; ay < 4; ay++)
        {
            node* vecino = vecinos[ay];
            if(vecino != NULL && vecino->value == '0' && !vecino->visited)
            {
                vecino->visited = 1;
                stack_push(rutaDeLaPila, vecino);
            }
        }
    }

    printf("El laberinto no tiene solucion :/");
    stack_destroy(rutaDeLaPila);
    return 0;
}

int main()
{
    node * maze = build_maze("prueba4.txt");

    printf("Laberinto: \n");
    print_maze(maze);

    //Lo de resolver el laberinto
    node* entrance = encontrarEntrada(maze);
    if(entrance != NULL)
    {
        printf("\n");
        printf("Entrada encontrada\n");
        int tendraSolucion = solve_maze(entrance);
        printf("\nEl laberinto %s solucion\n", tendraSolucion ? "Tiene" : "No tiene");
    }
    else
    {
        printf("No se encontro una entrada\n");
    }
    
    destroy_maze(maze);
}
