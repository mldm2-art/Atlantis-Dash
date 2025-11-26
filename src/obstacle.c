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

    // ----------------------
    // HITBOX PERSONALIZADA POR TIPO
    // ----------------------

    float hbX, hbY, hbW, hbH;

    // ================= FIXOS =================

    // Concha (pequena, bem central)
    if (tipo == OBSTACULO_CONCHA) {
        hbX = x + largura * 0.30f;
        hbY = y + altura  * 0.30f;
        hbW = largura * 0.40f;
        hbH = altura  * 0.40f;
    }

    // Pedra
    else if (tipo == OBSTACULO_PEDRA) {
        hbX = x + largura * 0.25f;
        hbY = y + altura  * 0.25f;
        hbW = largura * 0.50f;
        hbH = altura  * 0.50f;
    }

    // Coral
    else if (tipo == OBSTACULO_CORAL) {
        hbX = x + largura * 0.20f;
        hbY = y + altura  * 0.20f;
        hbW = largura * 0.60f;
        hbH = altura  * 0.60f;
    }

    // Alga (colisão mais no meio)
    else if (tipo == OBSTACULO_ALGA) {
        hbX = x + largura * 0.35f;
        hbY = y + altura  * 0.10f;
        hbW = largura * 0.30f;
        hbH = altura  * 0.80f;
    }


// ================= MÓVEIS =================


    // 🦀 Caranguejo → corpo, não pernas
    else if (tipo == OBSTACULO_CARANGUEJO) {
        hbX = x + largura * 0.35f;
        hbY = y + altura  * 0.25f;
        hbW = largura * 0.30f;
        hbH = altura  * 0.45f;
    }

    // 🪼 Água-viva → só a "cabeça"
    else if (tipo == OBSTACULO_AGUA_VIVA) {
        hbX = x + largura * 0.25f;
        hbY = y + altura  * 0.05f;
        hbW = largura * 0.50f;
        hbH = altura  * 0.40f;
    }



    // 🐳 Baleia → só região central/traseira (corpo)
    else if (tipo == OBSTACULO_BALEIA) {
        hbX = x + largura * 0.45f;     // ignora a frente inteira
        hbY = y + altura * 0.30f;
        hbW = largura * 0.35f;
        hbH = altura  * 0.40f;
    }



    // 🦈 Tubarão → região da boca / centro
    else if (tipo == OBSTACULO_TUBARAO) {
        hbX = x + largura * 0.32f;
        hbY = y + altura  * 0.25f;
        hbW = largura * 0.42f;
        hbH = altura  * 0.45f;
    }


    o->hitbox = (Rectangle){
        hbX,
        hbY,
        hbW,
        hbH
    };

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
            atual->y += atual->velocidade * deltaTime;

            float limiteTopo = hudAltura;
            float limiteBase = screenHeight;   // <<< depois ajusto isso para usar game->blocoTamanho

            if (atual->y > limiteBase) {
                atual->y = limiteTopo + 5;   // entra logo abaixo do HUD
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
    ObstacleTextures *tex,
    int carangueijoFrame,
    int aguaVivaFrame,
    int baleiaFrame,
    int tubaraoFrame     
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
            if (carangueijoFrame == 0)
                sprite = tex->carangueijoParado;
            else
                sprite = tex->carangueijoAnim;
            break;


        case OBSTACULO_TUBARAO:
            if (tubaraoFrame == 0)
                sprite = tex->tubaCentro;      // centro
            else if (tubaraoFrame == 1)
                sprite = tex->tubaDir;         // direita
            else if (tubaraoFrame == 2)
                sprite = tex->tubaCentro;      // centro
            else
                sprite = tex->tubaEsq;         // esquerda
            break;


        case OBSTACULO_AGUA_VIVA:
            if (aguaVivaFrame == 0)
                sprite = tex->aguaVivaCentro;
            else if (aguaVivaFrame == 1)
                sprite = tex->aguaVivaDir;
            else if (aguaVivaFrame == 2)
                sprite = tex->aguaVivaCentro;
            else
                sprite = tex->aguaVivaEsq;
            break;


        case OBSTACULO_BALEIA:
            if (baleiaFrame == 0)
                sprite = tex->baleiaParada;
            else
                sprite = tex->baleiaAnimada;
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
                   ObstacleTextures *texRef,
                   int carangueijoFrame,
                   int aguaVivaFrame, 
                   int baleiaFrame, 
                   int tubaraoFrame) {
    (void)hudAltura;

    Obstacle *atual = lista;
    while (atual != NULL) {
        float screenX = atual->x - cameraX; // scroll
        float screenY = atual->y;

        if (screenX + atual->largura >= -50 && screenX <= screenWidth + 50 &&
            screenY + atual->altura >= hudAltura && screenY <= screenHeight) {

            // Agora desenhamos o sprite correspondente ao tipo do obstáculo
            DrawObstacleSprite(atual, cameraX, texRef, carangueijoFrame, aguaVivaFrame, baleiaFrame, tubaraoFrame);

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