#include "game.h"

void update_game(GameData* game) {
    if (game->state == STATE_PLAYING) {
        if (!game->paused) {

            //Scroll controlado pela velocidade
            game->camera_y -= game->current_scroll_speed;

            update_player(game);
            update_enemies(game);
            update_bullets(game);
            handle_collisions(game);

            if (game->player.fuel <= 0) {
                game->player.lives--;
                if (game->player.lives <= 0) {
                    game->state = STATE_GAME_OVER;
                    game->trigger_music_stop = true;
                    game->trigger_gameover_sound = true;
                } else {
                    reset_level(game);
                    game->player.fuel = 100.0f;
                }
            }

            if (game->player.y < game->finish_line_y) {
                if (game->current_phase == MAX_PHASES) {
                    game->state = STATE_GAME_WON;
                    game->trigger_music_stop = true;
                    game->trigger_gamewon_sound = true;
                } else {
                    game->current_phase++;
                    game->state = STATE_LEVEL_COMPLETE;
                }
            }
        }
    }
}

void update_player(GameData* game) {
    game->player.fuel -= FUEL_CONSUMPTION_RATE;
    game->player.y = game->camera_y + PLAYER_SCREEN_Y;

    if (game->player.x <= 0) game->player.x = 0;
    if (game->player.x >= MAP_WIDTH - 1) game->player.x = MAP_WIDTH - 1;
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
                    game->enemies[i].x += sinf(game->camera_y * 0.05f) * SHIP_SPEED;
                    break;

                case ENTITY_HELICOPTER:
                    float helicopter_wobble = sinf(game->camera_y * 0.1f + i) * HELICOPTER_SPEED;
                    game->enemies[i].y += helicopter_wobble;
                    break;

                default: break;
            }
            if (game->enemies[i].y > game->camera_y + SCREEN_GRID_HEIGHT + 1) {
                game->enemies[i].active = false;
            }
        }
    }
}

void handle_collisions(GameData* game) {

    //1.COLISÃO: TIRO x INIMIGO

    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) continue;

        for (int j = 0; j < MAX_ENEMIES; j++) {
            if (!game->enemies[j].active) continue;

            float bx = game->bullets[i].x; //o x da bala que foi disparada
            float by = game->bullets[i].y; //o y da bala que foi disparada
            float ex = game->enemies[j].x; // o x do inimigo que estamos lidando
            float ey = game->enemies[j].y; // o y do inimigo que estamos lidando
            float ew = (float)game->enemies[j].width; // a largura do inimigo que estamos lidando

            if (bx >= ex && bx < ex + ew && by >= ey && by < ey + 1) { //esse if verifica se a bala esta ocupando a mesma posicao que o inimigo, se sim entao colide

                game->bullets[i].active = false; //se a colisao ocorre, a bala é desativada
                game->enemies[j].active = false; //se a colisao ocorre, o inimigo é desativado

                int score_antes = game->player.score;
                int pontos = 0;
                switch (game->enemies[j].type) {
                    case ENTITY_SHIP:         pontos = SHIP_POINTS; break;
                    case ENTITY_HELICOPTER:   pontos = HELICOPTER_POINTS; break;
                    case ENTITY_BRIDGE_PIECE: pontos = BRIDGE_POINTS; break;
                    case ENTITY_FUEL_STATION: pontos = 30; break;
                }

                game->player.score += pontos;
                if ((game->player.score / 1000) > (score_antes / 1000)) {
                    game->player.lives++;
                }
            }
        }
    }

    //2.COLISÃO: JOGADOR x INIMIGO
    float p_width = 0.7f; //define o quão largo é o cara que ta jogando
    float px_min = game->player.x + (1.0f - p_width) / 2.0f;
    float px_max = px_min + p_width;
    float py = game->player.y;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) continue;

        float ex = game->enemies[i].x; // x do inimigo que estamos lidando
        float ew = (float)game->enemies[i].width; // largura do inimigo que estamos lidando
        float ey = game->enemies[i].y; // y do inimigo que estamos lidando

        bool collision = (px_max > ex && px_min < ex + ew && py + 0.8f > ey && py < ey + 1.0f); //ver se o inimigo esta dentro do player 😳

        if (collision) {
            if (game->enemies[i].type == ENTITY_FUEL_STATION) {
                //Coleta a gasolina
                game->player.fuel += FUEL_RECHARGE_AMOUNT;
                if (game->player.fuel > 100.0f) game->player.fuel = 100.0f;
                game->enemies[i].active = false;
            } else {
                game->player.lives--;
                if (game->player.lives <= 0) {
                    game->state = STATE_GAME_OVER;
                    game->trigger_music_stop = true;
                    game->trigger_gameover_sound = true;
                } else {
                    reset_level(game);
                    game->player.fuel = 100.0f;
                }
            }
        }
    }

    // 3. COLISÃO: JOGADOR x TERRENO
    if (is_terrain(game, px_min, game->player.y) || is_terrain(game, px_max, game->player.y)) {
        game->player.lives--;
        if (game->player.lives <= 0) {
            game->state = STATE_GAME_OVER;
            game->trigger_music_stop = true;
            game->trigger_gameover_sound = true;
        } else {
            reset_level(game);
            game->player.fuel = 100.0f;
        }
    }
}
