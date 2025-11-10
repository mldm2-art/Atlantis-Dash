#include "obstacle.h"
#include "game.h"
#include <stdio.h>

// Inicializa obstáculos em colunas fixas sem aleatoriedade
void InitObstacles(Obstacle *obstaculos, int colunas, int linhas, int *total) {
    int idx = 0;

    for (int c = 0; c < colunas; c++) {
        if (c == 0) continue; // primeira coluna segura

        ObstacleType tipo;
        switch (c % 3) {
            case 1: tipo = IMOVEL; break;
            case 2: tipo = MOVEL2; break;
            default: tipo = MOVEL1; break;
        }

        int qtd = (tipo == MOVEL2) ? 2 : 1;
        for (int i = 0; i < qtd; i++) {
            obstaculos[idx].tipo = tipo;
            obstaculos[idx].coluna = c;
            obstaculos[idx].linha = i; // posição inicial pré-definida
            obstaculos[idx].yOffset = 0;
            obstaculos[idx].sentido = 1;
            idx++;
        }
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

// Verifica colisão jogador-obstáculo
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
