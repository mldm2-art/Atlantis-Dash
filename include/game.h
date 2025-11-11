#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "obstacle.h"

// Estados do jogo
typedef enum {
    MENU,
    JOGANDO
} GameState;

// Estrutura do jogador
typedef struct {
    int linha;
    int coluna;
    float blocoTamanho;
    float areaY;
} Player;

#define MAX_OBSTACLES 256

// Estrutura principal do jogo
typedef struct {
    GameState estado;

    // Informações da janela
    int screenWidth;
    int screenHeight;

    // Grid
    int linhas;
    int colunas;
    float hudAltura;
    float blocoTamanho;

    // Jogador
    Player player;
    Texture2D playerTexture;   // ✅ Sprite do jogador (96x96 PNG)

    // Menu
    int menuSelecionado;

    // Obstáculos
    Obstacle obstaculos[MAX_OBSTACLES];
    int totalObstaculos;

} Game;

// Funções principais
Game InitGame(int screenWidth, int screenHeight);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game); // ✅ nova função para liberar a textura

#endif
