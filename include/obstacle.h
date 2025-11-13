#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "raylib.h"

// Tipos de Obstáculos
typedef enum {
    IMOVEL,
    MOVEL1,
    MOVEL2
} ObstacleType;

typedef struct {
    int linha;
    int coluna;
    ObstacleType tipo;
    float yOffset;
    int sentido;
} Obstacle;

// Funções de Obstáculos
void InitObstacles(Obstacle *obstaculos, int colunas, int linhas, int *total);
void UpdateObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura, int linhas);
void DrawObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura);

// Colisão
int CheckCollisionPlayerObstacle(void *playerPtr, Obstacle *obstaculos, int total);

#endif 
