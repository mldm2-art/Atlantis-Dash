#include "raylib.h"

int main(void) {

    InitWindow(800, 600, "Atlantis Dash"); //inicializa a janela (função do raylib) com 800px de largura, 600px de altura e o título
    SetTargetFPS(60); //frames por segundo que o joigo vai rodar

    while (!WindowShouldClose()) { //while principal do jogo, dentro dele vai tudo que acontecerá até o player fechar a tela do raylib

        if (IsKeyPressed(KEY_ENTER)) {
            // Futuro: iniciar o jogo
        }

        // --- Desenho ---
        BeginDrawing(); //Início do desenho: a partir dessa função do raylib entra tudo oq será desenhado na nossa tela (textos, imagens, cores, bonecos)
            ClearBackground((Color){0, 105, 148, 255}); //define a cor de fundo

            int largura_total = GetScreenWidth(); //mantém a largura toda quando coloca fullscreen
            int altura_total = GetScreenHeight(); //mantém a altura toda quando coloca fullscreen

            DrawText("ATLANTIS DASH", //título
                     largura_total/2 - MeasureText("ATLANTIS DASH", 60)/2, 
                     altura_total/2 - 100,
                     60,
                     RAYWHITE); //cor do título

            DrawText("Pressione Enter para iniciar o jogo...",
                    largura_total/2 - MeasureText("Pressione Enter para iniciar o jogo...", 20)/2,
                    altura_total/2 + 50,
                    20,
                    RAYWHITE);

        EndDrawing(); //Fim do desenho
    }

    CloseWindow(); //fecha a janela
    return 0;
}