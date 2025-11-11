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

    // Carrega sprite do jogador
    game.playerTexture = LoadTexture("assets/imgs/personagemsprite.png");

    // Menu
    game.menuSelecionado = 0;
    game.nivelSelecionado = 0;

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
            if (game->menuSelecionado == 0) game->estado = SELECAO_NIVEL;
            else if(game->menuSelecionado == 1) game->estado = INSTRUCOES;
        }
    }

        // ---------------- INSTRUÇÕES ----------------
    else if (game->estado == INSTRUCOES) {
        // Volta para o menu ao apertar ESC ou ENTER
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = MENU;
        }
    }

        // ---------------- SELEÇÃO DE NÍVEL ----------------
    else if (game->estado == SELECAO_NIVEL) {
        // Navega entre os 4 níveis
        if (IsKeyPressed(KEY_W) && game->nivelSelecionado > 0) game->nivelSelecionado--;
        if (IsKeyPressed(KEY_S) && game->nivelSelecionado < 3) game->nivelSelecionado++;

        // ENTER inicia o jogo com o nível selecionado
        if (IsKeyPressed(KEY_ENTER)) {
            game->estado = JOGANDO;
            // Aqui no futuro você pode configurar a dificuldade de acordo com o nível
            // Exemplo: ajustar velocidade de obstáculos, quantidade, etc.
        }

        // Tecla ESC volta ao menu
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = MENU;
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
        // ESC volta à tela de seleção de nível
        if (IsKeyPressed(KEY_ESCAPE)) {
            game->estado = SELECAO_NIVEL;
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

        // ---------------- INSTRUÇÕES ----------------
    else if (game->estado == INSTRUCOES) {
        ClearBackground((Color){5, 25, 45, 255}); // Fundo azul escuro oceânico

        const char *titulo = "INSTRUCOES DO JOGO";
        DrawText(titulo,
                 game->screenWidth / 2 - MeasureText(titulo, 50) / 2,
                 80, 50, SKYBLUE);

        const char *texto1 = "Bem-vindo a ATLANTIS DASH!";
        const char *texto2 = "Você é um peixe tentando cruzar os mares de Atlântida.";
        const char *texto3 = "Desvie de obstáculos e avance o máximo que conseguir!";

        int fontSize = 22;
        DrawText(texto1, game->screenWidth / 2 - MeasureText(texto1, fontSize) / 2, 180, fontSize, RAYWHITE);
        DrawText(texto2, game->screenWidth / 2 - MeasureText(texto2, fontSize) / 2, 210, fontSize, RAYWHITE);
        DrawText(texto3, game->screenWidth / 2 - MeasureText(texto3, fontSize) / 2, 240, fontSize, RAYWHITE);

        // Controles
        DrawText("CONTROLES:", game->screenWidth / 2 - MeasureText("CONTROLES:", 25) / 2, 310, 25, YELLOW);
        DrawText("W - Mover para cima", game->screenWidth / 2 - 150, 350, 20, LIGHTGRAY);
        DrawText("S - Mover para baixo", game->screenWidth / 2 - 150, 380, 20, LIGHTGRAY);
        DrawText("D - Mover para frente", game->screenWidth / 2 - 150, 410, 20, LIGHTGRAY);
        DrawText("ESC - Voltar ao menu", game->screenWidth / 2 - 150, 440, 20, LIGHTGRAY);

        // Mensagem piscando
        const char *msg = "Pressione ESC ou ENTER para voltar";
        float alpha = (sin(GetTime() * 3) + 1) / 2;
        DrawText(msg,
                 game->screenWidth / 2 - MeasureText(msg, 18) / 2,
                 game->screenHeight - 80, 18,
                 Fade(RAYWHITE, 0.6f + 0.4f * alpha));
    }

    
    // ---------------- SELEÇÃO DE NÍVEL ----------------
    else if (game->estado == SELECAO_NIVEL) {
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

        const char *msg = "ENTER para iniciar  |  ESC para voltar";
        DrawText(msg, game->screenWidth / 2 - MeasureText(msg, 20) / 2, game->screenHeight - 80, 20, Fade(RAYWHITE, 0.8f));
    }

    // ---------------- JOGANDO ----------------
    else if (game->estado == JOGANDO) {
        // HUD
        DrawRectangle(0, 0, game->screenWidth, game->hudAltura, (Color){20, 50, 80, 255});
        DrawText(TextFormat("Nivel %d", game->nivelSelecionado + 1), 10, 10, 20, RAYWHITE);

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

        // ✅ Jogador com sprite
        float px = game->player.coluna * game->blocoTamanho + game->blocoTamanho / 4;
        float py = game->hudAltura + game->player.linha * game->blocoTamanho + game->blocoTamanho / 4;

        DrawTextureEx(
            game->playerTexture,
            (Vector2){ px, py },
            0.0f,
            game->blocoTamanho / 96.0f, // escala baseada no tamanho original do sprite
            WHITE
        );
    }

    EndDrawing();
}

// Libera os recursos do jogo

void UnloadGame(Game *game) {
    UnloadTexture(game->playerTexture);
}