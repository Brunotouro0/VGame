/*
 * Input Handling
 */

#include "game.h"
#include <string.h>

static bool quit_confirmed = false;

/**
 * Handle all input for the game
 */
void handle_input(GameData* game) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (game->state == STATE_PLAYING) {
            game->state = STATE_MENU;
        } else if (game->state == STATE_MENU) {
            // Graceful exit without platform-specific dialogs
            exit(0);
        }
    }
    
    switch (game->state) {
        case STATE_MENU:
            handle_menu_input(game);
            break;
        case STATE_PLAYING:
            handle_gameplay_input(game);
            break;
        
        case STATE_PAUSED:
            if (IsKeyPressed(KEY_ENTER)) {
                game->paused = 0;
            }
            break;
        
        case STATE_GAME_OVER:
            if (IsKeyPressed(KEY_ENTER)) {
                // If player's score qualifies, go to name-entry state
                if (is_highscore(game, game->player.score)) {
                    game->state = STATE_HIGH_SCORE;
                    // reset current name buffer
                    game->name_length = 0;
                    game->current_name[0] = '\0';
                } else {
                    game->state = STATE_MENU;
                    game->menu_selected = 0;
                }
            }
            break;

        case STATE_HIGH_SCORE:
            {
                // Collect typed characters via raylib's GetCharPressed
                int chr = GetCharPressed();
                while (chr > 0) {
                    // Accept printable ASCII
                    if (game->name_length < (int)sizeof(game->current_name) - 1 && chr >= 32 && chr <= 126) {
                        game->current_name[game->name_length++] = (char)chr;
                        game->current_name[game->name_length] = '\0';
                    }
                    chr = GetCharPressed();
                }

                // Backspace
                if (IsKeyPressed(KEY_BACKSPACE) && game->name_length > 0) {
                    game->name_length--;
                    game->current_name[game->name_length] = '\0';
                }

                // Confirm name
                if (IsKeyPressed(KEY_ENTER)) {
                    // Provide default name if empty
                    if (game->name_length == 0) {
                        strcpy(game->current_name, "ANON");
                    }
                    add_highscore(game, game->current_name, game->player.score);
                    game->state = STATE_MENU;
                    game->menu_selected = 0;
                }
                // Allow cancel with ESC
                if (IsKeyPressed(KEY_ESCAPE)) {
                    game->state = STATE_MENU;
                    game->menu_selected = 0;
                }
            }
            break;
        
        case STATE_LEVEL_COMPLETE:
            if (IsKeyPressed(KEY_ENTER)) {
                reset_level(game);
                game->state = STATE_PLAYING;
            }
            break;
        
        default:
            break;
    }
}

/**
 * Handle menu input
 */
void handle_menu_input(GameData* game) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        game->menu_selected--;
        if (game->menu_selected < 0) {
            game->menu_selected = 1;  // Wrap around
        }
    }
    
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        game->menu_selected++;
        if (game->menu_selected > 1) {
            game->menu_selected = 0;  // Wrap around
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)) {
        if (game->menu_selected == 0) {
            // Start game
            game->state = STATE_PLAYING;
            game->current_phase = 1;
            game->player.score = 0;
            game->player.lives = 3;
            game->player.fuel = 100;
            reset_level(game);
        } else if (game->menu_selected == 1) {
            // Quit
            exit(0);
        }
    }
}

/**
 * Handle gameplay input
 */
void handle_gameplay_input(GameData* game) {
    // Pause
    if (IsKeyPressed(KEY_ENTER)) {
        game->paused = !game->paused;
    }
    
    // Movement left
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        game->player.x -= PLAYER_SPEED * 2;
    }
    
    // Movement right
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        game->player.x += PLAYER_SPEED * 2;
    }
    
    // Shoot
    if (IsKeyPressed(KEY_K) || IsKeyPressed(KEY_SPACE)) {
        shoot_bullet(game);
    }
    
    // Restart level
    if (IsKeyPressed(KEY_R)) {
        reset_level(game);
        game->player.fuel = 100;
    }
}
