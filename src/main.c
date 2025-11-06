#include "raylib.h" //biblioteca gráfica
#include "game.h" //header das regras do jogo

// Função principal: ponto de entrada do jogo
int main(void) {
    // Define o tamanho da janela
    const int screenWidth = 800;
    const int screenHeight = 600;

    // Inicializa a janela Raylib
    InitWindow(screenWidth, screenHeight, "Atlantis Dash");

    // Inicializa o sistema de áudio (mesmo que ainda não use sons)
    InitAudioDevice();

    // Criação de uma variável 'game' do tipo 'Game'(struct que está em game.h) e iguala ela a chamada da função 'InitGame'(que está em game.c)
    Game game = InitGame(screenWidth, screenHeight);

    // Define o FPS (frames por segundo)
    SetTargetFPS(60);

    // Loop principal do jogo: roda até a janela ser fechada
    while (!WindowShouldClose()) {
        // Atualiza a lógica do jogo (movimentos, estados, etc.). Essa função está em
        UpdateGame(&game);

        // Desenha tudo na tela (menu, grid, jogador, etc.)
        DrawGame(&game);
    }

    // Encerra o sistema de áudio e fecha a janela
    CloseAudioDevice();
    CloseWindow();
    return 0;
}