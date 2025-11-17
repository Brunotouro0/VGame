/*
 * Game Logic Implementation
 */

#include "game.h"

void update_game(GameData* game) {
    if (game->state == STATE_PLAYING) {
        if (!game->paused) {

            game->camera_y -= SCROLL_SPEED;

            update_player(game);
            update_enemies(game);
            update_bullets(game);
            handle_collisions(game);

            if (game->player.fuel <= 0) {
                game->player.lives--;
                if (game->player.lives <= 0) {
                    game->state = STATE_GAME_OVER;
                } else {
                    reset_level(game);
                    game->player.fuel = 100;
                }
            }

            // <-- MUDANÇA (LINHA DE CHEGADA)
            // Em vez de checar se a câmera chegou ao topo (camera_y < 0),
            // checamos se o jogador (na base da câmera) passou da 'finish_line_y'.
            if (game->player.y < game->finish_line_y) {

                if (game->current_phase == MAX_PHASES) {
                    game->state = STATE_GAME_WON;
                } else {
                    game->current_phase++;
                    game->state = STATE_LEVEL_COMPLETE;
                }
            }
            // FIM DA MUDANÇA
        }
    }
}


void update_player(GameData* game) {
    game->player.fuel -= FUEL_CONSUMPTION_RATE;

    game->player.y = game->camera_y + PLAYER_SCREEN_Y;

    // Clamp position (X)
    if (game->player.x <= 0) {
        game->player.x = 0;
    }
    if (game->player.x >= MAP_WIDTH) {
        game->player.x = MAP_WIDTH - 1;
    }

    // Limite de Y baseado no mapa longo
    if (game->player.y >= game->current_phase_height - 1) {
        game->player.y = game->current_phase_height - 1;
    }
}


void update_bullets(GameData* game) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (game->bullets[i].active) {
            game->bullets[i].y -= BULLET_SPEED;
            if (game->bullets[i].y < game->camera_y) {
                game->bullets[i].active = false;
            }
        }
    }

    if (game->player.rapid_fire_cooldown > 0) {
        game->player.rapid_fire_cooldown--;
    }
}


void update_enemies(GameData* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {

            switch (game->enemies[i].type) {
                case ENTITY_SHIP:
                    float ship_offset = sinf(game->camera_y * 0.05f) * SHIP_PATROL_WIDTH;
                    game->enemies[i].x += ship_offset * SHIP_SPEED;
                    if (game->enemies[i].x < 1) game->enemies[i].x = 1;
                    if (game->enemies[i].x > MAP_WIDTH - 2) game->enemies[i].x = MAP_WIDTH - 2;
                    break;

                case ENTITY_HELICOPTER:
                    float helicopter_wobble = sinf(game->camera_y * 0.08f + i) * 0.05f;
                    game->enemies[i].y += helicopter_wobble;
                    break;

                case ENTITY_BRIDGE_PIECE:
                    break;
                case ENTITY_FUEL_STATION:
                    break;
            }

            if (game->enemies[i].y > game->camera_y + SCREEN_GRID_HEIGHT + 1) {
                game->enemies[i].active = false;
            }
        }
    }
}


void handle_collisions(GameData* game) {
    // Check bullet-enemy collisions
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game->enemies[j].active) continue;

            float bullet_x = game->bullets[i].x;
            float bullet_y = game->bullets[i].y;
            float enemy_x = game->enemies[j].x;
            float enemy_y = game->enemies[j].y;
            float enemy_width = game->enemies[j].width;

            if (bullet_x >= enemy_x && bullet_x < enemy_x + enemy_width &&
                bullet_y >= enemy_y && bullet_y < enemy_y + 1) {

                game->bullets[i].active = false;
                game->enemies[j].active = false;

                int score_antes = game->player.score;
                int pontos_ganhos = 0;

                switch (game->enemies[j].type) {
                    case ENTITY_SHIP:         pontos_ganhos = SHIP_POINTS; break;
                    case ENTITY_HELICOPTER:   pontos_ganhos = HELICOPTER_POINTS; break;
                    case ENTITY_BRIDGE_PIECE: pontos_ganhos = BRIDGE_POINTS; break;
                    default: break;
                }

                if (pontos_ganhos > 0) {
                    game->player.score += pontos_ganhos;
                    if ((game->player.score / 1000) > (score_antes / 1000)) {
                        game->player.lives++;
                    }
                }
            }
        }
    }

    // Check player-enemy collisions
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) continue;

        float enemy_x = game->enemies[i].x;
        float enemy_y = game->enemies[i].y;

        if (game->player.x >= enemy_x && game->player.x < enemy_x + game->enemies[i].width &&
            game->player.y >= enemy_y && game->player.y < enemy_y + 1) {

            if (game->enemies[i].type == ENTITY_FUEL_STATION) {
                game->player.fuel = (game->player.fuel + FUEL_RECHARGE_AMOUNT > 200) ? 200 : game->player.fuel + FUEL_RECHARGE_AMOUNT;
                game->enemies[i].active = false;
            } else if (game->enemies[i].type == ENTITY_SHIP ||
                       game->enemies[i].type == ENTITY_HELICOPTER ||
                       game->enemies[i].type == ENTITY_BRIDGE_PIECE) {

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


void shoot_bullet(GameData* game) {
    if (game->player.rapid_fire_cooldown > 0) {
        return;
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) {
            game->bullets[i].x = game->player.x;
            game->bullets[i].y = game->player.y - 1;
            game->bullets[i].active = true;
            game->player.rapid_fire_cooldown = MAX_RAPID_FIRE_COOLDOWN;
            break;
        }
    }
}
