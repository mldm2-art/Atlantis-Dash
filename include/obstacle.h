#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "raylib.h"
#include <stdbool.h>

// Tipos de obstáculo (fixos + móveis)
typedef enum {
    // Fixos (colunas de areia)
    OBSTACULO_PEDRA,
    OBSTACULO_CORAL,
    OBSTACULO_CONCHA,
    OBSTACULO_ALGA,

    // Móveis (colunas de mar)
    OBSTACULO_TUBARAO,
    OBSTACULO_CARANGUEJO,
    OBSTACULO_AGUA_VIVA,
    OBSTACULO_BALEIA
} ObstaculoTipo;

// Nó da lista encadeada de obstáculos
typedef struct Obstacle {
    ObstaculoTipo tipo;

    // posição no MUNDO (NÃO é posição na tela)
    float x;       // horizontal
    float y;       // vertical

    float largura;
    float altura;

    Rectangle hitbox; // em coordenadas de MUNDO

    // Movimento vertical (para móveis)
    float velocidade;  // pixels por segundo (0 se for fixo)
    int direcao;       // +1 = descendo, -1 = subindo, 0 = parado

    struct Obstacle *next;
} Obstacle;

// Criação / destruição / lista
Obstacle *CreateObstacle(ObstaculoTipo tipo,
                         float x, float y,
                         float largura, float altura,
                         float velocidade, int direcao);

void AddObstacle(Obstacle **lista, Obstacle *novo);
void DestroyObstacleList(Obstacle **lista);

// Remove obstáculos que ficaram totalmente à esquerda da câmera
void RemoveObstaclesLeftOf(Obstacle **lista, float cameraX);

// Atualiza movimento vertical dos móveis
void UpdateObstacles(Obstacle *lista, float deltaTime,
                     float hudAltura, float screenHeight);

// Desenha todos os obstáculos (aplicando scroll horizontal)
void DrawObstacles(Obstacle *lista,
                   float cameraX,
                   float hudAltura,
                   int screenWidth,
                   int screenHeight);

// Colisão do player (hitbox em coordenadas de MUNDO) com qualquer obstáculo
Obstacle *CheckCollisionPlayerObstacles(Rectangle playerHitbox, Obstacle *lista);

#endif