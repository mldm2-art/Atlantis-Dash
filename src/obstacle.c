#include "obstacle.h"
#include "game.h"
#include "textures.h"

#include "raylib.h"
#include <stdlib.h>

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

            float topo = hudAltura;
            float base = screenHeight;   // <<< depois ajusto isso para usar game->blocoTamanho

            // impedir subir demais
            if (atual->y < topo) {
                atual->y = topo;
                atual->direcao = 1;
            }

            // impedir descer demais
            if (atual->y + atual->altura > base) {
                atual->y = base - atual->altura;
                atual->direcao = -1;
            }
        }



        // atualiza hitbox em MUNDO
        atual->hitbox.x = atual->x;
        atual->hitbox.y = atual->y;

        atual = atual->next;
    }
}

// Desenha UM obstáculo usando sua textura correta
// Recebe as texturas vindas do Game (por referência)

static void DrawObstacleSprite(
    Obstacle *o,
    float cameraX,
    ObstacleTextures *tex
) {
    float screenX = o->x - cameraX;
    float screenY = o->y;

    Texture2D sprite;

    switch (o->tipo) {

        
        // FIXOS
       
        case OBSTACULO_PEDRA:
            sprite = tex->pedra;
            break;

        case OBSTACULO_CORAL:
            sprite = tex->coral;
            break;

        case OBSTACULO_CONCHA:
            sprite = tex->concha;
            break;

        case OBSTACULO_ALGA:
            // por enquanto SEM animação, usamos o frame central
            sprite = tex->algaCentro;
            break;

        
        // MÓVEIS (só carangueijo no nível 1)
        
        case OBSTACULO_CARANGUEJO:
            // por enquanto sem animação:
            sprite = tex->carangueijoParado;
            break;

        case OBSTACULO_TUBARAO:
            sprite = tex->tubaCentro;
            break;

        case OBSTACULO_AGUA_VIVA:
            sprite = tex->aguaVivaCentro;
            break;

        case OBSTACULO_BALEIA:
            sprite = tex->baleiaParada;
            break;

    }

    // Proporção do sprite para ocupar o mesmo espaço dos retângulos antigos
    float scaleX = o->largura  / sprite.width;
    float scaleY = o->altura   / sprite.height;

    DrawTextureEx(
        sprite,
        (Vector2){screenX, screenY},
        0.0f,
        (scaleX < scaleY ? scaleX : scaleY), // preserva proporção
        WHITE
    );
}

void DrawObstacles(Obstacle *lista,
                   float cameraX,
                   float hudAltura,
                   int screenWidth,
                   int screenHeight, 
                   ObstacleTextures *texRef) {
    (void)hudAltura;

    Obstacle *atual = lista;
    while (atual != NULL) {
        float screenX = atual->x - cameraX; // scroll
        float screenY = atual->y;

        if (screenX + atual->largura >= -50 && screenX <= screenWidth + 50 &&
            screenY + atual->altura >= hudAltura && screenY <= screenHeight) {

            // Agora desenhamos o sprite correspondente ao tipo do obstáculo
            DrawObstacleSprite(atual, cameraX, texRef);

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