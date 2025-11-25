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

    free(prev_row);
    fclose(fptr);

    return top_left;
}