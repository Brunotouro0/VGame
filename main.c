/*
 * RIVER-INF Main Game Loop
 * INF01202 - Algoritmos e Programação
 * Based on Atari 2600 River Raid (1982) by Carol Shaw
 */

#include "game.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RIVER-INF - INF01202");
    SetTargetFPS(60);
    
    // Initialize game state
    GameData game;
    initialize_game(&game);
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Handle input
        handle_input(&game);
        
        // Update game logic
        update_game(&game);
        
        // Render
        render_game(&game);
    }
    
    // Cleanup
    CloseWindow();
    
    return 0;
}
