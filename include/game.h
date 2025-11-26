#ifndef GAME_H // esses dois '#' impedem que o arquivo seja incluido duas vezes (include guard)
#define GAME_H

#include "raylib.h" // bilibioteca gráfica que possui diversas funções próprias como a Texture, a Draw
#include "obstacle.h" // Estruturas e funções de obstáculos (lista encadeada)
#include "textures.h"
#include <stdbool.h> // para usar bool no struct Game

// Estados do jogo (telas):
typedef enum {
    MENU,
    INSTRUCOES,
    SELECAO_NIVEL,
    JOGANDO
} GameState;

// Estrutura do jogador (peixe):
typedef struct { // struct 'Player' que traz as características do nosso personagem (peixe)
    float x, y; // posição na TELA (x fixo, y em blocos) obs:podemos mudar para y em pixels
    float largura, altura; // dimensões do sprite do peixe
    float blocoTamanho; // distancia entre as linhas (Cada W/S move exatamente 1 bloco)
    Rectangle hitbox; // define a hitbox do peixe hitbox.x = player.x (sempre será a mesma, pois quem anda é a tela) + camera.X (muda a cada vez que  'D' é apertado)
                     // por isso que o hitbox do peixe é essa soma, pois vai saber a posição real do peixe no MUNDO, e não na tela em si (já que a colisão ocorre no MUNDO)
} Player;

typedef struct {
    int vidas;
    int pontuacao;
    int moedas;
} HUD;

// Estrutura principal do jogo:
typedef struct { // struct 'Game' 
    GameState estado; // criação de uma variável 'estado' do tipo 'GameState' para sabermos em que tel estamos no momento

    // Dimensões da tela do raylib:
    int screenWidth; 
    int screenHeight;

    // Grid vertical (linhas horizontais)
    int linhas; // quantidade de linhas horizontais que ele usa no tela obs:podemos aumentar a quantidade de linhas 
    float hudAltura; // tamanho da barra superior de HUD
    float blocoTamanho; // distancia entre as linhas da area jogavel (memsa coisa que tem no struct 'Player')

    // Player:
    Player player; // criação de uma variável chamada 'player' do tipo 'Player'
    Texture2D playerTexture; // sprite do peixe vem aqui 
    Texture2D playerTexture2;   // segundo sprite do peixe
    float playerAnimTimer;      // contador interno
    int playerAnimFrame;        // 0 ou 1 para alternar sprites


    // Fundos:
    Texture2D backgroundTexture; // a gente tem uma imagem salva chamada 'menu_jogo.png em 'assets/imgs' 
    Texture2D seletorNivelBackground; // a gente tem uma imagem salva chamada 'menu_jogo.png em 'assets/imgs'
    // Texturas agrupadas de obstáculos
    ObstacleTextures obstTextures;
    Texture2D bgMar;
    Texture2D bgAreia;

    // Sprite exibida quando o personagem perde uma vida
    Texture2D gameOverTexture;  // sprite "game_over" (assets/imgs/game_over.png)
    bool showGameOver;          // true enquanto a sprite estiver sendo mostrada
    bool waitingForContinue;    // true se estamos aguardando o jogador apertar ESC para continuar

    // Menus:
    int menuSelecionado; // índice dos botões da tela de MENU (JOGAR e INTRUÇÕES) que é 0 ou 1
    int nivelSelecionado; // índice dos botões da tela de SELEÇÃO DE NÍVEL (NÍVEL 1, NÍVEL 2, NÍVEL 3 e NÍVEL 4) que é de 0 a 3

    HUD hud; // criação da variável 'hud' do tipo 'HUD'
    Texture2D hudTexture;


    // Obstáculos (lista encadeada):
    Obstacle *obstaculos; // ponteiro 'obstaculos' do tipo 'Obstacle' para o primeiro nó da lista encadeada de obstáculos (é como se fosse o ponteiro 'head')
    
    //animacao obstaculos
    float carangueijoAnimTimer;
    int carangueijoAnimFrame;
    // Mundo em colunas + câmera:
    float cameraX;             // deslocamento horizontal da câmera em pixels (quanto mundo já “rolou” para a esquerda)
    float cameraDestinoX;   // posição atual é cameraX e a posição de destino(cameraDestinoX) é: cameraX + larguraDeUmBloco. -> Depois disso a câmera vai deslizando até chegar nesse destino.
    bool cameraMovendo; // a camera ta andando agr? true: ta se movendo, flase: ta parada (impede que o jogador aperte d varias vezes enquanto a cameta ta andando)
    float cameraVelocidade; // pixels por frame, velocidade da camera
    float tempoParado;


    int numColunasVisiveis;    // quantidade de colunas que vemos por tela (7 por tela) obs:podemos aumentar isso também
    float colunaLargura;       // largura de cada coluna em pixels (colunaLargura = screenWidth / numColunasVisiveis)

    int worldColumns;          // total de colunas no nível (MUNDO) obs:podemos ajustar isso também
    int primeiraColuna;        // índice da coluna mais à esquerda do mundo (que será destruída ao apertar 'D')
    int proximaColuna; 
    

    // próxima coluna do mundo a ser gerada à direita (que será criada ao apertar 'D')

    Music musica;
} Game;

Game InitGame(int screenWidth, int screenHeight); // função da raylib que monta toda a estrutura para iniciar o jogo
void UpdateGame(Game *game); // função para atualizar o jogo (usa como paramêtro um ponteiro 'game' para struct 'Game')
void DrawGame(Game *game); // função para desenhar o jogo (usa como paramêtro um ponteiro 'game' para struct 'Game')
void UnloadGame(Game *game); // função que libera memória (texturas e listas encadeadas) (usa como paramêtro um ponteiro 'game' para struct 'Game')
void GenerateWorldForLevel(Game *game);

#endif