#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

static void ResetPlayer(Game *game);
static void UpdatePlayer(Game *game);
void GenerateWorldForLevel(Game *game);
static void SpawnColumn(Game *game, int worldColumnIndex); 
Moeda* CreateMoeda(float x, float y, float largura, float altura);
void AddMoeda(Moeda **lista, Moeda *nova);
void RemoveMoedasLeftOf(Moeda **lista, float cameraX);

void SalvarBanco(int total) {
    FILE *arquivo = fopen("dados_jogo.txt", "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", total);
        fclose(arquivo);
    }
}

int CarregarBanco() {
    int total = 0;
    FILE *arquivo = fopen("dados_jogo.txt", "r");
    if (arquivo != NULL) {
        fscanf(arquivo, "%d", &total);
        fclose(arquivo);
        return total;
    }
    return 0;
}

Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    srand((unsigned int)time(NULL));
    game.musica = LoadMusicStream("assets/sons/musica.mp3");
    PlayMusicStream(game.musica);

    game.screenWidth = screenWidth;
    game.screenHeight = screenHeight;
    game.estado = MENU;

    game.linhas = 6;
    game.hudAltura = 96;
    game.blocoTamanho = (screenHeight - game.hudAltura) / game.linhas;

    game.numColunasVisiveis = 9;
    game.colunaLargura = (float)screenWidth / game.numColunasVisiveis;
    game.worldColumns = 40;

    game.cameraX = 0.0f;
    game.cameraMovendo = false;
    game.cameraDestinoX = game.cameraX;
    game.cameraVelocidade = 384.0f;

    // Player
    game.playerTexture = LoadTexture("assets/imgs/personagemsprite.png");
    game.playerTexture2 = LoadTexture("assets/imgs/peixeanimado.png");
    game.playerAnimTimer = 0;
    game.playerAnimFrame = 0;
    game.player.blocoTamanho = game.blocoTamanho;

    float scalePeixe = (game.blocoTamanho * 0.6f) / game.playerTexture.height;
    game.player.largura = game.playerTexture.width * scalePeixe;
    game.player.altura  = game.playerTexture.height * scalePeixe;

    // Texturas Gerais
    game.backgroundTexture = LoadTexture("assets/imgs/menu_jogo.png");
    game.seletorNivelBackground = game.backgroundTexture;

    // Finais de Fase
    game.finaisFase[0] = LoadTexture("assets/imgs/mangue.png");
    game.finaisFaseAnim[0] = LoadTexture("assets/imgs/manggueanimado.png");
    game.finaisFase[1] = LoadTexture("assets/imgs/porto.png");
    game.finaisFaseAnim[1] = LoadTexture("assets/imgs/portoanimado.png");
    game.finaisFase[2] = LoadTexture("assets/imgs/fernando.png");
    game.finaisFaseAnim[2] = LoadTexture("assets/imgs/fernandoanimado.png");
    game.finaisFase[3] = LoadTexture("assets/imgs/boaviagem.png");
    game.finaisFaseAnim[3] = LoadTexture("assets/imgs/boaviagemanimado.png");

    game.nivelConcluidoImg = LoadTexture("assets/imgs/nivel_concluido.png");

    game.finalFaseAtivo = false;
    game.finalFaseX = 0;
    game.bgAnimTimer = 0;
    game.bgAnimFrame = 0;
    game.showLevelComplete = false;
    game.levelCompleteTimer = 0.0f; 

    // Obstáculos
    game.obstTextures.pedra = LoadTexture("assets/imgs/pedrapronta.png");
    game.obstTextures.coral = LoadTexture("assets/imgs/coralpronto.png");
    game.obstTextures.concha = LoadTexture("assets/imgs/conchapronta.png");
    game.obstTextures.algaCentro = LoadTexture("assets/imgs/algapronta.png");
    game.obstTextures.algaEsq    = LoadTexture("assets/imgs/algaesquerdapronta.png");
    game.obstTextures.algaDir    = LoadTexture("assets/imgs/algadireitapronta.png");

    game.obstTextures.carangueijoParado = LoadTexture("assets/imgs/carangueijopronto.png");
    game.obstTextures.carangueijoAnim   = LoadTexture("assets/imgs/carangueijoanimacaopronto.png");
    game.obstTextures.aguaVivaCentro = LoadTexture("assets/imgs/aguavivaapronta.png");
    game.obstTextures.aguaVivaEsq    = LoadTexture("assets/imgs/aguavivaesquerdapronta.png");
    game.obstTextures.aguaVivaDir    = LoadTexture("assets/imgs/aguavivadireitapronta.png");
    game.obstTextures.baleiaParada  = LoadTexture("assets/imgs/baleiapronta.png");
    game.obstTextures.baleiaAnimada = LoadTexture("assets/imgs/baleiaanimadapronta.png");
    game.obstTextures.tubaCentro = LoadTexture("assets/imgs/tubaraopronto.png");
    game.obstTextures.tubaEsq    = LoadTexture("assets/imgs/tubaraoesquerda.png");
    game.obstTextures.tubaDir    = LoadTexture("assets/imgs/tubaraodireita.png");

    game.bgMar   = LoadTexture("assets/imgs/mar.png");
    game.bgAreia = LoadTexture("assets/imgs/areia.png");
    game.gameOverTexture = LoadTexture("assets/imgs/game_over.png");
    game.showGameOver = false;
    game.waitingForContinue = false;

    game.menuSelecionado = 0;
    game.nivelSelecionado = 0;
    game.hud.vidas = 3;
    game.hud.pontuacao = 0;
    game.hud.moedas = 0;
    game.totalMoedasSalvas = CarregarBanco();
    game.hudTexture = LoadTexture("assets/imgs/hudvida.png");
    game.moedaTexture = LoadTexture("assets/imgs/moeda.png");
    game.moedas = NULL;
    game.obstaculos = NULL;
    game.primeiraColuna = 0;
    game.proximaColuna = 0;

    ResetPlayer(&game);

    game.carangueijoAnimTimer = 0; game.carangueijoAnimFrame = 0;
    game.aguaVivaAnimTimer = 0; game.aguaVivaAnimFrame = 0;
    game.baleiaAnimTimer = 0; game.baleiaAnimFrame = 0;
    game.tubaraoAnimTimer = 0; game.tubaraoAnimFrame = 0;

    return game;
}

void UpdateGame(Game *game) {
    UpdateMusicStream(game->musica);

    if (game->estado == MENU) {
        if (IsKeyPressed(KEY_W) && game->menuSelecionado > 0) game->menuSelecionado--;
        if (IsKeyPressed(KEY_S) && game->menuSelecionado < 1) game->menuSelecionado++;
        if (IsKeyPressed(KEY_ENTER)) {
            if (game->menuSelecionado == 0) game->estado = SELECAO_NIVEL;
            else if (game->menuSelecionado == 1) game->estado = INSTRUCOES;
        }
    }
    else if (game->estado == INSTRUCOES) {
        if (IsKeyPressed(KEY_ESCAPE)) game->estado = MENU;
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
        if (IsKeyPressed(KEY_ESCAPE)) game->estado = MENU;
        if (IsKeyPressed(KEY_R)) {
            game->totalMoedasSalvas = 0;
            SalvarBanco(0);
        }
    }
    else if (game->estado == JOGANDO) {
        float delta = GetFrameTime();

        // TELA DE CONCLUSÃO
        if (game->showLevelComplete) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                game->totalMoedasSalvas += game->hud.moedas;
                SalvarBanco(game->totalMoedasSalvas);
                game->hud.moedas = 0;
                game->showLevelComplete = false;
                game->estado = SELECAO_NIVEL;
                DestroyObstacleList(&game->obstaculos);
                ResetPlayer(game);
            }
            return;
        }

        if (game->showGameOver && game->waitingForContinue) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->showGameOver = false;
                game->waitingForContinue = false;
                if (game->hud.vidas > 0) {
                    game->hud.moedas = 0; 
                    GenerateWorldForLevel(game);
                    ResetPlayer(game);
                } else {
                    game->hud.moedas = 0;
                    game->estado = SELECAO_NIVEL;
                    DestroyObstacleList(&game->obstaculos);
                    ResetPlayer(game);
                }
            }
            return;
        }

        // --- ANIMAÇÕES ---
        game->playerAnimTimer += delta;
        if (game->playerAnimTimer >= 1.0f) {
            game->playerAnimTimer = 0;
            game->playerAnimFrame = !game->playerAnimFrame;
        }
        
        // ANIMAÇÃO DO FUNDO
        game->bgAnimTimer += delta;
        if (game->bgAnimTimer >= 0.5f) { 
            game->bgAnimTimer = 0;
            game->bgAnimFrame = !game->bgAnimFrame;
        }

        game->carangueijoAnimTimer += delta;
        if (game->carangueijoAnimTimer >= 1.0f) {
            game->carangueijoAnimTimer = 0;
            game->carangueijoAnimFrame = !game->carangueijoAnimFrame;
        }
        game->aguaVivaAnimTimer += delta;
        if (game->aguaVivaAnimTimer >= 0.20f) {
            game->aguaVivaAnimTimer = 0;
            game->aguaVivaAnimFrame++;
            if (game->aguaVivaAnimFrame > 3) game->aguaVivaAnimFrame = 0;
        }
        game->baleiaAnimTimer += delta;
        if (game->baleiaAnimTimer >= 0.30f) {
            game->baleiaAnimTimer = 0;
            game->baleiaAnimFrame = !game->baleiaAnimFrame;
        }
        game->tubaraoAnimTimer += delta;
        if (game->tubaraoAnimTimer >= 0.10f) {
            game->tubaraoAnimTimer = 0;
            game->tubaraoAnimFrame++;
            if (game->tubaraoAnimFrame > 3) game->tubaraoAnimFrame = 0;
        }

        UpdatePlayer(game);

        if (IsKeyDown(KEY_D) && !game->cameraMovendo && game->tempoParado <= 0) {
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
            if (hit && hit->velocidade == 0) return;
            if (hit && hit->velocidade > 0) {
                if (game->hud.vidas > 0) game->hud.vidas--;
                game->showGameOver = true;
                game->waitingForContinue = true;
                return;
            }

            int colunaAtual = (int)floorf((game->cameraX + 1.0f) / game->colunaLargura);
            int proximaColuna = colunaAtual + 1;
            if (proximaColuna >= game->worldColumns) proximaColuna = game->worldColumns - 1;

            game->cameraDestinoX = proximaColuna * game->colunaLargura;
            game->cameraMovendo = true;
        }

        if (game->cameraMovendo) {
            game->cameraX += game->cameraVelocidade * delta;
            if (game->cameraX >= game->cameraDestinoX - 0.5f) {
                game->cameraX = game->cameraDestinoX;
                game->cameraMovendo = false;
            }
            game->tempoParado = 0.08f;
        } else {
            if (game->tempoParado > 0) game->tempoParado -= delta;
        }

        // --- CORREÇÃO DA LÓGICA DO FINAL DE FASE ---
        if (!game->finalFaseAtivo) {
            int colunaAtual = (int)(game->cameraX / game->colunaLargura);
            if (colunaAtual >= game->worldColumns - game->numColunasVisiveis) {
                game->finalFaseAtivo = true;
                // CORREÇÃO: Alinhamos o fundo com a ÚLTIMA coluna onde a câmera para (worldColumns - 1)
                game->finalFaseX = (game->worldColumns - 1) * game->colunaLargura;
                game->levelCompleteTimer = 0.0f;
            }
        } else {
            float screenX = game->finalFaseX - game->cameraX;
            // Verifica se a imagem cobriu a tela (com pequena margem de tolerância de 5 pixels)
            if (screenX <= 5.0f) {
                game->levelCompleteTimer += delta;
                if (game->levelCompleteTimer >= 3.0f) { 
                    game->showLevelComplete = true;
                }
            }
        }

        float limiteRemocao = (game->primeiraColuna + 1) * game->colunaLargura;
        while (game->cameraX >= limiteRemocao) {
            RemoveObstaclesLeftOf(&game->obstaculos, game->cameraX);
            RemoveMoedasLeftOf(&game->moedas, game->cameraX);
            game->primeiraColuna++;
            if (game->proximaColuna < game->worldColumns) {
                SpawnColumn(game, game->proximaColuna);
                game->proximaColuna++;
            }
            limiteRemocao = (game->primeiraColuna + 1) * game->colunaLargura;
        }

        UpdateObstacles(game->obstaculos, delta, game->hudAltura, (float)game->screenHeight);

        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
        float realW = game->playerTexture.width * scalePeixe;
        float realH = game->playerTexture.height * scalePeixe;
        float mundoX = game->player.x + game->cameraX;
        game->player.hitbox.x = mundoX + realW * 0.05f;
        game->player.hitbox.y = game->player.y + realH * 0.05f;
        game->player.hitbox.width  = realW * 0.90f;
        game->player.hitbox.height = realH * 0.90f;

        Obstacle *colisao = CheckCollisionPlayerObstacles(game->player.hitbox, game->obstaculos);
        if (colisao != NULL && colisao->velocidade > 0) {
            if (game->hud.vidas > 0) game->hud.vidas--;
            game->showGameOver = true;
            game->waitingForContinue = true;
            return;
        }
        
        Moeda *m = game->moedas;
        Moeda *ant = NULL;
        while (m) {
            if (CheckCollisionRecs(game->player.hitbox, m->hitbox)) {
                game->hud.moedas++;
                game->hud.pontuacao += 5;
                if (!ant) game->moedas = m->next;
                else ant->next = m->next;
                free(m);
                break;
            }
            ant = m;
            m = m->next;
        }

        if (IsKeyPressed(KEY_Q)) {
            game->totalMoedasSalvas += game->hud.moedas;
            SalvarBanco(game->totalMoedasSalvas);
            game->hud.moedas = 0;
            game->estado = SELECAO_NIVEL;
            DestroyObstacleList(&game->obstaculos);
            ResetPlayer(game);
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->hud.moedas = 0; 
            game->estado = SELECAO_NIVEL;
            DestroyObstacleList(&game->obstaculos);
            ResetPlayer(game);
        }

        if (!game->cameraMovendo) {
            float snapped = roundf(game->cameraX / game->colunaLargura) * game->colunaLargura;
            game->cameraX = snapped;
            game->cameraDestinoX = snapped;
        }
    }
}

void DrawGame(Game *game) {
    BeginDrawing();

    if (game->estado == MENU) {
        DrawTexturePro(game->backgroundTexture,
            (Rectangle){0, 0, game->backgroundTexture.width, game->backgroundTexture.height},
            (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
            (Vector2){0, 0}, 0.0f, WHITE);
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
        ClearBackground((Color){5, 25, 45, 255});
        int w = game->screenWidth;
        int h = game->screenHeight;
        const char *titulo = "INSTRUCOES DO JOGO";
        DrawText(titulo, w / 2 - MeasureText(titulo, h * 0.05f) / 2, (int)(h * 0.08f), (int)(h * 0.05f), SKYBLUE);
        int fontSize = (int)(h * 0.01f);
        if (fontSize < 20) fontSize = 20; 
        int posY = (int)(h * 0.18f);
        const char *intro[] = {
            "Bem-vindo as profundezas do Atlantis-Dash!",
            "Nade entre corredores de areia e agua em Atlantida.",
            "Desvie de animais perigosos e corais enquanto avanca.",
            "Use seus reflexos para chegar o mais longe possivel!"
        };
        for (int i = 0; i < 4; i++) {
            DrawText(intro[i], w / 2 - MeasureText(intro[i], fontSize) / 2, posY + i * (fontSize + 6), fontSize, RAYWHITE);
        }
        const char *msg = "Pressione ESC para voltar";
        DrawText(msg, w / 2 - MeasureText(msg, fontSize) / 2, (int)(h - h * 0.06f), fontSize, WHITE);
    }
    else if (game->estado == SELECAO_NIVEL) {
        DrawTexturePro(game->seletorNivelBackground,
            (Rectangle){0, 0, game->seletorNivelBackground.width, game->seletorNivelBackground.height},
            (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
            (Vector2){0, 0}, 0.0f, WHITE);
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
        const char *msg = "ENTER: Iniciar | ESC: Voltar | R: Zerar Moedas";
        DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, game->screenHeight - 80, 20, Fade(RAYWHITE, 0.8f));
    }
    else if (game->estado == JOGANDO) {
        ClearBackground((Color){10, 30, 60, 255});
        
        float baseColuna = floorf(game->cameraX / game->colunaLargura);
        int colunasDesenho = game->numColunasVisiveis + 2;

        for (int i = 0; i < colunasDesenho; i++) {
            int colIndex = (int)baseColuna + i;
            float x = colIndex * game->colunaLargura - game->cameraX;
            bool colunaMovel = (colIndex % 2 == 0);
            Texture2D tile = colunaMovel ? game->bgMar : game->bgAreia;
            for (int linha = 0; linha < game->linhas; linha++) {
                float y = game->hudAltura + linha * game->blocoTamanho;
                DrawTexturePro(tile, 
                               (Rectangle){0,0,tile.width,tile.height},
                               (Rectangle){x,y,game->colunaLargura,game->blocoTamanho},
                               (Vector2){0,0}, 0.0f, WHITE);
            }
        }

        // --- DESENHO DO FINAL DA FASE ---
        if (game->finalFaseAtivo) {
            float screenX = game->finalFaseX - game->cameraX;
            
            Texture2D texFinal;
            if (game->bgAnimFrame == 0) {
                texFinal = game->finaisFase[game->nivelSelecionado];
            } else {
                texFinal = game->finaisFaseAnim[game->nivelSelecionado];
            }

            DrawTexturePro(
                texFinal,
                (Rectangle){0,0, texFinal.width, texFinal.height},
                (Rectangle){
                    screenX,
                    game->hudAltura,
                    game->screenWidth,
                    game->screenHeight - game->hudAltura
                },
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        }

        DrawTexturePro(game->hudTexture, 
                       (Rectangle){0,0, game->hudTexture.width, game->hudTexture.height},
                       (Rectangle){0,0, game->screenWidth, game->hudAltura}, 
                       (Vector2){0,0}, 0.0f, WHITE);
        
        Color gridColor = (Color){0,0,50,120};
        for (int r = 0; r <= game->linhas; r++) {
            float y = game->hudAltura + r * game->blocoTamanho;
            DrawLine(0, (int)y, game->screenWidth, (int)y, gridColor);
        }

        int fontSize = 20;
        DrawText(TextFormat("VIDAS: %d", game->hud.vidas), 20, 35, fontSize, RED);
        int visualMoedas = game->totalMoedasSalvas + game->hud.moedas;
        const char *textoMoedas = TextFormat("MOEDAS: %d", visualMoedas);
        DrawText(textoMoedas, game->screenWidth - MeasureText(textoMoedas, fontSize) - 20, 35, fontSize, GOLD);
        const char *ajuda = "Q: Salvar e Sair  |  ESC: Desistir";
        DrawText(ajuda, game->screenWidth/2 - MeasureText(ajuda, 10)/2, 70, 10, LIGHTGRAY);

        DrawObstacles(game->obstaculos, game->cameraX, game->hudAltura, 
                      game->screenWidth, game->screenHeight, &game->obstTextures,
                      game->carangueijoAnimFrame, game->aguaVivaAnimFrame, 
                      game->baleiaAnimFrame, game->tubaraoAnimFrame);

        Moeda *m = game->moedas;
        while (m) {
            float sx = m->x - game->cameraX;
            float scale = m->largura / game->moedaTexture.width;
            DrawTextureEx(game->moedaTexture, (Vector2){sx, m->y}, 0.0f, scale, WHITE);
            m = m->next;
        }

        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
        Texture2D ptex = (game->playerAnimFrame == 0) ? game->playerTexture : game->playerTexture2;
        
        if (!game->showLevelComplete) {
            DrawTextureEx(ptex, (Vector2){game->player.x, game->player.y}, 0.0f, scalePeixe, WHITE);
        }

        if (game->showLevelComplete) {
            DrawTexturePro(game->nivelConcluidoImg,
                (Rectangle){0, 0, game->nivelConcluidoImg.width, game->nivelConcluidoImg.height},
                (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
                (Vector2){0, 0}, 0.0f, WHITE);
            
            int tw = MeasureText(msg, 30);
            DrawText(msg, game->screenWidth/2 - tw/2, game->screenHeight - 100, 30, GREEN);
        }

        if (game->showGameOver) {
            float destW = game->screenWidth * 0.8f;
            float destH = destW * ((float)game->gameOverTexture.height / (float)game->gameOverTexture.width);
            float destX = (game->screenWidth - destW) * 0.5f;
            float destY = (game->screenHeight - destH) * 0.5f;
            DrawTexturePro(game->gameOverTexture, 
                           (Rectangle){0, 0, (float)game->gameOverTexture.width, (float)game->gameOverTexture.height},
                           (Rectangle){destX, destY, destW, destH}, 
                           (Vector2){0, 0}, 0.0f, WHITE);
            const char *msg = "Pressione ESC para continuar";
            DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, (int)(destY + destH + 10), 20, RAYWHITE);
        }
    }
    EndDrawing();
}

void UnloadGame(Game *game) {
    UnloadTexture(game->playerTexture);
    UnloadTexture(game->playerTexture2);
    UnloadTexture(game->backgroundTexture);
    for (int i=0; i<4; i++) {
        UnloadTexture(game->finaisFase[i]);
        UnloadTexture(game->finaisFaseAnim[i]);
    }
    UnloadTexture(game->nivelConcluidoImg);
    DestroyObstacleList(&game->obstaculos);
    UnloadTexture(game->obstTextures.pedra);
    UnloadTexture(game->obstTextures.coral);
    UnloadTexture(game->obstTextures.concha);
    UnloadTexture(game->obstTextures.algaCentro);
    UnloadTexture(game->obstTextures.algaEsq);
    UnloadTexture(game->obstTextures.algaDir);
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
    UnloadTexture(game->bgMar);
    UnloadTexture(game->bgAreia);
    UnloadTexture(game->gameOverTexture);
    UnloadTexture(game->hudTexture);
    UnloadTexture(game->moedaTexture);
    UnloadMusicStream(game->musica);
}

static void ResetPlayer(Game *game) {
    float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
    game->player.largura = game->playerTexture.width * scalePeixe;
    game->player.altura  = game->playerTexture.height * scalePeixe;
    int colunaPlayer = 0; 
    float colunaLarguraTela = game->screenWidth / game->numColunasVisiveis;
    float startX = colunaPlayer * colunaLarguraTela + (colunaLarguraTela - game->player.largura) * 0.5f;
    int linhaCentral = (game->linhas - 1) / 2;
    float startY = game->hudAltura + linhaCentral * game->blocoTamanho + (game->blocoTamanho - game->player.altura) * 0.5f;
    game->player.x = startX;
    game->player.y = startY;
    game->player.hitbox = (Rectangle){
        game->player.x + game->cameraX + game->player.largura * 0.1f,
        game->player.y + game->player.altura * 0.1f,
        game->player.largura * 0.8f,
        game->player.altura * 0.8f
    };
}

static void UpdatePlayer(Game *game) {
    float bloco = game->player.blocoTamanho;
    float mundoX = game->player.x + game->cameraX;
    Rectangle atual = { mundoX, game->player.y, game->player.largura, game->player.altura };
    float speed = 260.0f;
    float delta = GetFrameTime();
    Rectangle futuro = atual;
    if (IsKeyDown(KEY_W)) futuro.y -= speed * delta;
    if (IsKeyDown(KEY_S)) futuro.y += speed * delta;
    Obstacle *hit = CheckCollisionPlayerObstacles(futuro, game->obstaculos);
    if (!hit) {
        game->player.y = futuro.y;
    }
    else if (hit->velocidade > 0) {
        if (game->hud.vidas > 0) game->hud.vidas--;
        game->showGameOver = true;
        game->waitingForContinue = true;
        return;
    }
    float minY = game->hudAltura;
    float maxY = game->hudAltura + (game->linhas - 1) * bloco;
    if (game->player.y < minY) game->player.y = minY;
    if (game->player.y > maxY) game->player.y = maxY;
}

void GenerateWorldForLevel(Game *game) {
    DestroyObstacleList(&game->obstaculos);
    game->cameraX        = 0.0f;
    game->cameraMovendo  = false;
    game->cameraDestinoX = 0.0f;
    game->primeiraColuna = 0;
    game->finalFaseAtivo = false;
    game->showLevelComplete = false;
    game->bgAnimFrame = 0;
    game->bgAnimTimer = 0;
    game->levelCompleteTimer = 0.0f; 

    int colunasIniciais = game->numColunasVisiveis + 5;
    if (colunasIniciais > game->worldColumns) {
        colunasIniciais = game->worldColumns;
    }
    for (int c = 0; c < colunasIniciais; c++) {
        SpawnColumn(game, c);
    }
    game->proximaColuna = colunasIniciais;
}

static void SpawnColumn(Game *game, int worldColumnIndex) {
    if (worldColumnIndex < 0 || worldColumnIndex >= game->worldColumns) return;
    if (worldColumnIndex == 0) return;
    bool colunaMovel = (worldColumnIndex % 2 == 0); 
    float larguraBase = game->blocoTamanho * 0.8f;
    float colunaX = worldColumnIndex * game->colunaLargura;
    float topoHud  = game->hudAltura;
    float limiteInferior = game->hudAltura + game->linhas * game->blocoTamanho;
    int numObs;
    if (colunaMovel) {
        if (game->nivelSelecionado == 0) numObs = 1;
        else if (game->nivelSelecionado == 1) numObs = 2;
        else numObs = 2;
    } else {
        if (game->nivelSelecionado == 0) numObs = 2;
        else if (game->nivelSelecionado == 1) numObs = 2;
        else if (game->nivelSelecionado == 2) numObs = 2;
        else numObs = 4;
    }
    float posicoesGeradas[4];
    int gerados = 0;
    for (int i = 0; i < numObs; i++) {
        ObstaculoTipo tipo;
        float velocidade = 0.0f;
        int direcao = 0;
        float largura, altura, x, y;
        float distanciaMinima;
        if (colunaMovel) {
            if (game->nivelSelecionado == 0) {
                tipo = OBSTACULO_CARANGUEJO; velocidade = 60.0f; largura = larguraBase * 0.9f; altura = game->blocoTamanho * 0.8f;
            } else if (game->nivelSelecionado == 1) {
                int indiceMar = worldColumnIndex / 2;
                if (indiceMar % 2 == 0) { tipo = OBSTACULO_CARANGUEJO; velocidade = 60.0f; largura = larguraBase * 0.9f; altura = game->blocoTamanho * 0.8f; }
                else { tipo = OBSTACULO_AGUA_VIVA; velocidade = 80.0f; largura = larguraBase * 1.2f; altura = game->blocoTamanho * 0.9f; }
            } else if (game->nivelSelecionado == 2) {
                 int indiceMar = worldColumnIndex / 2;
                 int contador = indiceMar % 3;
                 if (contador == 0) { tipo = OBSTACULO_CARANGUEJO; velocidade = 60.0f; largura = larguraBase * 0.9f; altura = game->blocoTamanho * 0.8f; }
                 else if (contador == 1) { tipo = OBSTACULO_AGUA_VIVA; velocidade = 80.0f; largura = larguraBase * 1.2f; altura = game->blocoTamanho * 0.9f; }
                 else { tipo = OBSTACULO_BALEIA; velocidade = 50.0f; largura = larguraBase * 1.8f; altura = game->blocoTamanho * 1.4f; }
            } else {
                 int indiceMar = worldColumnIndex / 2;
                 int contador = indiceMar % 4;
                 if (contador == 0) { tipo = OBSTACULO_CARANGUEJO; velocidade = 60.0f; largura = larguraBase * 0.9f; altura = game->blocoTamanho * 0.8f; }
                 else if (contador == 1) { tipo = OBSTACULO_AGUA_VIVA; velocidade = 80.0f; largura = larguraBase * 1.2f; altura = game->blocoTamanho * 0.9f; }
                 else if (contador == 2) { tipo = OBSTACULO_BALEIA; velocidade = 50.0f; largura = larguraBase * 1.8f; altura = game->blocoTamanho * 1.4f; }
                 else { tipo = OBSTACULO_TUBARAO; velocidade = 100.0f; largura = larguraBase * 1.5f; altura = game->blocoTamanho * 1.1f; }
            }
            if (tipo == OBSTACULO_BALEIA) distanciaMinima = altura * 1.5f;
            else if (tipo == OBSTACULO_TUBARAO) distanciaMinima = altura * 1.25f;
            else distanciaMinima = altura * 1.00f;
            direcao = 1;
            float ajuste = 0.0f;
            if (tipo == OBSTACULO_BALEIA) ajuste = game->colunaLargura * 0.35f;
            else if (tipo == OBSTACULO_TUBARAO) ajuste = game->colunaLargura * 0.28f;
            x = colunaX + (game->colunaLargura - largura) * 0.5f + ajuste;
            float zonaMin = topoHud + (game->blocoTamanho * 0.2f);
            float zonaMax = limiteInferior - altura - (game->blocoTamanho * 0.2f);
            int tentativas = 0; bool valido = false;
            while (!valido && tentativas < 20) {
                y = zonaMin + (float)rand() / RAND_MAX * (zonaMax - zonaMin);
                valido = true;
                for (int k = 0; k < gerados; k++) {
                    float outraY = posicoesGeradas[k];
                    if ((y < outraY + altura + 5) && (y + altura + 5 > outraY)) { valido = false; break; }
                }
                tentativas++;
            }
            posicoesGeradas[gerados++] = y;
        } else {
            int r = rand() % 4;
            switch (r) {
                case 0: tipo = OBSTACULO_PEDRA;  break;
                case 1: tipo = OBSTACULO_CORAL;  break;
                case 2: tipo = OBSTACULO_CONCHA; break;
                default: tipo = OBSTACULO_ALGA;  break;
            }
            velocidade = 0.0f; direcao = 0;
            largura = game->blocoTamanho * 0.55f;
            altura  = game->blocoTamanho * 0.55f;
            x = colunaX + (game->colunaLargura / 2) - (largura / 2);
            int tentativas = 0; bool valido = false;
            while (!valido && tentativas < 30) {
                int linha = rand() % game->linhas;
                y = topoHud + linha * game->blocoTamanho + (game->blocoTamanho / 2) - (altura / 2);
                y += game->blocoTamanho * 0.05f;
                valido = true;
                for (int i = 0; i < gerados; i++) {
                    if (fabsf(y - posicoesGeradas[i]) < distanciaMinima) { valido = false; break; }
                }
                tentativas++;
            }
            if (!valido) break;
            posicoesGeradas[gerados++] = y;
        }
        Obstacle *o = CreateObstacle(tipo, x, y, largura, altura, velocidade, direcao);
        AddObstacle(&game->obstaculos, o);
    }
    float sorte = (float)rand() / RAND_MAX;
    if (!colunaMovel && sorte < 0.50f) {
        int matrizLogica[6][2];
        for (int l = 0; l < game->linhas; l++) { for (int c = 0; c < 2; c++) matrizLogica[l][c] = 0; }
        Obstacle *temp = game->obstaculos;
        while (temp != NULL) {
            if (fabsf(temp->x - colunaX) < game->colunaLargura * 0.5f) {
                int linhaMatriz = (int)((temp->y - topoHud) / game->blocoTamanho);
                if (linhaMatriz >= 0 && linhaMatriz < game->linhas) matrizLogica[linhaMatriz][0] = 1;
            }
            temp = temp->next;
        }
        int linhasLivres[6]; int qtdLivres = 0;
        for (int i = 0; i < game->linhas; i++) { if (matrizLogica[i][0] == 0) linhasLivres[qtdLivres++] = i; }
        if (qtdLivres > 0) {
            int indiceSorteado = rand() % qtdLivres;
            int linhaFinal = linhasLivres[indiceSorteado];
            float moedaL = game->blocoTamanho * 0.5f; float moedaA = game->blocoTamanho * 0.5f;
            float x = colunaX + (game->colunaLargura - moedaL) * 0.5f;
            float y = topoHud + linhaFinal * game->blocoTamanho + (game->blocoTamanho - moedaA) * 0.5f;
            Moeda *m = CreateMoeda(x, y, moedaL, moedaA);
            AddMoeda(&game->moedas, m);
        }
    }
}

Moeda* CreateMoeda(float x, float y, float largura, float altura) {
    Moeda *m = malloc(sizeof(Moeda));
    m->x = x; m->y = y; m->largura = largura; m->altura = altura;
    m->hitbox = (Rectangle){x, y, largura, altura}; m->next = NULL;
    return m;
}

void AddMoeda(Moeda **lista, Moeda *nova) {
    if (!*lista) { *lista = nova; return; }
    Moeda *atual = *lista;
    while (atual->next) atual = atual->next;
    atual->next = nova;
}

void RemoveMoedasLeftOf(Moeda **lista, float cameraX) {
    Moeda *atual = *lista, *ant = NULL;
    while (atual) {
        if (atual->x + atual->largura < cameraX - 10) {
            Moeda *rem = atual;
            if (!ant) *lista = atual->next;
            else ant->next = atual->next;
            atual = (ant ? ant->next : *lista);
            free(rem);
        } else {
            ant = atual;
            atual = atual->next;
        }
    }
}