// ----------------------------------------------------------
// game.h
// ----------------------------------------------------------
// Este arquivo contém as definições essenciais do jogo Atlantis Dash.
// Ele é incluído (com #include "game.h") dentro dos arquivos .c,
// para compartilhar as mesmas estruturas e funções entre eles.
// ----------------------------------------------------------

#ifndef GAME_H     // Evita que o arquivo seja incluído mais de uma vez
#define GAME_H

#include "raylib.h"  // Biblioteca principal usada para gráficos, som e entrada

// ----------------------------------------------------------
// ENUM: GameState
// ----------------------------------------------------------
// Define os possíveis "estados" em que o jogo pode estar.
// Isso é útil para controlar o que será mostrado na tela.
// ----------------------------------------------------------
typedef enum {
    MENU,       // Tela inicial com botões ("Jogar", "Instruções")
    JOGANDO     // Estado principal de gameplay (grid, jogador, etc.)
} GameState;

// ----------------------------------------------------------
// STRUCT: Player
// ----------------------------------------------------------
// Contém todas as informações relacionadas ao jogador.
// Cada campo aqui define parte da posição e dimensão dele.
// ----------------------------------------------------------
typedef struct {
    int linha;           // posição vertical atual (0 a 3)
    int coluna;          // posição horizontal (avança para frente)
    float blocoTamanho;  // tamanho do quadrado (bloco) que ele ocupa
    float areaY;         // posição Y (em pixels) onde começa a área jogável
} Player;

// ----------------------------------------------------------
// STRUCT: Game
// ----------------------------------------------------------
// Estrutura principal do jogo.
// Guarda o estado atual, medidas da tela, grid, HUD, jogador,
// e também o controle do menu inicial.
// ----------------------------------------------------------
typedef struct {
    GameState estado;    // estado atual (MENU, JOGANDO, etc.)

    // 🖥️ Informações da janela
    int screenWidth;     // largura atual da janela
    int screenHeight;    // altura atual da janela

    // 🧩 Estrutura do grid
    int linhas;          // número de linhas (4 fixas)
    int colunas;         // número de colunas calculadas automaticamente
    float hudAltura;     // altura reservada para o HUD no topo
    float blocoTamanho;  // tamanho de cada bloco quadrado do grid

    // 🐟 Jogador
    Player player;       // estrutura Player, com posição e tamanho

    // 🎮 Menu inicial
    int menuSelecionado; // índice da opção selecionada (0 = Jogar, 1 = Instruções)
} Game;

// ----------------------------------------------------------
// Funções principais do jogo (assinaturas)
// ----------------------------------------------------------
// Cada uma dessas funções é implementada em game.c
// ----------------------------------------------------------

// Inicializa todos os valores e estruturas do jogo
Game InitGame(int screenWidth, int screenHeight);

// Atualiza a lógica do jogo a cada frame (movimentos, seleção, etc.)
void UpdateGame(Game *game);

// Desenha o conteúdo atual na tela (menu, grid, jogador, etc.)
void DrawGame(Game *game);

#endif