#include "raylib.h"
#include "game.h"

int main(void) {

    // Criar a janela com um tamanho qualquer (não importa)
    InitWindow(800, 600, "Atlantis Dash");
    InitAudioDevice();

    // Agora sim → colocar fullscreen REAL
    SetWindowState(FLAG_FULLSCREEN_MODE);

    // Atualiza resolução REAL da tela
    int monitor = GetCurrentMonitor();
    int screenWidth  = GetMonitorWidth(monitor);
    int screenHeight = GetMonitorHeight(monitor);

    // ESC não fecha a janela
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