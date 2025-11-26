#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdio.h>


// Funções internas (estáticas)
static void ResetPlayer(Game *game);
static void UpdatePlayer(Game *game);
void GenerateWorldForLevel(Game *game);
static void SpawnColumn(Game *game, int worldColumnIndex); 


Moeda* CreateMoeda(float x, float y, float largura, float altura);
void AddMoeda(Moeda **lista, Moeda *nova);
void RemoveMoedasLeftOf(Moeda **lista, float cameraX);
// --- FUNÇÕES DE ARQUIVO (REQUISITO OBRIGATÓRIO) ---
// Salva a pontuação total em um arquivo de texto
void SalvarBanco(int total) {
    FILE *arquivo = fopen("dados_jogo.txt", "w"); // Abre para escrita (Write)
    if (arquivo != NULL) {
        fprintf(arquivo, "%d", total);
        fclose(arquivo);
    }
}

// Carrega a pontuação do arquivo
int CarregarBanco() {
    int total = 0;
    FILE *arquivo = fopen("dados_jogo.txt", "r"); // Abre para leitura (Read)
    if (arquivo != NULL) {
        fscanf(arquivo, "%d", &total);
        fclose(arquivo);
        return total;
    }
    return 0; // Se não existir, retorna 0
}
// ------------------------
// Inicialização do jogo
// ------------------------
Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    srand((unsigned int)time(NULL));
    game.musica = LoadMusicStream("assets/sons/musica.mp3");
    PlayMusicStream(game.musica);

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
    game.playerTexture2 = LoadTexture("assets/imgs/peixeanimado.png");
    game.playerAnimTimer = 0;
    game.playerAnimFrame = 0;   // começa com o sprite original

    game.player.blocoTamanho = game.blocoTamanho;

    // tamanho real do sprite após scale
    float scalePeixe = (game.blocoTamanho * 0.6f) / game.playerTexture.height;
    game.player.largura = game.playerTexture.width * scalePeixe;
    game.player.altura  = game.playerTexture.height * scalePeixe;

    // Texturas
    
    game.backgroundTexture = LoadTexture("assets/imgs/menu_jogo.png");
    game.seletorNivelBackground = game.backgroundTexture; // por enquanto igual

    // niveis
    game.mangueTexture = LoadTexture("assets/imgs/mangue.png");
    game.mangueAtivo = false;
    game.mangueX = 0;


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

    game.bgMar   = LoadTexture("assets/imgs/mar.png");
    game.bgAreia = LoadTexture("assets/imgs/areia.png");

    // SPRITE GAME OVER (overlay exibida quando perde vida)
    game.gameOverTexture = LoadTexture("assets/imgs/game_over.png"); // coloque o arquivo em assets/imgs/
    game.showGameOver = false;
    game.waitingForContinue = false;

    // Menus / HUD
    game.menuSelecionado = 0;
    game.nivelSelecionado = 0;
    game.hud.vidas = 3;
    game.hud.pontuacao = 0;
    game.hud.moedas = 0;
    game.totalMoedasSalvas = CarregarBanco(); // Usa a nova função
    game.hudTexture = LoadTexture("assets/imgs/hudvida.png");
    game.moedaTexture = LoadTexture("assets/imgs/moeda.png");
    game.moedas = NULL;



    // Obstáculos
    game.obstaculos = NULL;
    game.primeiraColuna = 0;
    game.proximaColuna = 0;

    ResetPlayer(&game);

    // animacao obstaculos
    game.carangueijoAnimTimer = 0;
    game.carangueijoAnimFrame = 0;
    game.aguaVivaAnimTimer = 0;
    game.aguaVivaAnimFrame = 0;
    game.baleiaAnimTimer = 0;
    game.baleiaAnimFrame = 0;
    game.tubaraoAnimTimer = 0;
    game.tubaraoAnimFrame = 0;



    return game;
}

// ------------------------
// Atualização
// ------------------------
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

        // --- NOVO: Tecla R para Zerar o Arquivo (Resetar Progresso) ---
        if (IsKeyPressed(KEY_R)) {
            game->totalMoedasSalvas = 0;
            SalvarBanco(0); // Função de arquivo obrigatória
        }
    }
    else if (game->estado == JOGANDO) {
        float delta = GetFrameTime();

        // Se estamos exibindo a sprite de "game over" aguardando o jogador apertar ESC
        if (game->showGameOver && game->waitingForContinue) {
            if (IsKeyPressed(KEY_ESCAPE)) {
                game->showGameOver = false;
                game->waitingForContinue = false;
                
                // Se ainda há vidas, regenerar mundo e resetar o player
                if (game->hud.vidas > 0) {
                    game->hud.moedas = 0; // <--- NOVO: Perde as moedas da fase ao morrer
                    GenerateWorldForLevel(game);
                    ResetPlayer(game);
                } else {
                    // sem vidas: volta para seleção de nível
                    game->hud.moedas = 0; // Perde as moedas da fase
                    game->estado = SELECAO_NIVEL;
                    DestroyObstacleList(&game->obstaculos);
                    ResetPlayer(game);
                }
            }
            return;
        }

        // ANIMAÇÃO DO PLAYER
        game->playerAnimTimer += GetFrameTime();
        if (game->playerAnimTimer >= 1.0f) {
            game->playerAnimTimer = 0;
            game->playerAnimFrame = !game->playerAnimFrame;
        }
        // ANIMAÇÃO DO CARANGUEIJO
        game->carangueijoAnimTimer += GetFrameTime();
        if (game->carangueijoAnimTimer >= 1.0f) {
            game->carangueijoAnimTimer = 0;
            game->carangueijoAnimFrame = !game->carangueijoAnimFrame;
        }
        // ANIMAÇÃO DA ÁGUA-VIVA
        game->aguaVivaAnimTimer += GetFrameTime();
        if (game->aguaVivaAnimTimer >= 0.20f) {
            game->aguaVivaAnimTimer = 0;
            game->aguaVivaAnimFrame++;

            if (game->aguaVivaAnimFrame > 3)
                game->aguaVivaAnimFrame = 0;
        }
        // ANIMAÇÃO DA BALEIA
        game->baleiaAnimTimer += GetFrameTime();
        if (game->baleiaAnimTimer >= 0.30f) {   // <-- muda de frame a cada 1 segundo
            game->baleiaAnimTimer = 0;
            game->baleiaAnimFrame = !game->baleiaAnimFrame;   // alterna 0 ↔ 1
        }
        // ANIMAÇÃO DO TUBARÃO
        game->tubaraoAnimTimer += GetFrameTime();
        if (game->tubaraoAnimTimer >= 0.10f) {   // 1 segundo entre frames
            game->tubaraoAnimTimer = 0;
            game->tubaraoAnimFrame++;

            if (game->tubaraoAnimFrame > 3)     // 4 frames: 0..3
                game->tubaraoAnimFrame = 0;
        }




        // MOVIMENTO VERTICAL
        UpdatePlayer(game);

        // ------ MOVIMENTO HORIZONTAL EM BLOCOS (D) ------
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

            if (hit && hit->velocidade == 0)
                return;

            if (hit && hit->velocidade > 0) {
                if (game->hud.vidas > 0) game->hud.vidas--;
                game->showGameOver = true;
                game->waitingForContinue = true;
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
            game->tempoParado = 0.08f;
        } 
        else {
            if (game->tempoParado > 0)
            game->tempoParado -= delta;
        }
        
        // ATIVAR O MANGUE QUANDO CHEGAR NO FINAL DO MAPA
        if (!game->mangueAtivo) {

            int colunaAtual = (int)(game->cameraX / game->colunaLargura);

            // Quando entrar nas últimas N colunas visíveis
            if (colunaAtual >= game->worldColumns - game->numColunasVisiveis) {

                game->mangueAtivo = true;

                // Posição inicial do mangue = grudado no final do mundo
                game->mangueX = game->worldColumns * game->colunaLargura;
            }
        }

        
        // GERAR / REMOVER COLUNAS
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

        // MOVIMENTO DOS OBSTÁCULOS
        UpdateObstacles(game->obstaculos, delta, game->hudAltura, (float)game->screenHeight);

        // ATUALIZA HITBOX DO PLAYER
        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
        float realW = game->playerTexture.width * scalePeixe;
        float realH = game->playerTexture.height * scalePeixe;
        float mundoX = game->player.x + game->cameraX;

        game->player.hitbox.x = mundoX + realW * 0.05f;
        game->player.hitbox.y = game->player.y + realH * 0.05f;
        game->player.hitbox.width  = realW * 0.90f;
        game->player.hitbox.height = realH * 0.90f;

        // ------ COLISÃO CONTÍNUA COM OBSTÁCULOS MÓVEIS ------
        Obstacle *colisao = CheckCollisionPlayerObstacles(game->player.hitbox, game->obstaculos);

        if (colisao != NULL && colisao->velocidade > 0) {
            if (game->hud.vidas > 0) game->hud.vidas--;
            game->showGameOver = true;
            game->waitingForContinue = true;
            return;
        }
        
        // COLETA DE MOEDA
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

        // --- LÓGICA DE ARQUIVOS E SAÍDA (ATUALIZADO) ---

        // OPÇÃO 1: SALVAR E SAIR (Tecla Q) -> Grava no Arquivo
        if (IsKeyPressed(KEY_Q)) {
            // Soma o que ganhou na fase ao total do banco
            game->totalMoedasSalvas += game->hud.moedas;
            
            // Grava no arquivo físico
            SalvarBanco(game->totalMoedasSalvas); 
            
            // Zera o contador da fase e volta pro menu
            game->hud.moedas = 0;
            game->estado = SELECAO_NIVEL;
            DestroyObstacleList(&game->obstaculos);
            ResetPlayer(game);
        }

        // OPÇÃO 2: DESISTIR (Tecla ESC) -> Perde o que ganhou na fase
        if (IsKeyPressed(KEY_ESCAPE)) {
            // Zera o contador da fase sem salvar no banco
            game->hud.moedas = 0; 

            game->estado = SELECAO_NIVEL;
            DestroyObstacleList(&game->obstaculos);
            ResetPlayer(game);
        }

        // SNAPPING
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
        DrawText("Chegar vivo ao final...", leftX, baseY + line, fontSize, RAYWHITE); 
        // (Pode manter seus textos originais aqui se quiser, abreviei para caber)

        int controlsY = baseY + 5 * line;
        DrawText("CONTROLES:", leftX, controlsY, fontSize + 4, YELLOW);
        DrawText("W/S - Cima/Baixo | D - Avancar", leftX + 20, controlsY + 2 * line, fontSize, RAYWHITE);
        DrawText("Q - Salvar e Sair | R - Resetar Save", leftX + 20, controlsY + 4 * line, fontSize, RAYWHITE);

        const char *msg = "Pressione ESC para voltar";
        float alpha = (sin(GetTime() * 3) + 1) / 2;
        DrawText(msg, w / 2 - MeasureText(msg, fontSize) / 2, (int)(h - h * 0.06f), fontSize, Fade(RAYWHITE, 0.6f + 0.4f * alpha));
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

        // --- ALTERADO: Adicionado aviso da tecla R ---
        const char *msg = "ENTER: Iniciar | ESC: Voltar | R: Zerar Moedas";
        DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, game->screenHeight - 80, 20, Fade(RAYWHITE, 0.8f));
    }
    else if (game->estado == JOGANDO) {
        ClearBackground((Color){10, 30, 60, 255});
        
        // ---------------- BACKGROUND ----------------
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
        if (game->mangueAtivo) {
            float screenX = game->mangueX - game->cameraX;
            DrawTexturePro(
                game->mangueTexture,
                (Rectangle){0,0, game->mangueTexture.width, game->mangueTexture.height},
                (Rectangle){
                    screenX,
                    game->hudAltura,
                    game->screenWidth,                        // cobre a área jogável
                    game->screenHeight - game->hudAltura
                },
                (Vector2){0,0},
                0.0f,
                WHITE
            );
        }
        

        // ---------------- HUD ----------------
        DrawTexturePro(game->hudTexture, 
                       (Rectangle){0,0, game->hudTexture.width, game->hudTexture.height},
                       (Rectangle){0,0, game->screenWidth, game->hudAltura}, 
                       (Vector2){0,0}, 0.0f, WHITE);
        
        Color gridColor = (Color){0,0,50,120};
        for (int r = 0; r <= game->linhas; r++) {
            float y = game->hudAltura + r * game->blocoTamanho;
            DrawLine(0, (int)y, game->screenWidth, (int)y, gridColor);
        }

        // ---------------- TEXTOS DO HUD (ALTERADO) ----------------
        int fontSize = 20;

        // 1. Vidas (Esquerda)
        DrawText(TextFormat("VIDAS: %d", game->hud.vidas), 20, 35, fontSize, RED);

        // 2. Moedas Totais (Direita Superior)
        // Soma o que já tinha no banco + o que pegou nesta fase
        int visualMoedas = game->totalMoedasSalvas + game->hud.moedas;
        const char *textoMoedas = TextFormat("MOEDAS: %d", visualMoedas);
        int larguraTexto = MeasureText(textoMoedas, fontSize);
        
        // Posiciona no canto direito com uma margem de 20px
        DrawText(textoMoedas, game->screenWidth - larguraTexto - 20, 35, fontSize, GOLD);

        // 3. Instruções de Teclas (Centro/Baixo do HUD)
        const char *ajuda = "Q: Salvar e Sair  |  ESC: Desistir (Perde moedas da fase)";
        DrawText(ajuda, game->screenWidth/2 - MeasureText(ajuda, 10)/2, 70, 10, LIGHTGRAY);


        // ---------------- OBSTÁCULOS ----------------
        DrawObstacles(game->obstaculos, 
                      game->cameraX, 
                      game->hudAltura, 
                      game->screenWidth, 
                      game->screenHeight, 
                      &game->obstTextures, 
                      game->carangueijoAnimFrame,
                      game->aguaVivaAnimFrame, 
                      game->baleiaAnimFrame,
                      game->tubaraoAnimFrame);

        // ---------------- MOEDAS ----------------
        Moeda *m = game->moedas;
        while (m) {
            float screenX = m->x - game->cameraX;
            float screenY = m->y;
            float scale = m->largura / game->moedaTexture.width;
            DrawTextureEx(game->moedaTexture, (Vector2){screenX, screenY}, 0.0f, scale, WHITE);
            m = m->next;
        }

        // ---------------- PLAYER ----------------
        float scalePeixe = (game->blocoTamanho * 0.6f) / game->playerTexture.height;
        Texture2D ptex = (game->playerAnimFrame == 0) ? game->playerTexture : game->playerTexture2;
        bool mangueCobreTela = false;
        if (game->mangueAtivo) {
            float screenX = game->mangueX - game->cameraX;
            if (screenX <= 0) {
                mangueCobreTela = true;   // ele já cobriu a área inteira
            }
        }
        if (!mangueCobreTela) {
            DrawTextureEx(ptex, (Vector2){game->player.x, game->player.y}, 0.0f, scalePeixe, WHITE);
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

// ------------------------
// Liberação
// ------------------------
void UnloadGame(Game *game) {
    UnloadTexture(game->playerTexture);
    UnloadTexture(game->playerTexture2);

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

    // Background
    UnloadTexture(game->bgMar);
    UnloadTexture(game->bgAreia);

    // Game over sprite
    UnloadTexture(game->gameOverTexture);
    // hud sprite
    UnloadTexture(game->hudTexture);
    //moeda
    UnloadTexture(game->moedaTexture);



    //musica
    UnloadMusicStream(game->musica);
    CloseAudioDevice();
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

    float mundoX = game->player.x + game->cameraX;

    // Hitbox REAL de movimento (full size)
    Rectangle atual = {
        mundoX,
        game->player.y,
        game->player.largura,
        game->player.altura
    };

    float speed = 260.0f;
    float delta = GetFrameTime();

    Rectangle futuro = atual;

    // MOVER PARA CIMA (pixel)
    if (IsKeyDown(KEY_W)) {
    futuro.y -= speed * delta;
    }

    // MOVER PARA BAIXO (pixel)
    if (IsKeyDown(KEY_S)) {
    futuro.y += speed * delta;
    }

    // Verificar colisão real (hitbox do sprite)
    Obstacle *hit = CheckCollisionPlayerObstacles(futuro, game->obstaculos);

    if (!hit) {
    game->player.y = futuro.y;
    }

    else if (hit->velocidade > 0) {
        // Morreu por obstáculo móvel: decrementar vida e mostrar overlay "game over"
        if (game->hud.vidas > 0) game->hud.vidas--;
        game->showGameOver = true;
        game->waitingForContinue = true; // aguardamos ESC para continuar
        return;
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

    int numObs;

    // MAIS OBSTÁCULOS CONFORME O NÍVEL
    if (colunaMovel) {
        
        // COLUNAS DE MAR (MÓVEIS)
        if (game->nivelSelecionado == 0) {
            numObs = 1;      // 1 bicho por coluna de mar
        }

        else if (game->nivelSelecionado == 1) {
            numObs = 2;      // 2 por coluna
        }

        else if (game->nivelSelecionado == 2) {
            numObs = 2;      // 3 por coluna
        }

        else {
            numObs = 2;      // 3 por coluna
        }
    }

    else {
        // COLUNAS DE AREIA (FIXOS)
        if (game->nivelSelecionado == 0) {
            numObs = 2;
        }

        else if (game->nivelSelecionado == 1) {
            numObs = 2;
        }

        else if (game->nivelSelecionado == 2) {
            numObs = 2;
        }
        
        else {
            numObs = 4;
        }
    }


    // CONTROLE DE DISTÂNCIA VERTICAL PARA OS MÓVEIS
    float posicoesGeradas[4];
    int gerados = 0;
    //float distanciaMinima = game->blocoTamanho * 0.9f;


    for (int i = 0; i < numObs; i++) {
        ObstaculoTipo tipo;
        float velocidade = 0.0f;
        int direcao = 0;
        float largura, altura, x, y;
        float distanciaMinima;

        if (colunaMovel) {
            // MÓVEIS
            int r;

            // Nível 1: apenas carangueijo
            if (game->nivelSelecionado == 0) {
                tipo = OBSTACULO_CARANGUEJO;
                velocidade = 60.0f; // velocidade do caranguejo
                largura = larguraBase * 0.9f;
                altura  = game->blocoTamanho * 0.8f;
            }
            // Nível 2: alterna por coluna de MAR (caranguejo / agua-viva)
            else if (game->nivelSelecionado == 1) {
                // Usar o índice da coluna para alternar entre os dois
                int indiceMar = worldColumnIndex / 2;  // só conta colunas de mar

                if (indiceMar % 2 == 0) {
                    tipo = OBSTACULO_CARANGUEJO;
                    velocidade = 60.0f;   // mais lento
                    largura = larguraBase * 0.9f;
                    altura  = game->blocoTamanho * 0.8f;
                } 

                else {
                    tipo = OBSTACULO_AGUA_VIVA;
                    velocidade = 80.0f;   // mais rápida
                    largura = larguraBase * 1.2f;
                    altura  = game->blocoTamanho * 0.9f;
                }
            }
            // Nível 3: alterna por coluna de MAR (carangueijo / agua-viva / baleia)
            else if (game->nivelSelecionado == 2) {
                int indiceMar = worldColumnIndex / 2;  // contamos só colunas de MAR
                int contador = indiceMar % 3;          // 0, 1, 2, 0, 1, 2...

                if (contador == 0) {
                    tipo = OBSTACULO_CARANGUEJO;
                    velocidade = 60.0f;
                    largura = larguraBase * 0.9f;
                    altura  = game->blocoTamanho * 0.8f;
                }

                else if (contador == 1) {
                    tipo = OBSTACULO_AGUA_VIVA;
                    velocidade = 80.0f;
                    largura = larguraBase * 1.2f;
                    altura  = game->blocoTamanho * 0.9f;
                }

                else {
                    tipo = OBSTACULO_BALEIA;
                    velocidade = 50.0f;   // mais lenta, mas grandona e perigosa
                    largura = larguraBase * 1.8f;
                    altura  = game->blocoTamanho * 1.4f;  // ocupa 2 blocos
                    
                }
            }
            // Nível 4: alterna por coluna de MAR (carangueijo / agua-viva / tubarão / baleia)
            else {
                int indiceMar = worldColumnIndex / 2;  
                int contador = indiceMar % 4;   // 0, 1, 2, 3, 0, 1...

                if (contador == 0) {
                    tipo = OBSTACULO_CARANGUEJO;
                    velocidade = 60.0f;
                    largura = larguraBase * 0.9f;
                    altura  = game->blocoTamanho * 0.8f;
                }
                
                else if (contador == 1) {
                    tipo = OBSTACULO_AGUA_VIVA;
                    velocidade = 80.0f;
                    largura = larguraBase * 1.2f;
                    altura  = game->blocoTamanho * 0.9f;
                }

                else if (contador == 2) {
                    tipo = OBSTACULO_BALEIA;
                    velocidade = 50.0f;   // lenta, porém gigante
                    largura = larguraBase * 1.8f;
                    altura  = game->blocoTamanho * 1.4f;  // ocupa 2 blocos
                    
                }

                else {
                    tipo = OBSTACULO_TUBARAO;
                    velocidade = 100.0f;   // rápido e perigoso
                    largura = larguraBase * 1.5f;
                    altura  = game->blocoTamanho * 1.1f;
                }
            }

            if (tipo == OBSTACULO_BALEIA) {
                distanciaMinima = altura * 1.5f;     // baleia precisa de MAIS espaço
            } 
            
            else if (tipo == OBSTACULO_TUBARAO) {
                distanciaMinima = altura * 1.25f;  // distância entre os tubarões
            }

            else {
                distanciaMinima = altura * 1.00f; // distância caranguejo e água viva
            }

            direcao = 1; // descendo

            
            float ajuste = 0.0f;
            // ajuste fino só para os maiores
            if (tipo == OBSTACULO_BALEIA) {
                ajuste = game->colunaLargura * 0.35f;
            }


            else if (tipo == OBSTACULO_TUBARAO) {
                ajuste = game->colunaLargura * 0.28f;
            }

            x = colunaX + (game->colunaLargura - largura) * 0.5f + ajuste;
        

            float zonaMin = topoHud + (game->blocoTamanho * 0.2f); 
            float zonaMax = limiteInferior - altura - (game->blocoTamanho * 0.2f);

            int tentativas = 0;
            bool valido = false;

            while (!valido && tentativas < 20) {
                y = zonaMin + (float)rand() / RAND_MAX * (zonaMax - zonaMin);
                valido = true;

                for (int k = 0; k < gerados; k++) {
                    float outraY = posicoesGeradas[k];

                    if ((y < outraY + altura + 5) && (y + altura + 5 > outraY)) {
                        valido = false;
                        break;
                    }
                }

                tentativas++;
            }

            posicoesGeradas[gerados++] = y;

        }

        else {
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

            largura = game->blocoTamanho * 0.55f;
            altura  = game->blocoTamanho * 0.55f;

            x = colunaX + (game->colunaLargura / 2) - (largura / 2);

            int tentativas = 0;
            bool valido = false;

            while (!valido && tentativas < 30) {
                int linha = rand() % game->linhas;
                y = topoHud + linha * game->blocoTamanho 
                    + (game->blocoTamanho / 2) 
                    - (altura / 2);

                y += game->blocoTamanho * 0.05f;

                valido = true;

                for (int i = 0; i < gerados; i++) {
                    if (fabsf(y - posicoesGeradas[i]) < distanciaMinima) {
                        valido = false;
                        break;
                    }
                }

                tentativas++;
            }

            if (!valido) break; 

            posicoesGeradas[gerados++] = y;
        }

        Obstacle *o = CreateObstacle(tipo, x, y, largura, altura, velocidade, direcao);
        AddObstacle(&game->obstaculos, o);
    }
// =================================================================
    //  REQUISITO OBRIGATÓRIO: USO DE MATRIZES E LAÇOS ANINHADOS
    // =================================================================
    
    // 1. Só tenta gerar moeda em colunas de Areia (fixas)
    //    E com 50% de chance (sorte)
    float sorte = (float)rand() / RAND_MAX;

    if (!colunaMovel && sorte < 0.50f) {

        // --- PASSO A: CRIAR E LIMPAR A MATRIZ ---
        // Matriz [Linhas][Status]:
        // Coluna 0 = Status (0=Livre, 1=Ocupado)
        // Coluna 1 = Reservado (ex: tipo do obstáculo)
        int matrizLogica[6][2]; 

        // Inicializa a matriz com zeros (Uso de laço aninhado for dentro de for)
        for (int l = 0; l < game->linhas; l++) {
            for (int c = 0; c < 2; c++) {
                matrizLogica[l][c] = 0; 
            }
        }

        // --- PASSO B: PREENCHER A MATRIZ COM OS OBSTÁCULOS EXISTENTES ---
        Obstacle *temp = game->obstaculos;
        while (temp != NULL) {
            // Verifica se o obstáculo está visualmente nesta coluna X
            if (fabsf(temp->x - colunaX) < game->colunaLargura * 0.5f) {
                
                // Transforma a posição Y (visual) em índice da Matriz (lógica)
                // Fórmula: (Y - HUD) / TamanhoBloco
                int linhaMatriz = (int)((temp->y - topoHud) / game->blocoTamanho);

                // Proteção de índice para não acessar memória inválida
                if (linhaMatriz >= 0 && linhaMatriz < game->linhas) {
                    matrizLogica[linhaMatriz][0] = 1; // Marca linha como OCUPADA (1)
                }
            }
            temp = temp->next;
        }

        // --- PASSO C: IDENTIFICAR LINHAS LIVRES NA MATRIZ ---
        int linhasLivres[6]; // Vetor auxiliar
        int qtdLivres = 0;

        for (int i = 0; i < game->linhas; i++) {
            // Se na matriz a linha i, coluna 0 for igual a 0 (Livre)
            if (matrizLogica[i][0] == 0) {
                linhasLivres[qtdLivres] = i;
                qtdLivres++;
            }
        }

        // --- PASSO D: CRIAR A MOEDA SE HOUVER ESPAÇO NA MATRIZ ---
        if (qtdLivres > 0) {
            // Sorteia um dos índices livres encontrados na matriz
            int indiceSorteado = rand() % qtdLivres;
            int linhaFinal = linhasLivres[indiceSorteado];

            // Converte de volta: Matriz -> Posição Visual (Pixels)
            float moedaL = game->blocoTamanho * 0.5f;
            float moedaA = game->blocoTamanho * 0.5f;
            
            float x = colunaX + (game->colunaLargura - moedaL) * 0.5f;
            float y = topoHud + linhaFinal * game->blocoTamanho + 
                      (game->blocoTamanho - moedaA) * 0.5f;

            Moeda *m = CreateMoeda(x, y, moedaL, moedaA);
            AddMoeda(&game->moedas, m);
        }
    }

} // <--- Fechamento da função SpawnColumn

Moeda* CreateMoeda(float x, float y, float largura, float altura) {
    Moeda *m = malloc(sizeof(Moeda));
    m->x = x;
    m->y = y;
    m->largura = largura;
    m->altura  = altura;
    m->hitbox = (Rectangle){x, y, largura, altura};
    m->next = NULL;
    return m;
}

void AddMoeda(Moeda **lista, Moeda *nova) {
    if (!*lista) {
        *lista = nova;
        return;
    }
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


