/*
 * Rendering and Display System
 */

#include "game.h"
#include <stdio.h>

/**
 * Main render function
 */
void render_game(GameData* game) {
    BeginDrawing();
    ClearBackground(DARKBLUE);
    
    switch (game->state) {
        case STATE_MENU:
            render_menu(game);
            break;
        
        case STATE_PLAYING:
        case STATE_PAUSED:
            render_map(game);
            render_enemies(game);
            render_bullets(game);
            render_player(game);
            render_hud(game);
            
            if (game->paused) {
                // Darken screen
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
                            (Color){0, 0, 0, 128});
                DrawText("PAUSED", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 20, 
                        40, YELLOW);
                DrawText("Press ENTER to continue", SCREEN_WIDTH/2 - 100, 
                        SCREEN_HEIGHT/2 + 30, 20, WHITE);
            }
            break;
        
        case STATE_GAME_OVER:
            render_game_over(game);
            break;

        case STATE_HIGH_SCORE:
            render_high_score(game);
            break;
        
        case STATE_LEVEL_COMPLETE:
            render_level_complete(game);
            break;
        
        default:
            break;
    }
    
    EndDrawing();
}

/**
 * Render main menu
 */
void render_menu(GameData* game) {
    DrawText("RIVER-INF", SCREEN_WIDTH/2 - 100, 80, 60, YELLOW);
    DrawText("Inspired by River Raid (1982)", SCREEN_WIDTH/2 - 150, 160, 20, LIGHTGRAY);
    
    // Draw menu options
    Color option1_color = (game->menu_selected == 0) ? YELLOW : WHITE;
    Color option2_color = (game->menu_selected == 1) ? YELLOW : WHITE;
    
    DrawText("START GAME", SCREEN_WIDTH/2 - 80, 300, 30, option1_color);
    DrawText("QUIT", SCREEN_WIDTH/2 - 50, 360, 30, option2_color);
    
    // Draw high scores
    DrawText("HIGH SCORES:", 50, 480, 20, LIGHTGRAY);
    for (int i = 0; i < game->num_highscores && i < 5; i++) {
        char score_text[64];
        sprintf(score_text, "%d. %s - %d", i+1, game->highscores[i].name, 
                game->highscores[i].score);
        DrawText(score_text, 50, 510 + i*30, 16, WHITE);
    }
    
    DrawText("Use UP/DOWN arrows to select, ENTER to confirm", SCREEN_WIDTH/2 - 200, 
            SCREEN_HEIGHT - 40, 14, GRAY);
}

/**
 * Render HUD (score and fuel)
 */
void render_hud(GameData* game) {
    char hud_text[128];
    
    // Score
    sprintf(hud_text, "SCORE: %d", game->player.score);
    DrawText(hud_text, 10, 10, 20, YELLOW);
    
    // Fuel
    sprintf(hud_text, "FUEL: %d", game->player.fuel);
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, 10, 20, 
            (game->player.fuel < 20) ? RED : LIME);
    
    // Lives
    sprintf(hud_text, "LIVES: %d", game->player.lives);
    DrawText(hud_text, SCREEN_WIDTH - 150, 10, 20, RED);
    
    // Phase
    sprintf(hud_text, "PHASE: %d", game->current_phase);
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT - 30, 16, WHITE);
}

/**
 * Render map (terrain and obstacles)
 */
void render_map(GameData* game) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char cell = game->map[y][x];
            int px = x * CELL_SIZE;
            int py = y * CELL_SIZE;
            
            if (cell == 'T') {
                // Forest/Terrain
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, DARKGREEN);
            } else if (cell == 'G') {
                // Fuel station
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, ORANGE);
                DrawText("F", px + 12, py + 10, 20, WHITE);
            } else if (cell == 'P') {
                // Bridge
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, BROWN);
            }
            // Empty space (river) = DARKBLUE (already background)
        }
    }
}

/**
 * Render player
 */
void render_player(GameData* game) {
    int px = (int)(game->player.x * CELL_SIZE);
    int py = (int)(game->player.y * CELL_SIZE);
    
    // Draw aircraft as a triangle pointing up
    Vector2 v1 = {px + CELL_SIZE/2, py + 5};
    Vector2 v2 = {px + 5, py + CELL_SIZE - 5};
    Vector2 v3 = {px + CELL_SIZE - 5, py + CELL_SIZE - 5};
    
    DrawTriangle(v1, v2, v3, YELLOW);
    DrawTriangleLines(v1, v2, v3, WHITE);
}

/**
 * Render enemies
 */
void render_enemies(GameData* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) continue;
        
        int ex = (int)(game->enemies[i].x * CELL_SIZE);
        int ey = (int)(game->enemies[i].y * CELL_SIZE);
        int width = game->enemies[i].width * CELL_SIZE;
        
        switch (game->enemies[i].type) {
            case ENTITY_SHIP:
                // Draw as rectangle
                DrawRectangle(ex, ey, CELL_SIZE, CELL_SIZE, RED);
                DrawRectangleLines(ex, ey, CELL_SIZE, CELL_SIZE, RED);
                DrawText("N", ex + 12, ey + 10, 20, WHITE);
                break;
            
            case ENTITY_HELICOPTER:
                // Draw as circle
                DrawCircle(ex + CELL_SIZE/2, ey + CELL_SIZE/2, CELL_SIZE/2 - 2, RED);
                DrawCircleLines(ex + CELL_SIZE/2, ey + CELL_SIZE/2, CELL_SIZE/2 - 2, RED);
                DrawText("X", ex + 10, ey + 5, 20, WHITE);
                break;
            
            case ENTITY_BRIDGE_PIECE:
                // Draw as dark rectangle
                DrawRectangle(ex, ey, CELL_SIZE, CELL_SIZE, DARKGRAY);
                DrawRectangleLines(ex, ey, CELL_SIZE, CELL_SIZE, BLACK);
                break;
            
            case ENTITY_FUEL_STATION:
                // Already drawn in map
                break;
        }
    }
}

/**
 * Render bullets
 */
void render_bullets(GameData* game) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) continue;
        
        int bx = (int)(game->bullets[i].x * CELL_SIZE) + CELL_SIZE/2;
        int by = (int)(game->bullets[i].y * CELL_SIZE) + CELL_SIZE/2;
        
        DrawCircle(bx, by, 3, YELLOW);
        DrawCircleLines(bx, by, 3, WHITE);
    }
}

/**
 * Render game over screen
 */
void render_game_over(GameData* game) {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    
    DrawText("GAME OVER", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 80, 60, RED);
    
    char final_score[64];
    sprintf(final_score, "FINAL SCORE: %d", game->player.score);
    DrawText(final_score, SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    
    if (is_highscore(game, game->player.score)) {
        DrawText("NEW HIGH SCORE!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 80, 30, LIME);
    }
    
    DrawText("Press ENTER to return to menu", SCREEN_WIDTH/2 - 150, 
            SCREEN_HEIGHT - 50, 20, WHITE);
}

/**
 * Render level complete screen
 */
void render_level_complete(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    
    DrawText("LEVEL COMPLETE!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 60, 50, GREEN);
    
    char level_text[64];
    sprintf(level_text, "Next: Phase %d", game->current_phase + 1);
    DrawText(level_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    
    DrawText("Press ENTER to continue", SCREEN_WIDTH/2 - 130, 
            SCREEN_HEIGHT/2 + 80, 20, WHITE);
}

/**
 * Render high score name entry screen
 */
void render_high_score(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("NEW HIGH SCORE!", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 100, 40, LIME);

    char score_text[64];
    sprintf(score_text, "SCORE: %d", game->player.score);
    DrawText(score_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 24, YELLOW);

    DrawText("Enter your name (max 31 chars):", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2, 20, WHITE);
    DrawText(game->current_name, SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 24, WHITE);

    DrawText("Press ENTER to confirm, BACKSPACE to edit, ESC to cancel", SCREEN_WIDTH/2 - 260, SCREEN_HEIGHT/2 + 80, 14, GRAY);
}
