#include "game.h"
#include <stdio.h>
#include <math.h>

static void GenerateColumnPattern(Game *game);
static void ClampCamera(Game *game);
static void UpdatePlayerColumn(Game *game);
static void ResetPlayer(Game *game);

Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    game.screenWidth = screenWidth;
    game.screenHeight = screenHeight;
    game.estado = MENU;

    game.linhas = 7;
    game.hudAltura = screenHeight * 0.15f;
    game.blocoTamanho = (screenHeight - game.hudAltura) / game.linhas;
    game.colunas = screenWidth / game.blocoTamanho;

    game.worldColumns = 200;
    game.cameraColumn = 0.0f;
    game.playerColumnOffset = 2.0f;

    GenerateColumnPattern(&game);

    game.player.blocoTamanho = game.blocoTamanho;
    game.player.areaY = game.hudAltura;

    game.playerTexture     = LoadTexture("assets/imgs/personagemsprite.png");
    game.backgroundTexture = LoadTexture("assets/imgs/menu_jogo.png");

    game.menuSelecionado = 0;
    game.nivelSelecionado = 0;
    game.vidas = 3;

    ResetPlayer(&game);

    InitObstacles(game.obstaculos, game.columnTypes, game.worldColumns, game.linhas, &game.totalObstaculos);

    return game;
}

void UpdateGame(Game *game) {
    int newWidth = GetScreenWidth();
    int newHeight = GetScreenHeight();

    if (newWidth != game->screenWidth || newHeight != game->screenHeight) {
        game->screenWidth = newWidth;
        game->screenHeight = newHeight;
        game->hudAltura = newHeight * 0.15f;
        game->blocoTamanho = (newHeight - game->hudAltura) / game->linhas;
        game->colunas = newWidth / game->blocoTamanho;
        game->player.blocoTamanho = game->blocoTamanho;
        game->player.areaY = game->hudAltura;
        ClampCamera(game);
        UpdatePlayerColumn(game);
    }

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
            game->vidas = 3;
            ResetPlayer(game);
            InitObstacles(game->obstaculos, game->columnTypes, game->worldColumns, game->linhas, &game->totalObstaculos);
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = MENU;
        }
    }

    else if (game->estado == JOGANDO) {
        if (IsKeyPressed(KEY_W) && game->player.linha > 0) game->player.linha--;
        if (IsKeyPressed(KEY_S) && game->player.linha < game->linhas - 1) game->player.linha++;

        const float scrollSpeed = 5.0f; // colunas por segundo
        if (IsKeyDown(KEY_D)) {
            game->cameraColumn += scrollSpeed * GetFrameTime();
        }

        ClampCamera(game);
        UpdatePlayerColumn(game);

        UpdateObstacles(game->obstaculos, game->totalObstaculos, game->linhas);

        if (CheckCollisionPlayerObstacle(&game->player, game->obstaculos, game->totalObstaculos)) {
            if (game->vidas > 0) game->vidas--;

            if (game->vidas > 0) {
                ResetPlayer(game);
            } else {
                game->vidas = 3;
                ResetPlayer(game);
                InitObstacles(game->obstaculos, game->columnTypes, game->worldColumns, game->linhas, &game->totalObstaculos);
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = SELECAO_NIVEL;
            ResetPlayer(game);
        }
    }
}

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

        const char *opcoes[2] = {"Jogar", "Instruções do jogo"};
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

        const char *titulo = "INSTRUÇÕES DO JOGO";
        DrawText(titulo,
                 w / 2 - MeasureText(titulo, h * 0.05f) / 2,
                 (int)(h * 0.08f), (int)(h * 0.05f), SKYBLUE);

        int fontSize = (int)(h * 0.022f);
        int posY = (int)(h * 0.18f);

        const char *intro[] = {
            "Bem-vindo às profundezas do Atlantis-Dash!",
            "Nade entre corredores de areia e água em Atlântida.",
            "Desvie de animais perigosos e corais enquanto avança.",
            "Use seus reflexos para chegar o mais longe possível!"
        };

        for (int i = 0; i < 4; i++) {
            DrawText(intro[i],
                     w / 2 - MeasureText(intro[i], fontSize) / 2,
                     posY + i * (fontSize + 6),
                     fontSize, RAYWHITE);
        }

        int leftX = (int)(w * 0.04f);
        int rightX = (int)(w * 0.55f);

        int baseY = (int)(h * 0.42f);
        int line = fontSize + 6;

        DrawText("OBJETIVO PRINCIPAL:", leftX, baseY, fontSize + 4, YELLOW);
        DrawText("Chegar vivo ao final de cada um dos quatro níveis, desviando de obstáculos",
                 leftX, baseY + line, fontSize, RAYWHITE);
        DrawText("e inimigos enquanto coleta o maior número possível de moedas.",
                 leftX, baseY + 2 * line, fontSize, RAYWHITE);
        DrawText("OBS: O próximo nível só será liberado quando o nível anterior for concluído.",
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
        DrawText("OBSTÁCULOS:", rightX, ObstacleY, fontSize + 4, YELLOW);
        DrawText("No Atlantis-Dash existem dois tipos de obstáculos: os fixos e os móveis.",
                 rightX, ObstacleY + line, fontSize, RAYWHITE);
        DrawText("Ao colidir com um obstáculo fixo, o seu personagem não consegue avançar.",
                 rightX, ObstacleY + 2 * line, fontSize, RAYWHITE);
        DrawText("Já ao colidir com obstáculos móveis, o personagem perde uma vida.", 
                 rightX, ObstacleY + 3 * line, fontSize, RAYWHITE);

        int RankingY = baseY + 11 * line;
        DrawText("RANKING:", rightX, RankingY, fontSize + 4, YELLOW);
        DrawText("O ranking do jogo funcionará da seguinte forma:",
                 rightX, RankingY + line, fontSize, RAYWHITE);
        DrawText("Quanto menor o tempo que você completa os níveis e quanto mais moedas",
                 rightX, RankingY + 2 * line, fontSize, RAYWHITE);
        DrawText("você coleta, maior será sua pontuação naquele nível.", 
                 rightX, RankingY + 3 * line, fontSize, RAYWHITE);
        DrawText("OBS: todas as moedas possuem o mesmo valor de pontuação", 
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
            game->backgroundTexture,
            (Rectangle){0, 0, game->backgroundTexture.width, game->backgroundTexture.height},
            (Rectangle){0, 0, (float)game->screenWidth, (float)game->screenHeight},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        const char *titulo = "SELECIONE O NIVEL";
        DrawText(titulo, game->screenWidth / 2 - MeasureText(titulo, 50) / 2, 100, 50, SKYBLUE);

        const char *niveis[4] = {
            "Nivel 1 - Recifes Rasos",
            "Nivel 2 - Caverna Coralina",
            "Nivel 3 - Passagem do Tubarao",
            "Nivel 4 - Abismo Final"
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

        DrawRectangle(0, 0, game->screenWidth, game->hudAltura, (Color){20, 50, 80, 255});
        DrawText(TextFormat("Nivel %d", game->nivelSelecionado + 1), 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Vidas: %d", game->vidas), 150, 10, 20, RAYWHITE);

        Color sandColor  = (Color){194, 178, 128, 255};
        Color waterColor = (Color){8, 24, 48, 255};
        Color gridColor  = (Color){0, 0, 50, 120};

        int firstCol = (int)floorf(game->cameraColumn);
        float frac = game->cameraColumn - (float)firstCol;
        int visibleCols = (int)ceilf((float)game->screenWidth / game->blocoTamanho) + 2;
        float totalHeight = game->linhas * game->blocoTamanho;

        for (int i = 0; i < visibleCols; i++) {
            int colIndex = firstCol + i;
            if (colIndex < 0 || colIndex >= game->worldColumns) continue;

            float x = ((float)i - frac) * game->blocoTamanho;
            Color laneColor = (game->columnTypes[colIndex] == COLUMN_SAND) ? sandColor : waterColor;
            DrawRectangleRec((Rectangle){x, game->hudAltura, game->blocoTamanho + 1.0f, totalHeight}, laneColor);
        }

        for (int i = 0; i <= visibleCols; i++) {
            float x = ((float)i - frac) * game->blocoTamanho;
            if (x < -game->blocoTamanho || x > game->screenWidth + game->blocoTamanho) continue;
            DrawLine((int)x, (int)game->hudAltura, (int)x, (int)(game->hudAltura + totalHeight), gridColor);
        }

        for (int r = 0; r <= game->linhas; r++) {
            float y = game->hudAltura + r * game->blocoTamanho;
            DrawLine(0, (int)y, game->screenWidth, (int)y, gridColor);
        }

        DrawObstacles(game->obstaculos, game->totalObstaculos, game->blocoTamanho, game->hudAltura, game->cameraColumn, game->screenWidth);

        float playerScreenX = (game->player.coluna - game->cameraColumn) * game->blocoTamanho + game->blocoTamanho * 0.25f;
        float playerScreenY = game->hudAltura + game->player.linha * game->blocoTamanho + game->blocoTamanho * 0.25f;

        DrawTextureEx(
            game->playerTexture,
            (Vector2){playerScreenX, playerScreenY},
            0.0f,
            game->blocoTamanho / 96.0f,
            WHITE
        );
    }

    EndDrawing();
}

void UnloadGame(Game *game) {
    UnloadTexture(game->playerTexture);
    UnloadTexture(game->backgroundTexture);
}

// ----------------------------------------------------------------
// Helpers internos
static void GenerateColumnPattern(Game *game) {
    for (int c = 0; c < game->worldColumns; c++) {
        if (c < 4) {
            game->columnTypes[c] = COLUMN_SAFE;
        } else {
            int mod = c % 4;
            game->columnTypes[c] = (mod < 2) ? COLUMN_SAFE : COLUMN_SAND;
        }
    }
}

static void ClampCamera(Game *game) {
    float visibleCols = (float)game->screenWidth / game->blocoTamanho;
    if (visibleCols < 1.0f) visibleCols = 1.0f;

    float maxByView = (float)game->worldColumns - visibleCols;
    float maxByPlayer = (float)game->worldColumns - (game->playerColumnOffset + 1.0f);
    float maxCamera = fminf(maxByView, maxByPlayer);
    if (maxCamera < 0.0f) maxCamera = 0.0f;

    if (game->cameraColumn > maxCamera) game->cameraColumn = maxCamera;
    if (game->cameraColumn < 0.0f) game->cameraColumn = 0.0f;
}

static void UpdatePlayerColumn(Game *game) {
    game->player.coluna = game->cameraColumn + game->playerColumnOffset;
    float maxCol = (float)game->worldColumns - 0.4f;
    if (game->player.coluna > maxCol) game->player.coluna = maxCol;
    if (game->player.coluna < 0.0f) game->player.coluna = 0.0f;
}

static void ResetPlayer(Game *game) {
    game->player.linha = game->linhas - 4;
    if (game->player.linha < 0) game->player.linha = game->linhas - 1;
    game->cameraColumn = 0.0f;
    ClampCamera(game);
    UpdatePlayerColumn(game);
}