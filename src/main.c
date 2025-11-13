#include "raylib.h"
#include "game.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Configurações da janela
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    InitWindow(screenWidth, screenHeight, "Atlantis Dash");
    InitAudioDevice();

    // ESC não fecha a janela
    SetExitKey(KEY_NULL);

    Game game = InitGame(screenWidth, screenHeight);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    // Libera todos os recursos do jogo (texturas etc.)
    UnloadGame(&game);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}