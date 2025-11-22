#ifndef MAZE_H
#define MAZE_H

typedef struct maze Maze;

//Constructor y destructor
Maze* mazeCreate(int fila, int columna);
void mazeDestroy(Maze* maze);

//Getters
int mazeGetFilas(const Maze* maze);
int mazeGetColumnas(const Maze* maze);
char mazeGetValor(const Maze* maze, int fila, int columna);

//Setters
void mazeSetValue(Maze* maze, int fila, int columna, char value);

//Funciones de laberinto
int mazeCargaDeArchivo(Maze* maze, const char* filename);
void mazePrint(const Maze* maze);
int mazeSolve(Maze* maze);
void mazePrintSolution(const Maze* maze);

//Funciones internas
void mazeLinkNodes(Maze* maze);
int mazeEncontrarEntrada(const Maze* maze, int* fila, int* columna);
int mazeEncontrarSalida(const Maze* maze, int* fila, int* columna);

#endif