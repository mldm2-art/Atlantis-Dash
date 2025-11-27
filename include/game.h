#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "obstacle.h"
#include "textures.h"
#include <stdbool.h>

typedef enum {
    MENU,
    INSTRUCOES,
    SELECAO_NIVEL,
    JOGANDO
} GameState;

typedef struct { 
    float x, y;
    float largura, altura; 
    float blocoTamanho; 
    Rectangle hitbox; 
} Player;

typedef struct {
    int pontuacao;
    int moedas;
    int vidas;
} HUD;

typedef struct Moeda {
    float x, y;
    float largura, altura;
    Rectangle hitbox;
    struct Moeda *next;
} Moeda;

typedef struct { 
    GameState estado;

    int screenWidth; 
    int screenHeight;

    int linhas;
    float hudAltura;
    float blocoTamanho;

    // Player
    Player player;
    Texture2D playerTexture;
    Texture2D playerTexture2;
    float playerAnimTimer;
    int playerAnimFrame;

    // Fundos
    Texture2D backgroundTexture; 
    Texture2D seletorNivelBackground;
    ObstacleTextures obstTextures;
    Texture2D bgMar;
    Texture2D bgAreia;

    // --- MUDANÇA AQUI: FINAIS DE FASE ---
    Texture2D finaisFase[4];      // Imagens normais
    Texture2D finaisFaseAnim[4];  // Imagens animadas
    Texture2D nivelConcluidoImg;  // Tela final "Nível Concluído"
    
    bool finalFaseAtivo;          // Se o fundo final já começou a aparecer
    float finalFaseX;             // Posição X do fundo final
    
    float bgAnimTimer;            // Timer para animar o fundo
    int bgAnimFrame;              // Frame atual (0 ou 1)
    
    float levelCompleteTimer;     // <--- NOVO: Conta o tempo antes de mostrar "Nível Concluído"
    bool showLevelComplete;       // Se mostra a imagem final
    // ------------------------------------
    
    // Menus
    int menuSelecionado;
    int nivelSelecionado;

    HUD hud;
    Texture2D hudTexture;

    // Moedas
    int totalMoedasSalvas;
    Texture2D moedaTexture;
    Moeda *moedas;

    // Obstáculos
    Obstacle *obstaculos; 
    
    // Animação obstaculos
    float carangueijoAnimTimer;
    int carangueijoAnimFrame;
    float aguaVivaAnimTimer;
    int aguaVivaAnimFrame;
    float baleiaAnimTimer;
    int baleiaAnimFrame;
    float tubaraoAnimTimer;
    int tubaraoAnimFrame;

    // Câmera
    float cameraX;
    float cameraDestinoX;
    bool cameraMovendo;
    float cameraVelocidade;
    float tempoParado;

    Texture2D gameOverTexture;
    bool showGameOver;
    bool waitingForContinue;

    int numColunasVisiveis;
    float colunaLargura;

    int worldColumns;
    int primeiraColuna;
    int proximaColuna; 
    
    Music musica;
} Game;

Game InitGame(int screenWidth, int screenHeight);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);
void GenerateWorldForLevel(Game *game);

void SalvarBanco(int total);
int CarregarBanco();

#endif