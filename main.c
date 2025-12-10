#include "game.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RIVER-INF - INF01202");

    InitAudioDevice();

    // --- CARREGAMENTO E VERIFICAÇÃO DE ÁUDIO ---

    // 1. Música de Fundo
    Music musicaFundo = LoadMusicStream("recursos/musicaFundo.mp3");

    // 2. Som de Tiro
    Sound somTiro = LoadSound("recursos/tiro.wav");

    // 3. Som de Game Over
    Sound somGameOver = LoadSound("recursos/somGameOver.mp3"); // Use .mp3 se tiver convertido

    // 4. Som de Vitória (NOVO)
    Sound somGameWon = LoadSound("recursos/somGameWon.mp3"); // Certifique-se de ter este arquivo

    // Ajuste de Volume
    SetMusicVolume(musicaFundo, 0.2f);
    SetSoundVolume(somTiro, 0.2f);
    SetSoundVolume(somGameOver, 0.2f);
    SetSoundVolume(somGameWon, 0.2f);

    SetTargetFPS(60);

    GameData game;

    // --- CARREGAMENTO DE SPRITES ---
    game.texPlayer = LoadTexture("recursos/aviao.png");
    game.texShip   = LoadTexture("recursos/navio.png");
    game.texHeli   = LoadTexture("recursos/helicoptero.png");
    game.texFuel   = LoadTexture("recursos/combustivel.png");
    game.texBridge = LoadTexture("recursos/ponte.png");
    game.texBullet = LoadTexture("recursos/tiro.png");

    initialize_game(&game);

    while (!WindowShouldClose()) {

        // --- GERENCIAMENTO DE ÁUDIO POR FLAGS ---
        if (game.trigger_music_start) {
            StopMusicStream(musicaFundo);
            PlayMusicStream(musicaFundo);
            game.trigger_music_start = false;
        }

        if (game.trigger_music_stop) {
            StopMusicStream(musicaFundo);
            game.trigger_music_stop = false;
        }

        // Tocar som de Game Over
        if (game.trigger_gameover_sound) {
            PlaySound(somGameOver);
            game.trigger_gameover_sound = false;
        }

        // --- NOVO: Tocar som de Vitória ---
        if (game.trigger_gamewon_sound) {
            PlaySound(somGameWon);
            game.trigger_gamewon_sound = false;
        }

        if (IsMusicStreamPlaying(musicaFundo)) {
            UpdateMusicStream(musicaFundo);
        }

        // --- LOOP DO JOGO ---
        handle_input(&game);
        update_game(&game);

        if (game.trigger_shoot_sound) {
            PlaySound(somTiro);
            game.trigger_shoot_sound = false;
        }

        render_game(&game);
    }

    // --- LIMPEZA DE MEMÓRIA (Unload) ---
    UnloadTexture(game.texPlayer);
    UnloadTexture(game.texShip);
    UnloadTexture(game.texHeli);
    UnloadTexture(game.texFuel);
    UnloadTexture(game.texBridge);
    UnloadTexture(game.texBullet);

    UnloadMusicStream(musicaFundo);
    UnloadSound(somTiro);
    UnloadSound(somGameOver);
    UnloadSound(somGameWon); // --- NOVO: Descarregar som ---

    CloseAudioDevice();
    CloseWindow();

    return 0;
}
