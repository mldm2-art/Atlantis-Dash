#include "raylib.h"
#include "game.h"

int main(void) {

    int screenWidth = 864;
    int screenHeight = 672;

    // Criar a janela com um tamanho qualquer (não importa)
    InitWindow(screenWidth, screenHeight, "Atlantis Dash");
    InitAudioDevice();
    
    SetExitKey(KEY_NULL);

    Game game = InitGame(screenWidth, screenHeight);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    UnloadGame(&game);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}