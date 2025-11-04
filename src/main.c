// Este código VAI em src/main.c
#include "raylib.h"


// Inclui os "Cardápios" (.h). Não faz mal se eles estiverem vazios por agora.
#include "player.h"
#include "enemy.h"
#include "game.h"

// A "PORTA DE ENTRADA" (o 'main' que estava a faltar)
int main(void)
{
    // --- INICIALIZAÇÃO ---
    InitWindow(800, 450, "Atlantis Dash - FUNCIONOU!");
    SetTargetFPS(60);

    // --- O LOOP DO JOGO ---
    while (!WindowShouldClose())
    {
        // --- Lógica (ainda vazia) ---
        
        // --- Desenhar ---
        BeginDrawing();

            ClearBackground(BLACK); // Limpa a tela
            
            // Este texto prova que o seu Makefile e o main.c estão a funcionar juntos!
            DrawText("O JOGO ESTA A RODAR!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    // --- FINALIZAÇÃO ---
    CloseWindow();
    return 0;
}