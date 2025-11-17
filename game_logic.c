/*
 * Game Logic Implementation
 */

#include "game.h"
#include <math.h>

// Game constants for physics
#define BULLET_SPEED 0.3f
#define FUEL_CONSUMPTION_RATE 0.00f
#define SHIP_POINTS 30
#define HELICOPTER_POINTS 60
#define BRIDGE_POINTS 200
#define FUEL_RECHARGE_AMOUNT 30

#define HELICOPTER_SPEED 0.08f
#define SHIP_SPEED 0.1f
#define SHIP_PATROL_WIDTH 4
#define MAX_RAPID_FIRE_COOLDOWN 8

/**
 * Main game update function
 */
void update_game(GameData* game) {
    if (game->state == STATE_PLAYING) {
        if (!game->paused) {
            update_player(game);
            update_enemies(game);
            update_bullets(game);
            handle_collisions(game);

            // Check if fuel depleted
            if (game->player.fuel <= 0) {
                game->player.lives--;
                if (game->player.lives <= 0) {
                    game->state = STATE_GAME_OVER;
                } else {
                    reset_level(game);
                    game->player.fuel = 100;
                }
            }

            // Check if reached top of map (level complete)
            if (game->player.y < 0) {
    // Se o jogador acabou de terminar a FASE 5 (a última)
    if (game->current_phase == MAX_PHASES) {
        game->state = STATE_GAME_WON; // Mude para o estado de VENCEU!
    }
    // Se o jogador terminou qualquer outra fase
    else {
        game->current_phase++;
        game->state = STATE_LEVEL_COMPLETE;
    }
}
        }
    }
}

/**
 * Update player position and fuel
 */
void update_player(GameData* game) {
    // Consume fuel continuously
    game->player.fuel -= FUEL_CONSUMPTION_RATE;

    // Auto move forward (up)
    game->player.y -= PLAYER_SPEED;

    // Clamp position within bounds
    if (game->player.x <= 0) {
        game->player.x = 0;
    }
    if (game->player.x >= MAP_WIDTH) {
        game->player.x = MAP_WIDTH - 1;
    }

    if (game->player.y >= MAP_HEIGHT) {
        game->player.y = MAP_HEIGHT - 1;
    }
}

/**
 * Update bullets
 */
void update_bullets(GameData* game) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            game->bullets[i].y -= BULLET_SPEED;

            // Deactivate if out of bounds
            if (game->bullets[i].y < 0) {
                game->bullets[i].active = false;
            }
        }
    }

    // Update fire rate cooldown
    if (game->player.rapid_fire_cooldown > 0) {
        game->player.rapid_fire_cooldown--;
    }
}

/**
 * Update enemies with intelligent movement patterns
 */
void update_enemies(GameData* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            // Base movement down the screen
            game->enemies[i].y += PLAYER_SPEED;

            switch (game->enemies[i].type) {
                case ENTITY_SHIP:
                    // Ships patrol horizontally with sinusoidal pattern
                    // Use game phase progress to create varied patterns
                    float ship_offset = sinf(game->phase_progress * 0.05f) * SHIP_PATROL_WIDTH;
                    game->enemies[i].x += ship_offset * SHIP_SPEED;

                    // Keep ships in bounds
                    if (game->enemies[i].x < 1) {
                        game->enemies[i].x = 1;
                    }
                    if (game->enemies[i].x > MAP_WIDTH - 2) {
                        game->enemies[i].x = MAP_WIDTH - 2;
                    }
                    break;

                case ENTITY_HELICOPTER:
                    // Helicopters move slower and hover more erratically
                    float helicopter_wobble = sinf(game->phase_progress * 0.08f + i) * 0.05f;
                    game->enemies[i].y += helicopter_wobble;
                    break;

                case ENTITY_BRIDGE_PIECE:
                    // Bridges are stationary obstacles
                    // No additional movement needed
                    break;

                case ENTITY_FUEL_STATION:
                    // Fuel stations drift slowly
                    game->enemies[i].y += 0.02f;
                    break;

                default:
                    break;
            }

            // Remove if out of bounds
            if (game->enemies[i].y > MAP_HEIGHT) {
                game->enemies[i].active = false;
            }
        }
    }
}

/**
 * Handle collisions between player, enemies, and bullets
 */
void handle_collisions(GameData* game) {
    // Check bullet-enemy collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game->enemies[j].active) continue;

            // Simple bounding box collision
            float bullet_x = game->bullets[i].x;
            float bullet_y = game->bullets[i].y;

            float enemy_x = game->enemies[j].x;
            float enemy_y = game->enemies[j].y;
            float enemy_width = game->enemies[j].width;

            // Check if bullet hits enemy
            if (bullet_x >= enemy_x && bullet_x < enemy_x + enemy_width &&
                bullet_y >= enemy_y && bullet_y < enemy_y + 1) {

                game->bullets[i].active = false;
                game->enemies[j].active = false;

                // Award points
                switch (game->enemies[j].type) {
                    case ENTITY_SHIP:
                        game->player.score += SHIP_POINTS;
                        break;
                    case ENTITY_HELICOPTER:
                        game->player.score += HELICOPTER_POINTS;
                        break;
                    case ENTITY_BRIDGE_PIECE:
                        game->player.score += BRIDGE_POINTS;
                        break;
                    case ENTITY_FUEL_STATION:
                        break;  // No points for fuel
                }

                // Add bonus life every 1000 points
                if (game->player.score / 1000 > (game->player.score - (SHIP_POINTS + HELICOPTER_POINTS)) / 1000) {
                    game->player.lives++;
                }
            }
        }
    }

    // Check player-enemy collisions
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) continue;

        float enemy_x = game->enemies[i].x;
        float enemy_y = game->enemies[i].y;

        // Check collision with player
        if (game->player.x >= enemy_x && game->player.x < enemy_x + game->enemies[i].width &&
            game->player.y >= enemy_y && game->player.y < enemy_y + 1) {

            // Handle different collision types
            if (game->enemies[i].type == ENTITY_FUEL_STATION) {
                // Refuel
                game->player.fuel = (game->player.fuel + FUEL_RECHARGE_AMOUNT > 200) ? 200 : game->player.fuel + FUEL_RECHARGE_AMOUNT;
                game->enemies[i].active = false;  // Consume fuel station
            } else if (game->enemies[i].type == ENTITY_SHIP ||
                      game->enemies[i].type == ENTITY_HELICOPTER ||
                      game->enemies[i].type == ENTITY_BRIDGE_PIECE) {
                // Collision with enemy = lose life
                game->player.lives--;
                if (game->player.lives <= 0) {
                    game->state = STATE_GAME_OVER;
                } else {
                    reset_level(game);
                    game->player.fuel = 100;
                }
            }
        }
    }

    // Check player-terrain collisions
    if (is_terrain(game, game->player.x, game->player.y)) {
        game->player.lives--;
        if (game->player.lives <= 0) {
            game->state = STATE_GAME_OVER;
        } else {
            reset_level(game);
            game->player.fuel = 100;
        }
    }
}

/**
 * Shoot a bullet from player position
 */
void shoot_bullet(GameData* game) {
    // Check fire rate cooldown
    if (game->player.rapid_fire_cooldown > 0) {
        return;
    }

    // Find inactive bullet slot
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) {
            game->bullets[i].x = game->player.x;
            game->bullets[i].y = game->player.y - 1;  // Spawn slightly ahead of player
            game->bullets[i].active = true;

            // Set cooldown for rapid fire prevention
            game->player.rapid_fire_cooldown = MAX_RAPID_FIRE_COOLDOWN;
            break;
        }
    }
}
