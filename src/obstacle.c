#include "obstacle.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>

static float RandomFloatRange(float min, float max) {
    return min + ((float)rand() / (float)RAND_MAX) * (max - min);
}

void InitObstacles(Obstacle *obstaculos, ColumnType *columnTypes, int worldColumns, int linhas, int *total) {
    static int seeded = 0;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = 1;
    }

    *total = 0;
    const int safeStartColumns = 5;

    for (int col = 0; col < worldColumns && *total < MAX_OBSTACLES; col++) {
        if (columnTypes[col] != COLUMN_SAND) continue;
        if (col < safeStartColumns) continue;

        int movingCount = 1 + rand() % 2; // 1-2 peixes móveis
        int staticCount = 1 + rand() % 2; // 1-2 corais fixos

        for (int m = 0; m < movingCount && *total < MAX_OBSTACLES; m++) {
            Obstacle *obs = &obstaculos[(*total)++];
            obs->coluna = col;
            obs->tipo = (rand() % 100 < 40) ? MOVEL2 : MOVEL1;
            obs->altura = (obs->tipo == MOVEL2) ? 1.8f : 1.2f;

            float maxInicio = (float)linhas - obs->altura;
            if (maxInicio < 0.0f) maxInicio = 0.0f;

            obs->linha = RandomFloatRange(0.0f, maxInicio);
            obs->velocidade = RandomFloatRange(1.2f, 2.6f);
            obs->sentido = (rand() % 2 == 0) ? 1 : -1;
        }

        for (int s = 0; s < staticCount && *total < MAX_OBSTACLES; s++) {
            Obstacle *obs = &obstaculos[(*total)++];
            obs->coluna = col;
            obs->tipo = IMOVEL;
            obs->altura = (rand() % 100 < 50) ? 1.0f : 1.5f;

            float maxInicio = (float)linhas - obs->altura;
            if (maxInicio < 0.0f) maxInicio = 0.0f;

            obs->linha = RandomFloatRange(0.0f, maxInicio);
            obs->velocidade = 0.0f;
            obs->sentido = 0;
        }
    }
}

void UpdateObstacles(Obstacle *obstaculos, int total, int linhas) {
    float dt = GetFrameTime();

    for (int i = 0; i < total; i++) {
        Obstacle *obs = &obstaculos[i];
        if (obs->tipo == IMOVEL) continue;

        obs->linha += obs->velocidade * obs->sentido * dt;

        float maxLinha = (float)linhas - obs->altura;
        if (maxLinha < 0.0f) maxLinha = 0.0f;

        if (obs->linha < 0.0f) {
            obs->linha = 0.0f;
            obs->sentido *= -1;
        } else if (obs->linha > maxLinha) {
            obs->linha = maxLinha;
            obs->sentido *= -1;
        }
    }
}

void DrawObstacles(Obstacle *obstaculos, int total, float blocoTamanho, float hudAltura, float cameraColumn, int screenWidth) {
    Color fishSmall = (Color){255, 196, 0, 255};
    Color fishBig   = (Color){255, 120, 40, 255};
    Color coralColor= (Color){220, 80, 120, 255};

    for (int i = 0; i < total; i++) {
        Obstacle *obs = &obstaculos[i];

        float screenX = (obs->coluna - cameraColumn) * blocoTamanho + blocoTamanho * 0.1f;
        float width = blocoTamanho * 0.8f;

        if (screenX + width < -blocoTamanho || screenX > screenWidth + blocoTamanho) continue;

        float screenY = hudAltura + obs->linha * blocoTamanho;
        float height = obs->altura * blocoTamanho;

        Color drawColor;
        if (obs->tipo == IMOVEL)      drawColor = coralColor;
        else if (obs->tipo == MOVEL2) drawColor = fishBig;
        else                          drawColor = fishSmall;

        DrawRectangleRounded((Rectangle){screenX, screenY, width, height}, 0.35f, 6, drawColor);
    }
}

int CheckCollisionPlayerObstacle(void *playerPtr, Obstacle *obstaculos, int total) {
    Player *player = (Player *)playerPtr;

    float pLeft   = player->coluna + 0.2f;
    float pRight  = pLeft + 0.6f;
    float pTop    = (float)player->linha + 0.1f;
    float pBottom = pTop + 0.8f;

    for (int i = 0; i < total; i++) {
        Obstacle *obs = &obstaculos[i];

        float oLeft   = (float)obs->coluna + 0.1f;
        float oRight  = oLeft + 0.8f;
        float oTop    = obs->linha;
        float oBottom = oTop + obs->altura;

        if (pRight > oLeft && pLeft < oRight && pBottom > oTop && pTop < oBottom) {
            return 1;
        }
    }
    return 0;
}