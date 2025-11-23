#include <stdio.h>
#include <stdlib.h>
#include "tda_maze2/Stack.h"

typedef struct node{
    char value; // Camino = '0' y Pared = '1'.
    struct node * up; // apunta al nodo de arriba.
    struct node * down; // apunta al nodo de abajo.
    struct node * left; // apunta al nodo de la izquierda.
    struct node * right; // apunta al nodo de la derecha.
}node;

// Funcion para crear los nodos e inicializando los valores de cada nuevo nodo.
node * create_node(char value){
    node * new = malloc(sizeof(node));
    if(new == NULL) return NULL; // Hacemos un catcher del error, por si malloc no funciono correctamente.

    new->value = value;
    new->up = new->down = new->right = new->left = NULL;
    return new;
}

// Funcion que recibe el nombre del archivo como parametro para poder armar el laberinto, retorna el apuntador a la esquina superior izquierda.
node * build_maze(char * filename){
    FILE * fptr = fopen(filename, "r");
    if(fptr == NULL){
        printf("Error abriendo el archivo.");
        fclose(fptr);
        return NULL;
    }

    int rows, column;
    if(fscanf(fptr, "%d %d", &rows, &column) != 2){
        printf("Error, la primera linea del archivo debe contener las filas y columnas.\n");
        fclose(fptr);
        return NULL;
    }

    node ** prev_row = calloc(column, sizeof(node *)); // Creamos prev_row para recordar por cada columna, cuál fue el nodo de la fila anterior, y así poder conectar cada nodo nuevo con su vecino de arriba.
    if(prev_row == NULL){
        printf("Error reservando memoria para prev_row.\n");
        fclose(fptr);
        return NULL;
    }

    node * top_left = NULL; // Va a contener la direccion del nodo esquina superior izquierda.

    for(int r = 0; r < rows; r++){
        node * anterior_samefila = NULL; // Nodo anterior en la misma fila.

        for(int c = 0; c < column; c++){
            char ch;

            if(fscanf(fptr, " %c", &ch) != 1){
                printf("Error, datos insuficientes en la fila %d column %d\n", r, c);
                free(prev_row);
                fclose(fptr);
                return NULL;
            }

            if(ch != '0' && ch != '1'){ // Checamos que los caracteres del archivo sean validos, solo podemos recibir '0' o '1'.
                printf("Error, hay un caracter invalido en el archivo.");
                free(prev_row);
                fclose(fptr);
                return NULL;
            }

            node * current = create_node(ch);
            if(current == NULL){
                printf("Error reservando nodo.\n");
                free(prev_row);
                fclose(fptr);
                return NULL;
            }

            if(anterior_samefila){
                current->left = anterior_samefila;
                anterior_samefila->right = current;
            }

            if(prev_row[c]){
                current->up = prev_row[c];
                prev_row[c]->down = current;
            }

            prev_row[c] = current;
            anterior_samefila = current;

            // Guardamos el nodo esquina superiro izquierda.
            if(r == 0 && c == 0){
                top_left = current;
            }
        }
    }

    free(prev_row);
    fclose(fptr);

    return top_left;
}

// Funcion para imprimir el laberinto, recibe como parametro el apuntador a la esquina superior izquierda.

void print_maze(node * top_left){
    node * inicio = top_left;
    while(inicio != NULL){
        node * n = inicio;
        while(n){
            printf("%c", n->value);
            n = n->right;
        }
        printf("\n");
        inicio = inicio->down;
    }

}

// Funcion para liberar todo el laberinto.
void free_maze(node * top_left){
    node * inicio = top_left;
    while(inicio != NULL){
        node * n = inicio->down;
        node * m = inicio;
        while(m){
            node * next = m->right;
            free(m);
            m = next;
        }
        inicio = n;
    }
}


int main(){
    node * maze = build_maze("prueba2.txt");

    printf("Laberinto: \n");
    print_maze(maze);

    free_maze(maze);
}











