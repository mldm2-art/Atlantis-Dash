#include "obstacle.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Inicializa obstáculos com aleatoriedade
void InitObstacles(Obstacle *obstaculos, int colunas, int linhas, int *total) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned int)time(NULL)); seeded = 1; }

    int idx = 0;

    // Começa da coluna 1 (coluna 0 é segura para o spawn do jogador)
    for (int c = 1; c < colunas; c++) {
        if (idx >= MAX_OBSTACLES) break;

        // Chance de não ter obstáculo nessa coluna (para garantir caminhos)
        int roll = rand() % 100;
        if (roll < 35) continue; // ~35% de coluna vazia

        // Sorteia o tipo (ponderado)
        ObstacleType tipo;
        int t = rand() % 100;
        if (t < 40) tipo = IMOVEL;        // 40%
        else if (t < 75) tipo = MOVEL1;   // 35%
        else tipo = MOVEL2;               // 25%

        // Altura em blocos
        int altura = (tipo == MOVEL2 ? 2 : 1);
        int linhaMaxStart = (linhas - altura);
        if (linhaMaxStart < 0) linhaMaxStart = 0;

        // Posição inicial aleatória
        int linha = (linhaMaxStart > 0) ? (rand() % (linhaMaxStart + 1)) : 0;

        // Cria um único obstáculo por coluna
        obstaculos[idx].tipo = tipo;
        obstaculos[idx].coluna = c;
        obstaculos[idx].linha = linha;
        obstaculos[idx].yOffset = 0.0f;
        obstaculos[idx].sentido = 1;

        idx++;
    }

    *total = idx;
}

// Atualiza obstáculos móveis (passam reto, respawn topo)
void UpdateObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura, int linhas) {
    for (int i = 0; i < total; i++) {
        if (obstaculos[i].tipo == IMOVEL) continue;

        float vel = 2.0f;
        obstaculos[i].yOffset += vel * obstaculos[i].sentido;

        float limiteInferior = (linhas - (obstaculos[i].tipo == MOVEL2 ? 2 : 1)) * blocoTamanho;

        if (obstaculos[i].yOffset > limiteInferior) {
            // respawn topo
            obstaculos[i].yOffset = 0;
            obstaculos[i].linha = 0; // sempre inicia no topo
        }
    }
}

// Desenha obstáculos
void DrawObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura) {
    for (int i = 0; i < total; i++) {
        float x = obstaculos[i].coluna * blocoTamanho;
        float y = hudAltura + obstaculos[i].linha * blocoTamanho + obstaculos[i].yOffset;
        float altura = (obstaculos[i].tipo == MOVEL2 ? 2 * blocoTamanho : blocoTamanho);

        Color cor;
        switch (obstaculos[i].tipo) {
            case IMOVEL: cor = RED; break;
            case MOVEL1: cor = ORANGE; break;
            case MOVEL2: cor = MAROON; break;
        }

        DrawRectangle(x, y, blocoTamanho, altura, cor);
    }
}

// Verifica colisão jogador-obstáculo (por grid)
int CheckCollisionPlayerObstacle(void *playerPtr, Obstacle *obstaculos, int total) {
    Player *player = (Player *)playerPtr;

    for (int i = 0; i < total; i++) {
        int obsLinhaStart = obstaculos[i].linha;
        int obsLinhaEnd = obsLinhaStart + (obstaculos[i].tipo == MOVEL2 ? 2 : 1);
        int obsCol = obstaculos[i].coluna;

        if (player->coluna == obsCol &&
            player->linha >= obsLinhaStart &&
            player->linha < obsLinhaEnd) {
            return 1;
        }
    }
    return 0;
}