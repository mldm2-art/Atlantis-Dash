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
    o->next = NULL;

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
    while (atual->next != NULL) {
        atual = atual->next;
    }
    atual->next = novo;
}

void DestroyObstacleList(Obstacle **lista) {
    Obstacle *atual = *lista;
    while (atual != NULL) {
        Obstacle *prox = atual->next;
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
                *lista = atual->next;
                atual = *lista;
            } else {
                anterior->next = atual->next;
                atual = anterior->next;
            }
            free(rem);
        } else {
            anterior = atual;
            atual = atual->next;
        }
    }
}

void UpdateObstacles(Obstacle *lista, float deltaTime, float hudAltura, float screenHeight) {
    Obstacle *atual = lista;
    while (atual != NULL) {
        if (atual->velocidade > 0.0f) {   // móveis se mexem na VERTICAL
            atual->y += atual->direcao * atual->velocidade * deltaTime;

            if (atual->velocidade > 0.0f) {   // móveis se mexem na VERTICAL
                atual->y += atual->direcao * atual->velocidade * deltaTime;

                // limites da área jogável
                float topo = hudAltura;
                float base = hudAltura + 6 * 96.0f;   // 6 linhas de 96px cada

                // impedir subir para dentro do HUD
                if (atual->y < topo) {
                    atual->y = topo;
                    atual->direcao = 1;  // desce
                }

                // impedir passar do final da área de jogo
                if (atual->y + atual->altura > base) {
                    atual->y = base - atual->altura;
                    atual->direcao = -1;  // sobe
                }
            }
        }

        // atualiza hitbox em MUNDO
        atual->hitbox.x = atual->x;
        atual->hitbox.y = atual->y;

        atual = atual->next;
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

        atual = atual->next;
    }
}

Obstacle *CheckCollisionPlayerObstacles(Rectangle playerHitbox, Obstacle *lista) {
    Obstacle *atual = lista;
    while (atual != NULL) {
        if (CheckCollisionRecs(playerHitbox, atual->hitbox)) {
            return atual;  // retorna o obstáculo que bateu
        }
        atual = atual->next;
    }
    return NULL;  // sem colisão
}