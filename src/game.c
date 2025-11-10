#include "game.h"
#include <stdio.h>
#include <math.h>

Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    game.screenWidth = screenWidth;
    game.screenHeight = screenHeight;
    game.estado = MENU;

    // Grid
    game.linhas = 4;
    game.hudAltura = screenHeight * 0.15f;
    game.blocoTamanho = (screenHeight - game.hudAltura) / game.linhas;
    game.colunas = screenWidth / game.blocoTamanho;

    // Jogador
    game.player.linha = 2;
    game.player.coluna = 0;
    game.player.blocoTamanho = game.blocoTamanho;
    game.player.areaY = game.hudAltura;

    // Menu
    game.menuSelecionado = 0;

    // Obstáculos
    InitObstacles(game.obstaculos, game.colunas, game.linhas, &game.totalObstaculos);

    return game;
}

void UpdateGame(Game *game) {
    // Atualiza dimensões da janela
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
    }

    // ---------------- MENU ----------------
    if (game->estado == MENU) {
        if (IsKeyPressed(KEY_W) && game->menuSelecionado > 0) game->menuSelecionado--;
        if (IsKeyPressed(KEY_S) && game->menuSelecionado < 1) game->menuSelecionado++;
        if (IsKeyPressed(KEY_ENTER)) {
            if (game->menuSelecionado == 0) game->estado = JOGANDO;
            else TraceLog(LOG_INFO, "Mostrar instrucoes...");
        }
    }
    // ---------------- JOGANDO ----------------
    else if (game->estado == JOGANDO) {
        if (IsKeyPressed(KEY_W) && game->player.linha > 0) game->player.linha--;
        if (IsKeyPressed(KEY_S) && game->player.linha < game->linhas - 1) game->player.linha++;
        if (IsKeyPressed(KEY_D) && game->player.coluna < game->colunas - 1) game->player.coluna++;

        UpdateObstacles(game->obstaculos, game->totalObstaculos, game->blocoTamanho, game->hudAltura, game->linhas);

        // Colisão com obstáculos
        if (CheckCollisionPlayerObstacle(&game->player, game->obstaculos, game->totalObstaculos)) {
            game->player.linha = 2;
            game->player.coluna = 0;
        }
    }
}

void DrawGame(Game *game) {
    BeginDrawing();
    ClearBackground((Color){10, 30, 60, 255}); // fundo azul oceânico

    // ---------------- MENU ----------------
    if (game->estado == MENU) {
        const char *titulo = "ATLANTIS DASH";
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
    // ---------------- JOGANDO ----------------
    else if (game->estado == JOGANDO) {
        // HUD
        DrawRectangle(0, 0, game->screenWidth, game->hudAltura, (Color){20, 50, 80, 255});
        DrawText("HUD", 10, 10, 20, RAYWHITE);

        // Grid
        for (int l = 0; l < game->linhas; l++) {
            for (int c = 0; c < game->colunas; c++) {
                Rectangle bloco = {
                    c * game->blocoTamanho,
                    game->hudAltura + l * game->blocoTamanho,
                    game->blocoTamanho,
                    game->blocoTamanho
                };
                DrawRectangleLines(bloco.x, bloco.y, bloco.width, bloco.height, (Color){0, 0, 50, 100});
            }
        }

        // Obstáculos
        DrawObstacles(game->obstaculos, game->totalObstaculos, game->blocoTamanho, game->hudAltura);

        // Jogador
        float px = game->player.coluna * game->blocoTamanho + game->blocoTamanho / 4;
        float py = game->hudAltura + game->player.linha * game->blocoTamanho + game->blocoTamanho / 4;
        DrawRectangle(px, py, game->blocoTamanho / 2, game->blocoTamanho / 2, YELLOW);
    }

    EndDrawing();
}
