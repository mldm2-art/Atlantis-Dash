// src/game.c
#include "game.h"
#include <stdio.h>

// ----------------------------------------------------------
// Função: InitGame()
// ----------------------------------------------------------
// Inicializa o estado inicial do jogo e define medidas do grid
// ----------------------------------------------------------
Game InitGame(int screenWidth, int screenHeight) {
    Game game = {0};

    game.screenWidth = screenWidth;
    game.screenHeight = screenHeight;
    game.estado = MENU; // o jogo começa no menu

    // Parâmetros do grid (para depois do menu)
    game.linhas = 4;
    game.hudAltura = screenHeight * 0.15f;
    game.blocoTamanho = (screenHeight - game.hudAltura) / game.linhas;
    game.colunas = screenWidth / game.blocoTamanho;

    // Posição inicial do jogador
    game.player.linha = 2;
    game.player.coluna = 0;
    game.player.blocoTamanho = game.blocoTamanho;
    game.player.areaY = game.hudAltura;

    // Variável de seleção do menu (índice do botão)
    game.menuSelecionado = 0; // 0 = Jogar, 1 = Instruções

    return game;
}

// ----------------------------------------------------------
// Função: UpdateGame()
// ----------------------------------------------------------
// Atualiza a lógica do jogo a cada frame.
// - Detecta mudanças de tamanho da janela (grid adaptável)
// - Controla navegação no menu
// - Atualiza movimento no estado "JOGANDO"
// ----------------------------------------------------------
void UpdateGame(Game *game) {
    // Atualiza dimensões da janela (para adaptação)
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

    // ---------------- MENU PRINCIPAL ----------------
    if (game->estado == MENU) {
        // Alterna entre as opções do menu com W (para cima) e S (para baixo)
        if (IsKeyPressed(KEY_W) && game->menuSelecionado > 0)
            game->menuSelecionado--;
        if (IsKeyPressed(KEY_S) && game->menuSelecionado < 1)
            game->menuSelecionado++;

        // Pressionar ENTER seleciona a opção atual
        if (IsKeyPressed(KEY_ENTER)) {
            if (game->menuSelecionado == 0) {
                game->estado = JOGANDO; // começa o jogo
            } else {
                // Aqui poderíamos abrir uma tela de instruções futuramente
                TraceLog(LOG_INFO, "Mostrar instruções...");
            }
        }
    }

    // ---------------- ESTADO JOGANDO ----------------
    else if (game->estado == JOGANDO) {
        if (IsKeyPressed(KEY_W) && game->player.linha > 0)
            game->player.linha--;
        if (IsKeyPressed(KEY_S) && game->player.linha < game->linhas - 1)
            game->player.linha++;
        if (IsKeyPressed(KEY_D) && game->player.coluna < game->colunas - 1)
            game->player.coluna++;
    }
}

// ----------------------------------------------------------
// Função: DrawGame()
// ----------------------------------------------------------
// Desenha o conteúdo do jogo conforme o estado atual.
// ----------------------------------------------------------
void DrawGame(Game *game) {
    BeginDrawing();
    ClearBackground((Color){10, 30, 60, 255}); // fundo azul oceânico

    // ------------------------- MENU -------------------------
    if (game->estado == MENU) {
        // 🐠 Título principal
        const char *titulo = "ATLANTIS DASH";
        DrawText(titulo,
                 game->screenWidth / 2 - MeasureText(titulo, 60) / 2,
                 120, 60, SKYBLUE);

        // Cria um pequeno brilho no título (efeito de pulsar)
        float brilho = (sin(GetTime() * 2) + 1) * 0.5f; // oscila entre 0 e 1
        Color corBrilho = Fade(WHITE, 0.4f + 0.4f * brilho);
        DrawText(titulo,
                 game->screenWidth / 2 - MeasureText(titulo, 60) / 2,
                 120, 60, corBrilho);

        // 🧭 Botões do menu
        const char *opcoes[2] = {"Jogar", "Instruções do jogo"};
        int tamanhoFonte = 30;
        int espacamento = 70; // distância entre botões
        int baseY = game->screenHeight / 2 - espacamento / 2;

        for (int i = 0; i < 2; i++) {
            int textWidth = MeasureText(opcoes[i], tamanhoFonte);
            int posX = game->screenWidth / 2 - textWidth / 2;
            int posY = baseY + i * espacamento;

            // Se a opção estiver selecionada, cria um efeito de brilho pulsante
            if (i == game->menuSelecionado) {
                float alpha = (sin(GetTime() * 4) + 1) / 2; // efeito de piscar
                Color glow = Fade(YELLOW, 0.5f + 0.5f * alpha);

                // Caixa atrás do texto (leve destaque)
                DrawRectangle(posX - 20, posY - 10, textWidth + 40, 50, Fade(BLUE, 0.3f));
                DrawText(opcoes[i], posX, posY, tamanhoFonte, glow);
            } else {
                // Opção não selecionada → texto cinza claro
                DrawText(opcoes[i], posX, posY, tamanhoFonte, LIGHTGRAY);
            }
        }

        // ✨ Mensagem inferior de instrução
        const char *msg = "Aperte ENTER para selecionar";
        float alpha = (sin(GetTime() * 3) + 1) / 2; // brilho oscilante
        DrawText(msg,
                 game->screenWidth / 2 - MeasureText(msg, 20) / 2,
                 game->screenHeight - 80, 20,
                 Fade(RAYWHITE, 0.6f + 0.4f * alpha));

        // TODO: futuramente adicionar uma textura de fundo animado com DrawTexture()
    }

    // ------------------------- JOGO -------------------------
    else if (game->estado == JOGANDO) {
        // Desenha o HUD (faixa superior)
        DrawRectangle(0, 0, game->screenWidth, game->hudAltura, (Color){20, 50, 80, 255});
        DrawText("HUD", 10, 10, 20, RAYWHITE);

        // Desenha o grid (linhas de debug)
        for (int l = 0; l < game->linhas; l++) {
            for (int c = 0; c < game->colunas; c++) {
                Rectangle bloco = {
                    c * game->blocoTamanho,
                    game->hudAltura + l * game->blocoTamanho,
                    game->blocoTamanho,
                    game->blocoTamanho
                };
                DrawRectangleLines(bloco.x, bloco.y, bloco.width, bloco.height,
                                   (Color){0, 0, 50, 100});
            }
        }

        // Desenha o jogador (quadrado amarelo)
        float px = game->player.coluna * game->blocoTamanho + game->blocoTamanho / 4;
        float py = game->hudAltura + game->player.linha * game->blocoTamanho + game->blocoTamanho / 4;
        DrawRectangle(px, py, game->blocoTamanho / 2, game->blocoTamanho / 2, YELLOW);
    }

    EndDrawing();
}
