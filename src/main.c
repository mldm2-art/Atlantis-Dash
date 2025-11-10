#include "raylib.h"
#include "game.h"

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Atlantis Dash");
    InitAudioDevice();

    Game game = InitGame(screenWidth, screenHeight);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        UpdateGame(&game);
        DrawGame(&game);
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
