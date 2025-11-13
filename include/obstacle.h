#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "raylib.h"

#define MAX_OBSTACLES 512
#define MAX_WORLD_COLUMNS 512

// Tipos de colunas (verticais)
typedef enum {
    COLUMN_SAFE,
    COLUMN_SAND
} ColumnType;

// Tipos de obstáculos
typedef enum {
    IMOVEL,
    MOVEL1,
    MOVEL2
} ObstacleType;

typedef struct {
    int coluna;          // índice da coluna (vertical)
    float linha;         // topo do obstáculo (em unidades de linha)
    float altura;        // altura em blocos
    ObstacleType tipo;
    float velocidade;    // linhas por segundo
    int sentido;         // 1 (descendo) ou -1 (subindo)
} Obstacle;

void InitObstacles(Obstacle *obstaculos, ColumnType *columnTypes, int worldColumns, int linhas, int *total);
void UpdateObstacles(Obstacle *obstaculos, int total, int linhas);
void DrawObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura, float cameraColumn, int screenWidth);
int CheckCollisionPlayerObstacle(void *playerPtr, Obstacle *obstaculos, int total);

#endif