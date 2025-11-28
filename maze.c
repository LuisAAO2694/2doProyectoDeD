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

    int row; //Fila, 
    int col; //Columanos = esto para guardar coordenadas
    int visited;
}node;

// Funcion para crear los nodos e inicializando los valores de cada nuevo nodo.
node * create_node(char value, int row, int col)
{
    node * new = malloc(sizeof(node));
    if(new == NULL) return NULL; // Hacemos un catcher del error, por si malloc no funciono correctamente.

    new->value = value;
    new->up = new->down = new->right = new->left = NULL;
    new->visited = 0;

    new->row = row; //Guardamos la fila 
    new->col = col; //Guardamos la columna 
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

            //Paso las coordenadas 
            //r = fila actual
            //c 0 columna actual
            node * current = create_node(ch, r, c);
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

    if(fscanf(fptr, "%s", buffer) == 1)
    {
        printf("Error, el archivo tiene mas filas de las mencionadas.\n");
        free(prev_row);
        fclose(fptr);
        return NULL;
    }

    free(prev_row);
    fclose(fptr);

    return top_left;
}

//Funcion para mostrar la ruta
void mostrarRuta(stack* rutaAct)
{
    //RutaAct en si es el stack que tiene la ruta desde la entrtada hasta la salida

    //Checamos si el stack de la ruta es valida o esta vacia :/
    if (rutaAct == NULL || stack_is_empty(rutaAct))
    {
        printf("No hay ruta para mostrar\n");
        return;
    }
    
    printf("\n--- Ruta de solucion ---\n");
    
    //Creo el stack temporal para checar el orden de los elementos 
    /*
        Nuestro stack original tiene: [entrada, paso1, paso2, y ps asi hasta la salida en LIFO 
    */
    stack* temp = stack_create();
    int paso = 1; //Nuestro contador de pasos, esto es para numerarlos
    
    //Transferir por 1era vez para invertir orden, del orginal al temp
    //osea ahora empezaria asi: [salida, ..... paso2, paso1, entrada]
    while (!stack_is_empty(rutaAct))
    {
        //Sacamos el elemento del stack original y lo ponemos en el temporal
        stack_push(temp, stack_pop(rutaAct));
    }
    
    //Aqui es la segunda transferencia del stack temp al stack original
    while (!stack_is_empty(temp))
    {
        //Obtenemos el siguiente nodo de la ruta (de void* a node*)
        node* n = (node*)stack_pop(temp);
        printf("Paso %d: (%d, %d)\n", paso, n->row, n->col);
        //Aqui solo devolvemos el nodo al stack original para no perder nada
        stack_push(rutaAct, n);

        //Incremento el contador de pasos
        paso++;
    }
    
    stack_destroy(temp);
    printf("--- Fin de nuestra ruta ---\n");
}

//Funcion para imprimir la ruta con el camino
/*
Recibe: top_left - Puntero a la esquina superior izquierda del maze
ruta_solucion - Stack con los nodos que forman la ruta solucion
*/
void printLaberintoCamino(node * top_left, stack* ruta_solucion)
{
    //Checar si exite el laberinto
    if (top_left == NULL)
    {
        printf("Laberinto vacio\n");
        return;
    }
    
    //Conteo de las dimensiones del maze

    int rows = 0, cols = 0; //Variables para almacenar las filas y col
    node* temp = top_left; //Nuestro puntero temporal para recorrerlo
    
    //Contar filas, para recorrer todo verticalemnte hacia abajo
    while (temp != NULL)
    {
        //Incremento de las filas
        rows++;
        temp = temp->down; //Muevo a las prox fila
    }
    
    //Contar columnas, igual recorro horizontalmente hhacia la derecha
    temp = top_left; //Reinicio el puntero al inicio 
    while (temp != NULL)
    {
        cols++; //Incremento las columnas 
        temp = temp->right; //Moverse a la sig columna 
    }
    
    //Crear matriz temporal
    char** laberinto_visual = malloc(rows * sizeof(char*));
    //Para cada fila, checamos la memoria para las columnas 
    for (int i = 0; i < rows; i++)
    {
        laberinto_visual[i] = malloc(cols * sizeof(char));
    }
    
    //Llenar con valores originales
    node* fila_actual = top_left;
    //El for para recorrer todas las filas 
    for (int r = 0; r < rows; r++)
    {
        node* col_actual = fila_actual; //Este seria el primer nodo de la fila act
        for (int c = 0; c < cols; c++)
        {
            //Copiamos el valor del nodo act a la matrtiz temp
            laberinto_visual[r][c] = col_actual->value;
            col_actual = col_actual->right; //Nos movemos a la siguiente columna 
        }
        //Aqui nos movemos a la siguiente fila 
        fila_actual = fila_actual->down;
    }
    
    //Marcamos el camino de la solucion
    if (ruta_solucion != NULL)
    {
        //Creamos otra vez el stack ttemp
        stack* temp_stack = stack_create();
        
        //Transferir para no perder el stack original
        //Yyy mienttras transferimos, marcamos las pos actual
        while (!stack_is_empty(ruta_solucion))
        {
            node* n = (node*)stack_pop(ruta_solucion);
            //Marcar cada pos de la matriz 
            laberinto_visual[n->row][n->col] = 'X';

            //Guardamos el nodo en el stack original
            stack_push(temp_stack, n);
        }
        
        //Devolver nodos al stack original
        while (!stack_is_empty(temp_stack))
        {
            stack_push(ruta_solucion, stack_pop(temp_stack));
        }
        
        stack_destroy(temp_stack);
    }
    
    //Imprimir laberinto con camino marcado
    printf("\n---- Solucion del laberinto ----\n");
    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            printf("%c ", laberinto_visual[r][c]);
        }
        printf("\n");
    }
    printf("\n");
    printf("1 = Pared, 0 = Camino, X = Ruta solucion\n");
    
    //Liberar memoria
    for (int i = 0; i < rows; i++)
    {
        free(laberinto_visual[i]);
    }
    free(laberinto_visual);
}

//Funcion para imprimir el laberinto, recibe como parametro el apuntador a la esquina superior izquierda.
void print_maze(node * top_left)
{
    node * inicio = top_left; //Nuestro puntero que recorre las filas

    //Aqui con este while pues solo recorremos todas las filas act
    while(inicio != NULL)
    {
        node * n = inicio;
        while(n)
        {
            printf("%c", n->value);
            n = n->right; //Me muevo a la sigueinte col
        }
        printf("\n");
        inicio = inicio->down; //Muevo a la siguiente fila 
    }

}

//Funcion para liberar todo el laberinto
void destroy_maze(node * top_left)
{
    node * inicio = top_left; //Igual nuestro puntero para reccorrer las filas 
    
    while(inicio != NULL)
    {
        //Guardamos la referencia a la siguiente fila antes de liberar la act
        node * n = inicio->down;
        node * m = inicio; //Puntero para recorrer y liberar nodos de esta fila
        
        //Recorrer y liberamos todos los nodos de la fila act
        while(m)
        {
            node * next = m->right; //Guardar ref
            free(m); //Liberar
            m = next; //Nos vamos al sig
        }
        inicio = n; //Y seguimos con la siguiente fila 
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

//Version simplificada
//Recibe: entrada - Puntero al nodo de entrada del maze
//Retorna: Stack* con la ruta completa desde entrada hasta salida, o NULL si no hay nada 
stack* solve_maze(node* entrada)
{
    //Checamos si no hay entarda, no se puede solucionar el lab
    if(entrada == NULL) 
    {
        return NULL;
    }

    //Bien aqui creamos los Stacks:
    /*
        pila_principal: Stack para nodos pendientes por explorar
        ruta_solucion: Stack que guarda el historial completo de nodos que se visitaron (en orden creo - al final si fue por orden jajaja)
    */
    stack* pila_principal = stack_create();
    stack* ruta_solucion = stack_create();
    
    //Comenzamos por la entrada, lo agregamos a la pila principal y ya pues la ponemos como visitada
    stack_push(pila_principal, entrada);
    entrada->visited = 1; //La marccamos como la visitada 
    
    //Continua mientras haya nodos pendientes por explorar en la pila principal
    while (!stack_is_empty(pila_principal)) 
    {
        //Tomamos el ultimo nodo agregado LIFO
        //En si checamos cada camino a detalle 
        node* current = (node*)stack_pop(pila_principal);
        //Guardamos este nodo en el stack para poder hacerle un tracking al camino
        stack_push(ruta_solucion, current);
        
        // Aqui checamos si el nodo actual cumple con las condiciones del exit
        //Esta en la primera fila (n->up == NULL)
        //Es un path valido (value == '0')
        if (isExit(current)) 
        {
            printf("¡Salida Encontrada!\n");
            stack_destroy(pila_principal);
            return ruta_solucion;
        }
        
        //Explorar vecinos en orden: arriba, derecha, abajo, izquierda
        node* vecinos[] = {current->up, current->right, current->down, current->left};
        
        //Aqui checamos cada uno de los 4 posibles vecinos 
        for (int i = 0; i < 4; i++) 
        {
            node* vecino = vecinos[i];

            //Checamos si mi vecino es explorable
            /*
            vecino != NULL: El vecino existe xd
            vecino->value == '0': Es un camino, no una paredd
            !vecino->visited: No ha sido visitado antes (evita ciclos)
            */
            if (vecino != NULL && vecino->value == '0' && !vecino->visited) 
            {
                //Marco el vecino ccomo visitado para no checarlo. otra vez 
                vecino->visited = 1;

                //Agregamos el vecino a la pila principal para explorarlo después
                stack_push(pila_principal, vecino);
            }
        }

        /*
        OJO: Si no hay vecinos explorables, el algoritmo retrocede
        al sacar el siguiente nodo de la pila principal (backtracking<-investigando es algo que tambien se aplica en los juegos)
        */
    }

    //Si llegamos Aqui no tiene solucion
    printf("El laberinto no tiene solucion :/");
    stack_destroy(pila_principal);
    stack_destroy(ruta_solucion);
    return NULL;
}

int main()
{
    node * maze = build_maze("prueba7.txt");
    if (maze == NULL)
    {
        printf("Error cargando el laberinto\n");
        return 1;
    }

    printf("Laberinto: \n");
    print_maze(maze);

    //Lo de resolver el laberinto
    node* entrance = encontrarEntrada(maze);
    if(entrance != NULL)
    {
        printf("\nEntrada encontrada en: (%d, %d)\n", entrance->row, entrance->col);
        
        stack* ruta_solucion = solve_maze(entrance);
        
        if (ruta_solucion != NULL)
        {
            //Aqui mostramos la ruta solucion
            mostrarRuta(ruta_solucion);
            
            //Mostramos el laberinto con el camino marcado
            printLaberintoCamino(maze, ruta_solucion);
            
            //Liberar el stack de la ruta <- IMPORTANTE
            stack_destroy(ruta_solucion);
            
            printf("\nEl laberinto TIENE solucion\n");
        }
        else
        {
            printf("\nEl laberinto NO tiene solucion\n");
        }
    }
    else
    {
        printf("No se encontro una entrada\n");
    }
    
    destroy_maze(maze);
    return 0;
}
