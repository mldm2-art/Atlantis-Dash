#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "obstacle.h"

// Estados do jogo
typedef enum {
    MENU,
    INSTRUCOES,
    SELECAO_NIVEL,
    JOGANDO
} GameState;

// Estrutura do jogador
typedef struct {
    int linha;
    float coluna;
    float blocoTamanho;
    float areaY;
} Player;

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
    Texture2D playerTexture;
    Texture2D backgroundTexture;
    int menuSelecionado;
    int nivelSelecionado;

    // Vidas
    int vidas;

    // Obstáculos
    Obstacle obstaculos[MAX_OBSTACLES];
    int totalObstaculos;

    // Mundo horizontal
    int worldColumns;
    float cameraColumn;
    float playerColumnOffset;
    ColumnType columnTypes[MAX_WORLD_COLUMNS];

} Game;

// Funções principais
Game InitGame(int screenWidth, int screenHeight);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif