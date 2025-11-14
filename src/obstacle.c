#include "obstacle.h"
#include <stdlib.h>

// Cores temporárias para cada tipo (até ter sprites)
static Color GetObstacleColor(ObstaculoTipo tipo) {
    switch (tipo) {
        // Fixos (areia)
        case OBSTACULO_PEDRA:      return GRAY;
        case OBSTACULO_CORAL:      return ORANGE;
        case OBSTACULO_CONCHA:     return BEIGE;
        case OBSTACULO_ALGA:       return GREEN;

        // Móveis (mar)
        case OBSTACULO_TUBARAO:    return RED;
        case OBSTACULO_CARANGUEJO: return MAROON;
        case OBSTACULO_AGUA_VIVA:  return PURPLE;
        case OBSTACULO_BALEIA:     return BLUE;

        default:                   return WHITE;
    }
}

Obstacle *CreateObstacle(ObstaculoTipo tipo,
                         float x, float y,
                         float largura, float altura,
                         float velocidade, int direcao) {
    Obstacle *o = (Obstacle *)malloc(sizeof(Obstacle));
    if (!o) return NULL;

    o->tipo = tipo;
    o->x = x;
    o->y = y;
    o->largura = largura;
    o->altura = altura;
    o->velocidade = velocidade;
    o->direcao = direcao;
    o->prox = NULL;

    o->hitbox = (Rectangle){ x, y, largura, altura };

    return o;
}

void AddObstacle(Obstacle **lista, Obstacle *novo) {
    if (!novo) return;
    if (*lista == NULL) {
        *lista = novo;
        return;
    }
    Obstacle *atual = *lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = novo;
}

void DestroyObstacleList(Obstacle **lista) {
    Obstacle *atual = *lista;
    while (atual != NULL) {
        Obstacle *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    *lista = NULL;
}

void RemoveObstaclesLeftOf(Obstacle **lista, float cameraX) {
    if (!lista || !*lista) return;

    Obstacle *atual = *lista;
    Obstacle *anterior = NULL;

    while (atual != NULL) {
        // se o obstáculo está totalmente à esquerda da câmera (bem fora da tela)
        if (atual->x + atual->largura < cameraX - 5.0f) {
            Obstacle *rem = atual;
            if (anterior == NULL) {
                *lista = atual->prox;
                atual = *lista;
            } else {
                anterior->prox = atual->prox;
                atual = anterior->prox;
            }
            free(rem);
        } else {
            anterior = atual;
            atual = atual->prox;
        }
    }
}

void UpdateObstacles(Obstacle *lista, float deltaTime,
                     float hudAltura, float screenHeight) {
    Obstacle *atual = lista;
    while (atual != NULL) {
        if (atual->velocidade > 0.0f) {   // móveis se mexem na VERTICAL
            atual->y += atual->direcao * atual->velocidade * deltaTime;

            // wrap vertical: se sair por baixo, volta por cima; se sair por cima, volta por baixo
            if (atual->y > screenHeight) {
                atual->y = hudAltura - atual->altura;
            } else if (atual->y + atual->altura < hudAltura) {
                atual->y = screenHeight;
            }
        }

        // atualiza hitbox em MUNDO
        atual->hitbox.x = atual->x;
        atual->hitbox.y = atual->y;

        atual = atual->prox;
    }
}

void DrawObstacles(Obstacle *lista,
                   float cameraX,
                   float hudAltura,
                   int screenWidth,
                   int screenHeight) {
    (void)hudAltura;

    Obstacle *atual = lista;
    while (atual != NULL) {
        float screenX = atual->x - cameraX; // scroll
        float screenY = atual->y;

        if (screenX + atual->largura >= -50 && screenX <= screenWidth + 50 &&
            screenY + atual->altura >= hudAltura && screenY <= screenHeight) {

            Color c = GetObstacleColor(atual->tipo);
            DrawRectangle((int)screenX,
                          (int)screenY,
                          (int)atual->largura,
                          (int)atual->altura,
                          c);
        }

        atual = atual->prox;
    }
}

bool CheckCollisionPlayerObstacles(Rectangle playerHitbox, Obstacle *lista) {
    Obstacle *atual = lista;
    while (atual != NULL) {
        if (CheckCollisionRecs(playerHitbox, atual->hitbox)) {
            return true;
        }
        atual = atual->prox;
    }
    return false;
}
