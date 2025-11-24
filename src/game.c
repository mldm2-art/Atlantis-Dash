#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Funções internas (estáticas)
static void ResetPlayer(Game *game);
static void UpdatePlayer(Game *game);
void GenerateWorldForLevel(Game *game);
static void SpawnColumn(Game *game, int worldColumnIndex);

// ------------------------
// Inicialização do jogo
// ------------------------
Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    srand((unsigned int)time(NULL));

    game.screenWidth = screenWidth;
    game.screenHeight = screenHeight;
    game.estado = MENU;

    // Grid vertical: 6 linhas
    game.linhas = 6;
    game.hudAltura = 96;
    game.blocoTamanho = (screenHeight - game.hudAltura) / game.linhas;

    // Mundo em colunas
    game.numColunasVisiveis = 9;                    // 14 colunas visíveis
    game.colunaLargura = (float)screenWidth / game.numColunasVisiveis;
    game.worldColumns = 40;                          // nível com 40 colunas de mundo

    // camera começa parada 
    game.cameraX = 0.0f;
    game.cameraMovendo = false;
    game.cameraDestinoX = game.cameraX;
    game.cameraVelocidade = 384.0f;                  // pixels por segundo

    // Player
    game.playerTexture = LoadTexture("assets/imgs/personagemsprite.png");
    game.player.blocoTamanho = game.blocoTamanho;

    // tamanho real do sprite após scale
    float scalePeixe = (game.blocoTamanho * 0.6f) / game.playerTexture.height;
    game.player.largura = game.playerTexture.width * scalePeixe;
    game.player.altura  = game.playerTexture.height * scalePeixe;

    // Texturas
    
    game.backgroundTexture = LoadTexture("assets/imgs/menu_jogo.png");
    game.seletorNivelBackground = game.backgroundTexture; // por enquanto igual

    
    //TEXTURAS DE OBSTÁCULOS
        // Fixos
    game.obstTextures.pedra = LoadTexture("assets/imgs/pedrapronta.png");
    game.obstTextures.coral = LoadTexture("assets/imgs/coralpronto.png");
    game.obstTextures.concha = LoadTexture("assets/imgs/conchapronta.png");

        // Alga animada
    game.obstTextures.algaCentro = LoadTexture("assets/imgs/algapronta.png");
    game.obstTextures.algaEsq    = LoadTexture("assets/imgs/algaesquerdapronta.png");
    game.obstTextures.algaDir    = LoadTexture("assets/imgs/algadireitapronta.png");

        // Carangueijo (nível 1)
    game.obstTextures.carangueijoParado = LoadTexture("assets/imgs/carangueijopronto.png");
    game.obstTextures.carangueijoAnim   = LoadTexture("assets/imgs/carangueijoanimacaopronto.png");

        // Água-viva
    game.obstTextures.aguaVivaCentro = LoadTexture("assets/imgs/aguavivaapronta.png");
    game.obstTextures.aguaVivaEsq    = LoadTexture("assets/imgs/aguavivaesquerdapronta.png");
    game.obstTextures.aguaVivaDir    = LoadTexture("assets/imgs/aguavivadireitapronta.png");

        // Baleia
    game.obstTextures.baleiaParada  = LoadTexture("assets/imgs/baleiapronta.png");
    game.obstTextures.baleiaAnimada = LoadTexture("assets/imgs/baleiaanimadapronta.png");

        // Tubarão
    game.obstTextures.tubaCentro = LoadTexture("assets/imgs/tubaraopronto.png");
    game.obstTextures.tubaEsq    = LoadTexture("assets/imgs/tubaraoesquerda.png");
    game.obstTextures.tubaDir    = LoadTexture("assets/imgs/tubaraodireita.png");


    // Menus / HUD
    game.menuSelecionado = 0;
    game.nivelSelecionado = 0;
    game.hud.vidas = 3;
    game.hud.pontuacao = 0;
    game.hud.moedas = 0;

    // Obstáculos
    game.obstaculos = NULL;
    game.primeiraColuna = 0;
    game.proximaColuna = 0;

    ResetPlayer(&game);

    return game;
}

// ------------------------
// Atualização
// ------------------------
void UpdateGame(Game *game) {
    // (por enquanto ignorando resize pra não complicar colunas)
    if (game->estado == MENU) {
        if (IsKeyPressed(KEY_W) && game->menuSelecionado > 0) game->menuSelecionado--;
        if (IsKeyPressed(KEY_S) && game->menuSelecionado < 1) game->menuSelecionado++;
        if (IsKeyPressed(KEY_ENTER)) {
            if (game->menuSelecionado == 0) game->estado = SELECAO_NIVEL;
            else if (game->menuSelecionado == 1) game->estado = INSTRUCOES;
        }
    }
    else if (game->estado == INSTRUCOES) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = MENU;
        }
    }
    else if (game->estado == SELECAO_NIVEL) {
        if (IsKeyPressed(KEY_W) && game->nivelSelecionado > 0) game->nivelSelecionado--;
        if (IsKeyPressed(KEY_S) && game->nivelSelecionado < 3) game->nivelSelecionado++;

        if (IsKeyPressed(KEY_ENTER)) {
            game->estado = JOGANDO;
            game->hud.vidas = 3;

            GenerateWorldForLevel(game);
            ResetPlayer(game);
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = MENU;
        }
    }
    else if (game->estado == JOGANDO) {
        float delta = GetFrameTime();

        // MOVIMENTO VERTICAL
        UpdatePlayer(game);

        // ------ MOVIMENTO HORIZONTAL EM BLOCOS (D) ------
        if (IsKeyPressed(KEY_D) && !game->cameraMovendo) {

            float mundoX = game->player.x + game->cameraX;

            Rectangle atual = {
                mundoX + game->player.largura * 0.1f,
                game->player.y + game->player.altura * 0.1f,
                game->player.largura * 0.8f,
                game->player.altura * 0.8f
            };

            Rectangle futuro = atual;
            futuro.x += game->colunaLargura - game->player.largura * 0.2f;

            Obstacle *hit = CheckCollisionPlayerObstacles(futuro, game->obstaculos);

            if (hit && hit->velocidade == 0)
                return;

            if (hit && hit->velocidade > 0) {
                GenerateWorldForLevel(game);
                ResetPlayer(game);
                return;
            }

            int colunaAtual = (int)floorf((game->cameraX + 1.0f) / game->colunaLargura);
            int proximaColuna = colunaAtual + 1;

            if (proximaColuna >= game->worldColumns)
            proximaColuna = game->worldColumns - 1;


            game->cameraDestinoX = proximaColuna * game->colunaLargura;
            game->cameraMovendo = true;
        }

        // MOVIMENTO SUAVE DA CÂMERA
        if (game->cameraMovendo) {
            game->cameraX += game->cameraVelocidade * delta;

            if (game->cameraX >= game->cameraDestinoX - 0.5f) {
                game->cameraX = game->cameraDestinoX;
                game->cameraMovendo = false;
            }
        }

        // GERAR / REMOVER COLUNAS
        float limiteRemocao = (game->primeiraColuna + 1) * game->colunaLargura;

        while (game->cameraX >= limiteRemocao) {

            RemoveObstaclesLeftOf(&game->obstaculos, game->cameraX);

            game->primeiraColuna++;

            if (game->proximaColuna < game->worldColumns) {
                SpawnColumn(game, game->proximaColuna);
                game->proximaColuna++;
            }

            limiteRemocao = (game->primeiraColuna + 1) * game->colunaLargura;
        }

        // MOVIMENTO DOS OBSTÁCULOS
        UpdateObstacles(game->obstaculos, delta, game->hudAltura, (float)game->screenHeight);

        // ATUALIZA HITBOX DO PLAYER — agora usando o TAMANHO REAL DO SPRITE
        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;

        float realW = game->playerTexture.width * scalePeixe;
        float realH = game->playerTexture.height * scalePeixe;

        float mundoX = game->player.x + game->cameraX;

        // hitbox ligeiramente menor para evitar colisão fantasma
        game->player.hitbox.x = mundoX + realW * 0.05f;
        game->player.hitbox.y = game->player.y + realH * 0.05f;

        game->player.hitbox.width  = realW * 0.90f;
        game->player.hitbox.height = realH * 0.90f;

        // ESC -> voltar nível
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = SELECAO_NIVEL;
            DestroyObstacleList(&game->obstaculos);
            ResetPlayer(game);
        }

        // SNAPPING — SEMPRE alinhado à grade
        if (!game->cameraMovendo) {
            float snapped = roundf(game->cameraX / game->colunaLargura) * game->colunaLargura;
            game->cameraX = snapped;
            game->cameraDestinoX = snapped;
        }
    }
}

// ------------------------
// Desenho
// ------------------------
void DrawGame(Game *game) {
    BeginDrawing();

    if (game->estado == MENU) {
        DrawTexturePro(
            game->backgroundTexture,
            (Rectangle){0, 0, game->backgroundTexture.width, game->backgroundTexture.height},
            (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        const char *titulo = "ATLANTIS-DASH";
        DrawText(titulo, game->screenWidth / 2 - MeasureText(titulo, 60) / 2, 120, 60, SKYBLUE);

        float brilho = (sin(GetTime() * 2) + 1) * 0.5f;
        DrawText(titulo, game->screenWidth / 2 - MeasureText(titulo, 60) / 2, 120, 60, Fade(WHITE, 0.4f + 0.4f * brilho));

        const char *opcoes[2] = {"Jogar", "Instrucoes do jogo"};
        int tamanhoFonte = 30;
        int espacamento = 70;
        int baseY = game->screenHeight / 2 - espacamento / 2;

        for (int i = 0; i < 2; i++) {
            int textWidth = MeasureText(opcoes[i], tamanhoFonte);
            int posX = game->screenWidth / 2 - textWidth / 2;
            int posY = baseY + i * espacamento;

            if (i == game->menuSelecionado) {
                float alpha = (sin(GetTime() * 4) + 1) / 2;
                Color glow = Fade(YELLOW, 0.5f + 0.5f * alpha);
                DrawRectangle(posX - 20, posY - 10, textWidth + 40, 50, Fade(BLUE, 0.3f));
                DrawText(opcoes[i], posX, posY, tamanhoFonte, glow);
            } else {
                DrawText(opcoes[i], posX, posY, tamanhoFonte, LIGHTGRAY);
            }
        }

        const char *msg = "Aperte ENTER para selecionar";
        float alphaMsg = (sin(GetTime() * 3) + 1) / 2;
        DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, game->screenHeight - 80, 20, Fade(RAYWHITE, 0.6f + 0.4f * alphaMsg));
    }
    else if (game->estado == INSTRUCOES) {
        // Seu texto de instruções antigo
        ClearBackground((Color){5, 25, 45, 255});

        int w = game->screenWidth;
        int h = game->screenHeight;

        const char *titulo = "INSTRUCOES DO JOGO";
        DrawText(titulo,
                 w / 2 - MeasureText(titulo, h * 0.05f) / 2,
                 (int)(h * 0.08f), (int)(h * 0.05f), SKYBLUE);

        int fontSize = (int)(h * 0.01f);
        int posY = (int)(h * 0.18f);

        const char *intro[] = {
            "Bem-vindo as profundezas do Atlantis-Dash!",
            "Nade entre corredores de areia e agua em Atlantida.",
            "Desvie de animais perigosos e corais enquanto avanca.",
            "Use seus reflexos para chegar o mais longe possivel!"
        };

        for (int i = 0; i < 4; i++) {
            DrawText(intro[i],
                     w / 2 - MeasureText(intro[i], fontSize) / 2,
                     posY + i * (fontSize + 6),
                     fontSize, RAYWHITE);
        }

        int leftX = (int)(w * 0.04f);
        int rightX = (int)(w * 0.52f);

        int baseY = (int)(h * 0.42f);
        int line = fontSize + 6;

        DrawText("OBJETIVO PRINCIPAL:", leftX, baseY, fontSize + 4, YELLOW);
        DrawText("Chegar vivo ao final de cada um dos quatro niveis, desviando de obstaculos",
                 leftX, baseY + line, fontSize, RAYWHITE);
        DrawText("e inimigos enquanto coleta o maior numero possivel de moedas.",
                 leftX, baseY + 2 * line, fontSize, RAYWHITE);
        DrawText("OBS: O proximo nivel so sera liberado quando o nivel anterior for concluido.",
                 leftX, baseY + 3 * line, fontSize, RAYWHITE);

        int controlsY = baseY + 5 * line;
        DrawText("CONTROLES:", leftX, controlsY, fontSize + 4, YELLOW);
        DrawText("Nas telas de menu:", leftX, controlsY + line, fontSize, LIGHTGRAY);
        DrawText("W - Ir para cima", leftX + 20, controlsY + 2 * line, fontSize, RAYWHITE);
        DrawText("S - Ir para baixo", leftX + 20, controlsY + 3 * line, fontSize, RAYWHITE);
        DrawText("ENTER - Selecionar", leftX + 20, controlsY + 4 * line, fontSize, RAYWHITE);
        DrawText("ESC - Voltar", leftX + 20, controlsY + 5 * line, fontSize, RAYWHITE);

        DrawText("Durante o jogo:", leftX, controlsY + 7 * line, fontSize, LIGHTGRAY);
        DrawText("W - Nadar para cima", leftX + 20, controlsY + 8 * line, fontSize, RAYWHITE);
        DrawText("S - Nadar para baixo", leftX + 20, controlsY + 9 * line, fontSize, RAYWHITE);
        DrawText("D - Avancar (a camera acompanha o peixe)", leftX + 20, controlsY + 10 * line, fontSize, RAYWHITE);
        DrawText("ESC - Sair do nivel", leftX + 20, controlsY + 11 * line, fontSize, RAYWHITE);

        int infoY = baseY;
        DrawText("SISTEMA DE VIDAS:", rightX, infoY, fontSize + 4, YELLOW);
        DrawText("Voce comeca com 3 vidas. Colisoes removem 1 vida.",
                 rightX, infoY + line, fontSize, RAYWHITE);
        DrawText("Ao perder todas, voce retorna ao inicio do nivel.",
                 rightX, infoY + 2 * line, fontSize, RAYWHITE);

        int ObstacleY = baseY + 5 * line;
        DrawText("OBSTACULOS:", rightX, ObstacleY, fontSize + 4, YELLOW);
        DrawText("No Atlantis-Dash existem dois tipos de obstaculos: os fixos e os moveis.",
                 rightX, ObstacleY + line, fontSize, RAYWHITE);
        DrawText("Ao colidir com um obstaculo fixo, o seu personagem nao consegue avancar.",
                 rightX, ObstacleY + 2 * line, fontSize, RAYWHITE);
        DrawText("Ja ao colidir com obstaculos moveis, o personagem perde uma vida.",
                 rightX, ObstacleY + 3 * line, fontSize, RAYWHITE);

        int RankingY = baseY + 11 * line;
        DrawText("RANKING:", rightX, RankingY, fontSize + 4, YELLOW);
        DrawText("O ranking do jogo funcionara da seguinte forma:",
                 rightX, RankingY + line, fontSize, RAYWHITE);
        DrawText("Quanto menor o tempo que voce completa os niveis e quanto mais moedas",
                 rightX, RankingY + 2 * line, fontSize, RAYWHITE);
        DrawText("voce coleta, maior sera sua pontuacao naquele nivel.",
                 rightX, RankingY + 3 * line, fontSize, RAYWHITE);
        DrawText("OBS: todas as moedas possuem o mesmo valor de pontuacao",
                 rightX, RankingY + 4 * line, fontSize, RAYWHITE);

        const char *msg = "Pressione ESC para voltar";
        float alpha = (sin(GetTime() * 3) + 1) / 2;
        DrawText(msg,
                 w / 2 - MeasureText(msg, fontSize) / 2,
                 (int)(h - h * 0.06f), fontSize,
                 Fade(RAYWHITE, 0.6f + 0.4f * alpha));
    }
    else if (game->estado == SELECAO_NIVEL) {
        DrawTexturePro(
            game->seletorNivelBackground,
            (Rectangle){0, 0, game->seletorNivelBackground.width, game->seletorNivelBackground.height},
            (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        const char *titulo = "SELECIONE O NÍVEL";
        DrawText(titulo, game->screenWidth / 2 - MeasureText(titulo, 50) / 2, 100, 50, SKYBLUE);

        const char *niveis[4] = {
            "Nível 1 - Manguebeat de Chico Science",
            "Nível 2 - Porto de Galinhas",
            "Nível 3 - Fernando de Noronha",
            "Nível 4 - Ataques da orla de Boa Viagem"
        };

        int tamanhoFonte = 28;
        int espacamento = 60;
        int baseY = game->screenHeight / 2 - espacamento * 2;

        for (int i = 0; i < 4; i++) {
            int textWidth = MeasureText(niveis[i], tamanhoFonte);
            int posX = game->screenWidth / 2 - textWidth / 2;
            int posY = baseY + i * espacamento;

            if (i == game->nivelSelecionado) {
                float alpha = (sin(GetTime() * 4) + 1) / 2;
                Color glow = Fade(YELLOW, 0.5f + 0.5f * alpha);
                DrawRectangle(posX - 25, posY - 10, textWidth + 50, 45, Fade(BLUE, 0.3f));
                DrawText(niveis[i], posX, posY, tamanhoFonte, glow);
            } else {
                DrawText(niveis[i], posX, posY, tamanhoFonte, LIGHTGRAY);
            }
        }

        const char *msg = "ENTER para iniciar   |   ESC para voltar";
        DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, game->screenHeight - 80, 20, Fade(RAYWHITE, 0.8f));
    }
    else if (game->estado == JOGANDO) {
        ClearBackground((Color){10, 30, 60, 255});

        // HUD
        DrawRectangle(0, 0, game->screenWidth, game->hudAltura, (Color){20, 50, 80, 255});
        DrawText(TextFormat("Nivel %d", game->nivelSelecionado + 1), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Vidas: %d", game->hud.vidas), 150, 10, 20, RAYWHITE);

        Color sandColor  = (Color){194, 178, 128, 255};
        Color waterColor = (Color){8, 24, 48, 255};
        Color gridColor  = (Color){0, 0, 50, 120};

        // Desenho das colunas de fundo (mar/areia) com scroll
        float baseColuna = floorf(game->cameraX / game->colunaLargura);
        int colunasDesenho = game->numColunasVisiveis + 2;

        for (int i = 0; i < colunasDesenho; i++) {
            int colIndex = (int)baseColuna + i;
            float x = colIndex * game->colunaLargura - game->cameraX;

            Color laneColor = (colIndex % 2 == 0) ? waterColor : sandColor;
            DrawRectangle((int)x,
                          (int)game->hudAltura,
                          (int)ceilf(game->colunaLargura) + 1,
                          (int)(game->screenHeight - game->hudAltura),
                          laneColor);
        }

        // Linhas horizontais (grid)
        for (int r = 0; r <= game->linhas; r++) {
            float y = game->hudAltura + r * game->blocoTamanho;
            DrawLine(0, (int)y, game->screenWidth, (int)y, gridColor);
        }

        // Obstáculos (com scroll)
        DrawObstacles(
            game->obstaculos,
            game->cameraX,
            game->hudAltura,
            game->screenWidth,
            game->screenHeight,
            &game->obstTextures   // << adicionamos isso
        );
        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
        // Player (usa posição de TELA)
        DrawTextureEx(
            game->playerTexture,
            (Vector2){game->player.x, game->player.y},
            0.0f,
            scalePeixe,
            WHITE
        );
    }

    EndDrawing();
}

// ------------------------
// Liberação
// ------------------------
void UnloadGame(Game *game) {
    UnloadTexture(game->playerTexture);
    UnloadTexture(game->backgroundTexture);
    DestroyObstacleList(&game->obstaculos);
        // FIXOS
    UnloadTexture(game->obstTextures.pedra);
    UnloadTexture(game->obstTextures.coral);
    UnloadTexture(game->obstTextures.concha);

    // ALGA
    UnloadTexture(game->obstTextures.algaCentro);
    UnloadTexture(game->obstTextures.algaEsq);
    UnloadTexture(game->obstTextures.algaDir);

    // MÓVEIS
    UnloadTexture(game->obstTextures.carangueijoParado);
    UnloadTexture(game->obstTextures.carangueijoAnim);

    UnloadTexture(game->obstTextures.aguaVivaCentro);
    UnloadTexture(game->obstTextures.aguaVivaEsq);
    UnloadTexture(game->obstTextures.aguaVivaDir);

    UnloadTexture(game->obstTextures.baleiaParada);
    UnloadTexture(game->obstTextures.baleiaAnimada);

    UnloadTexture(game->obstTextures.tubaCentro);
    UnloadTexture(game->obstTextures.tubaEsq);
    UnloadTexture(game->obstTextures.tubaDir);

}

// ------------------------
// Helpers internos
// ------------------------

// Player sempre na coluna 1 da TELA (segunda coluna)
static void ResetPlayer(Game *game) {

    // --------------------------
    // 1) RE-CALCULA SCALE DO PLAYER
    // --------------------------
    // O sprite do peixe deve ocupar ~60% da altura do bloco
    float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;

    game->player.largura = game->playerTexture.width * scalePeixe;
    game->player.altura  = game->playerTexture.height * scalePeixe;

    // --------------------------
    // 2) POSIÇÃO INICIAL NA SEGUNDA COLUNA
    // --------------------------
    int colunaPlayer = 0; // (0 = esquerda, 1 = segunda coluna)

    float colunaLarguraTela = game->screenWidth / game->numColunasVisiveis;

    float startX =
        colunaPlayer * colunaLarguraTela +
        (colunaLarguraTela - game->player.largura) * 0.5f;

    // --------------------------
    // 3) LINHA CENTRAL EXATA DO GRID
    // (funciona para qualquer número de linhas)
    // --------------------------
    int linhaCentral = (game->linhas - 1) / 2;  // <- agora sempre funciona

    float startY =
        game->hudAltura +
        linhaCentral * game->blocoTamanho +
        (game->blocoTamanho - game->player.altura) * 0.5f;

    // --------------------------
    // 4) APLICA POSIÇÃO FINAL
    // --------------------------
    game->player.x = startX;
    game->player.y = startY;

    // --------------------------
    // 5) DEFINE HITBOX CORRETA
    // --------------------------
    game->player.hitbox = (Rectangle){
        game->player.x + game->cameraX + game->player.largura * 0.1f,
        game->player.y + game->player.altura * 0.1f,
        game->player.largura * 0.8f,
        game->player.altura * 0.8f
    };
}

bool TemObstaculoFixo(Game *game, Rectangle futuro) {
    Obstacle *o = game->obstaculos;
    while (o != NULL) {

        // ignorar móveis
        if (o->velocidade == 0) {  // fixos
            if (CheckCollisionRecs(futuro, o->hitbox)) {
                return true;
            }
        }

        o = o->next;
    }
    return false;
}

static void UpdatePlayer(Game *game) {

    float bloco = game->player.blocoTamanho;

    // HITBOX ATUAL EM MUNDO
    float mundoX = game->player.x + game->cameraX;
    Rectangle atual = {
        mundoX + game->player.largura * 0.1f,
        game->player.y + game->player.altura * 0.1f,
        game->player.largura * 0.8f,
        game->player.altura * 0.8f
    };

    // ------- MOVER PARA CIMA -------
    if (IsKeyPressed(KEY_W)) {
        Rectangle futuro = atual;
        futuro.y -= bloco;

        Obstacle *hit = CheckCollisionPlayerObstacles(futuro, game->obstaculos);
        
        // LIVRE
        if (!hit) {
            game->player.y -= bloco;
        }
        else {
            // MÓVEL → morre + reinicia nível
            if (hit->velocidade > 0) {
                GenerateWorldForLevel(game);
                ResetPlayer(game);
                return;
            }

            // FIXO → BLOQUEIA
            return;
        }
    }

    // ------- MOVER PARA BAIXO -------
    if (IsKeyPressed(KEY_S)) {
        Rectangle futuro = atual;
        futuro.y += bloco;

        Obstacle *hit = CheckCollisionPlayerObstacles(futuro, game->obstaculos);
        
        if (!hit) {
            game->player.y += bloco;
        }
        else {
            if (hit->velocidade > 0) {
                GenerateWorldForLevel(game);
                ResetPlayer(game);
                return;
            }

            // FIXO → BLOQUEIA
            return;
        }
    }

    // LIMITES
    float minY = game->hudAltura;
    float maxY = game->hudAltura + (game->linhas - 1) * bloco;

    if (game->player.y < minY) game->player.y = minY;
    if (game->player.y > maxY) game->player.y = maxY;
}
// Gera colunas iniciais do nível
void GenerateWorldForLevel(Game *game) {
    // limpa lista antiga
    DestroyObstacleList(&game->obstaculos);

    // reseta câmera
    game->cameraX        = 0.0f;
    game->cameraMovendo  = false;
    game->cameraDestinoX = 0.0f;
    game->primeiraColuna = 0;

    // quantas colunas já começam geradas
    int colunasIniciais = game->numColunasVisiveis + 5;
    if (colunasIniciais > game->worldColumns) {
        colunasIniciais = game->worldColumns;
    }

    // gera do mundo as colunas 0..colunasIniciais-1
    for (int c = 0; c < colunasIniciais; c++) {
        SpawnColumn(game, c);
    }

    game->proximaColuna = colunasIniciais;
}


// Gera uma coluna do mundo (fixa ou móvel) no índice especificado
static void SpawnColumn(Game *game, int worldColumnIndex) {
    if (worldColumnIndex < 0 || worldColumnIndex >= game->worldColumns) return;

    // PRIMEIRA COLUNA: nunca gera obstáculos
    if (worldColumnIndex == 0) {
        return;
    }


    bool colunaMovel = (worldColumnIndex % 2 == 0); // par = mar (móveis), impar = areia (fixos)

    float larguraBase = game->blocoTamanho * 0.8f;
    float colunaX = worldColumnIndex * game->colunaLargura;
    float topoHud  = game->hudAltura;
    float limiteInferior = game->hudAltura + game->linhas * game->blocoTamanho;
    float margem = 6.0f;

    int numObs = colunaMovel ? 2 : 1;

    for (int i = 0; i < numObs; i++) {
        ObstaculoTipo tipo;
        float velocidade = 0.0f;
        int direcao = 0;
        float largura, altura, x, y;

        if (colunaMovel) {
            // MÓVEIS
            int r;

            // Nível 1: apenas carangueijo
            if (game->nivelSelecionado == 0) {
                tipo = OBSTACULO_CARANGUEJO;
                velocidade = 90.0f;
            }
            // Nível 2: carangueijo + água-viva
            else if (game->nivelSelecionado == 1) {
                r = rand() % 2;
                switch (r) {
                    case 0: tipo = OBSTACULO_CARANGUEJO; velocidade = 90.0f; break;
                    case 1: tipo = OBSTACULO_AGUA_VIVA;  velocidade = 70.0f; break;
                }
            }
            // Nível 3: carangueijo + água-viva + tubarão
            else if (game->nivelSelecionado == 2) {
                r = rand() % 3;
                switch (r) {
                    case 0: tipo = OBSTACULO_CARANGUEJO; velocidade = 90.0f; break;
                    case 1: tipo = OBSTACULO_AGUA_VIVA;  velocidade = 70.0f; break;
                    case 2: tipo = OBSTACULO_TUBARAO;    velocidade = 120.0f; break;
                }
            }
            // Nível 4: todos liberados
            else {
                r = rand() % 4;
                switch (r) {
                    case 0: tipo = OBSTACULO_CARANGUEJO; velocidade = 90.0f;  break;
                    case 1: tipo = OBSTACULO_AGUA_VIVA;  velocidade = 70.0f;  break;
                    case 2: tipo = OBSTACULO_TUBARAO;    velocidade = 120.0f; break;
                    default: tipo = OBSTACULO_BALEIA;    velocidade = 50.0f;  break;
                }
            }           

            direcao = 1; // descendo

            largura = larguraBase;
            altura  = game->blocoTamanho * 0.8f;

            x = colunaX + (game->colunaLargura - largura) * 0.5f;
            y = topoHud + margem + ((float)rand() / (float)RAND_MAX) * (limiteInferior - topoHud - altura - 2*margem);
        } else {
            // FIXOS
            int r = rand() % 4;
            switch (r) {
                case 0: tipo = OBSTACULO_PEDRA;  break;
                case 1: tipo = OBSTACULO_CORAL;  break;
                case 2: tipo = OBSTACULO_CONCHA; break;
                default: tipo = OBSTACULO_ALGA;  break;
            }
            velocidade = 0.0f;
            direcao = 0;

            largura = larguraBase * 0.75f;
            altura  = game->blocoTamanho * 1.0f;

            x = colunaX + (game->colunaLargura - largura) * 0.5f;

            // escolhe linha garantida (tem espaço para 2 blocos)
            int linha = rand() % (game->linhas - 2);  

            // posiciona exatamente em cima da linha
            y = topoHud + linha * game->blocoTamanho;

        }

        Obstacle *o = CreateObstacle(tipo, x, y, largura, altura, velocidade, direcao);
        AddObstacle(&game->obstaculos, o);
    }
}
