#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "raylib.h"

#define MAX_OBSTACLES 512
#define MAX_WORLD_COLUMNS 512

/*Tipos de colunas (verticais) --- ver dps com os sprites
typedef enum {
    COLUMN_SAFE,
    COLUMN_SAND
} ColumnType; */

// ------------------------------------------------------------
// Tipos de Obstáculos (fixos e móveis)
// ------------------------------------------------------------
typedef enum {
    // Obstáculos FIXOS
    OBSTACULO_PEDRA,
    OBSTACULO_CORAL,
    OBSTACULO_CONCHA,
    OBSTACULO_ALGA,

    // Obstáculos MÓVEIS
    OBSTACULO_TUBARAO,
    OBSTACULO_CARANGUEJO,
    OBSTACULO_AGUA_VIVA,
    OBSTACULO_BALEIA

} ObstaculoTipo;

// ------------------------------------------------------------
// Estrutura completa de um único obstáculo
// ------------------------------------------------------------
typedef struct {
    ObstaculoTipo tipo;    // Tipo do obstáculo (pedra, coral, tubarão...)

    // ---- Posição no mundo (em pixels) ----
    float x;               // coordenada X real
    float y;               // coordenada Y real

    // ---- Tamanho do sprite / hitbox ----
    float largura;         // largura em pixels
    float altura;          // altura em pixels
    Rectangle hitbox;      // hitbox real usado nas colisões

    // ---- Visual ----
    Texture2D textura;     // textura correspondente ao tipo

    // ---- Movimento (apenas para os móveis) ----
    float velocidade;      // 0 para fixo, >0 para móvel
    int direcao;           // +1 para direita, -1 para esquerda

} Obstacle;



void InitObstacles(Obstacle *obstaculos, ColumnType *columnTypes, int worldColumns, int linhas, int *total);
void UpdateObstacles(Obstacle *obstaculos, int total, int linhas);
void DrawObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura, float cameraColumn, int screenWidth);
int CheckCollisionPlayerObstacle(void *playerPtr, Obstacle *obstaculos, int total);

#endif