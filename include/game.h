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

// Estrutura do player
typedef struct {
    float x, y;             // posição na TELA (x fixo, y em blocos)
    float largura, altura;
    float blocoTamanho;
    Rectangle hitbox;       // em coordenadas de MUNDO (x + cameraX, y)
} Player;

// Estrutura principal do jogo
typedef struct {
    GameState estado;

    int screenWidth;
    int screenHeight;

    // Grid vertical (linhas horizontais)
    int linhas;
    float hudAltura;
    float blocoTamanho;

    // Player
    Player player;
    Texture2D playerTexture;

    // Fundos
    Texture2D backgroundTexture;
    Texture2D seletorNivelBackground;

    // Menus
    int menuSelecionado;
    int nivelSelecionado;

    // HUD
    int vidas;
    int pontuacao;
    int moedasColetadas;

    // Obstáculos (lista encadeada)
    Obstacle *obstaculos;

    // Mundo em colunas + câmera
    float cameraX;             // deslocamento horizontal da câmera (pixels)
    float velocidadeScroll;    // velocidade de scroll quando segura D

    int numColunasVisiveis;    // 7 (tela padrão)
    float colunaLargura;       // largura de cada coluna em pixels

    int worldColumns;          // total de colunas no nível (100)
    int primeiraColuna;        // índice da coluna mais à esquerda do mundo
    int proximaColuna;         // próxima coluna do mundo a ser gerada à direita
} Game;

Game InitGame(int screenWidth, int screenHeight);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif
