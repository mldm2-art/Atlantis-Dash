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
    float x, y; // posicao real em pixels pq a colisao funciona em pixels
    float largura, altura; // tamanho do sprite (pq a raylib tem que saber p desenhar)
    Rectangle hitbox; // area invísivel,  pd ser do tamanho do srite
    float blocoTamanho; // tamanho do bloco do grid
} Player;

// Lista encadeada de obstáculos
typedef struct ObstaculoNode {
    Obstacle dados;                // dados do obstáculo
    struct ObstaculoNode *prox;    // próximo da lista
} ObstaculoNode;

// ------------------------------
// Estrutura principal do jogo
// ------------------------------
typedef struct {

    // ----------------------
    // Estado atual
    // ----------------------
    GameState estado;

    // ----------------------
    // Janela
    // ----------------------
    int screenWidth;
    int screenHeight;

    // ----------------------
    // HUD
    // ----------------------
    int vidas;
    int pontuacao;
    int moedasColetadas;
    float hudAltura;            // Área superior reservada

    // ----------------------
    // Player
    // ----------------------
    Player player;
    Texture2D texturaPlayer;

    // ----------------------
    // Obstáculos (lista encadeada)
    // ----------------------
    ObstaculoNode *obstaculos;  // Ponteiro para o primeiro obstáculo da lista

    // ----------------------
    // Texturas dos obstáculos
    // ----------------------
    Texture2D texturaPedra;
    Texture2D texturaCoral;
    Texture2D texturaConcha;
    Texture2D texturaAlga;

    // Obstáculos móveis (inimigos)
    Texture2D texturaTubarão;
    Texture2D texturaCaranguejo;
    Texture2D texturaAguaViva;
    Texture2D texturaBaleia;

    // ----------------------
    // Fundo do jogo --- vai mudar pq tem específicas p movel e p fixo
    // ----------------------
    Texture2D backgroundTexture;

    // ----------------------
    // Controle de scroll (tecla D)
    // ----------------------
    float velocidadeScroll;      // Velocidade que o cenário anda
    float cameraX;               // Quanto a tela já andou
    float distanciaPercorrida;   // Distância total percorrida no nível
    float metaDoNivel;           // Distância necessária para completar o nível

    // ----------------------
    // Menu
    // ----------------------
    int menuSelecionado;
    int nivelSelecionado;

    // ----------------------
    // Flags de controle
    // ----------------------
    bool podeMover;              // Player pode dar outro passo?
    float cooldownMovimento;     // Timer entre movimentos
    bool scrollAtivo;            // Se o player está apertando D
    bool paused;                 // Pausado ou não
    bool gameOverFlag;           // Status de game over

} Game;

// Funções principais
Game InitGame(int screenWidth, int screenHeight);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif