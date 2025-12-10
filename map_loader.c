#include "game.h"

void load_phase(GameData* game, int phase_num) {
    char filename[64];
    sprintf(filename, "sample_maps/fase%d.txt", phase_num);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not load phase file %s\n", filename);
        for (int i = 0; i < PHASE_HEIGHT; i++) {
            memset(game->map[i], ' ', MAP_WIDTH);
            game->map[i][MAP_WIDTH] = '\0';
        }
        game->current_phase_height = 20;
        return;
    }

    int i = 0;
    while (i < PHASE_HEIGHT && fgets(game->map[i], MAP_WIDTH + 2, file) != NULL) {
        game->map[i][strcspn(game->map[i], "\n")] = '\0';
        i++;
    }
    game->current_phase_height = i;
    fclose(file);

    game->finish_line_y = 0;
    bool found_finish = false;
    for (int y = 0; y < game->current_phase_height; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (game->map[y][x] == 'L') {
                game->finish_line_y = y;
                found_finish = true;
                break;
            }
        }
        if (found_finish) break;
    }

    int enemy_count = 0;
    for (int y = 0; y < game->current_phase_height && enemy_count < MAX_ENEMIES; y++) {
        for (int x = 0; x < MAP_WIDTH && enemy_count < MAX_ENEMIES; x++) {
            char cell = game->map[y][x];

            if (cell == 'N' || cell == 'X' || cell == 'P' || cell == 'G') {
                game->enemies[enemy_count].x = (float)x;
                game->enemies[enemy_count].y = (float)y;
                game->enemies[enemy_count].active = true;

                if (cell == 'N') game->enemies[enemy_count].type = ENTITY_SHIP;
                else if (cell == 'X') game->enemies[enemy_count].type = ENTITY_HELICOPTER;
                else if (cell == 'P') game->enemies[enemy_count].type = ENTITY_BRIDGE_PIECE;
                else if (cell == 'G') {
                    game->enemies[enemy_count].type = ENTITY_FUEL_STATION;
                    int w = 0;
                    while (x + w < MAP_WIDTH && game->map[y][x + w] == 'G') w++;
                    game->enemies[enemy_count].width = w;
                    x += w - 1;
                }
                if (cell != 'G') game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
        }
    }
}

bool load_highscores(GameData* game) {
    FILE* file = fopen("highscore.bin", "rb");
    if (file == NULL) {
        game->num_highscores = 0;
        return false;
    }
    fread(&game->num_highscores, sizeof(int), 1, file);
    if (game->num_highscores > MAX_HIGHSCORES) game->num_highscores = MAX_HIGHSCORES;
    fread(game->highscores, sizeof(HighScore), game->num_highscores, file);
    fclose(file);
    return true;
}

void save_highscores(GameData* game) {
    FILE* file = fopen("highscore.bin", "wb");
    if (file == NULL) return;
    fwrite(&game->num_highscores, sizeof(int), 1, file);
    fwrite(game->highscores, sizeof(HighScore), game->num_highscores, file);
    fclose(file);
}

void initialize_game(GameData* game) {
    game->state = STATE_MENU;
    game->current_phase = 1;
    game->player.lives = 3;
    game->player.score = 0;
    game->player.fuel = 100.0f;
    game->player.x = MAP_WIDTH / 2;
    game->player.rapid_fire_cooldown = 0;
    game->menu_selected = 0;
    game->paused = 0;
    game->current_name[0] = '\0';
    game->current_scroll_speed = SCROLL_SPEED_NORMAL;

    // --- FLAGS INICIAIS ---
    game->trigger_shoot_sound = false;
    game->trigger_music_start = false;
    game->trigger_music_stop = false;
    game->trigger_gameover_sound = false;
    game->trigger_gamewon_sound = false; // --- NOVO: Inicializa como false ---

    for (int i = 0; i < MAX_BULLETS; i++) game->bullets[i].active = false;
    load_highscores(game);
}

void reset_level(GameData* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) game->enemies[i].active = false;
    for (int i = 0; i < MAX_BULLETS; i++) game->bullets[i].active = false;
    load_phase(game, game->current_phase);
    game->camera_y = (float)(game->current_phase_height - SCREEN_GRID_HEIGHT);
    game->player.x = MAP_WIDTH / 2;
    game->player.y = game->camera_y + PLAYER_SCREEN_Y;
    game->current_scroll_speed = SCROLL_SPEED_NORMAL;
    if (is_terrain(game, game->player.x, game->player.y)) {
        int ox = (int)game->player.x;
        for (int offset = 1; offset < MAP_WIDTH/2; offset++) {
            if (!is_terrain(game, (float)(ox + offset), game->player.y)) {
                game->player.x = (float)(ox + offset); break;
            }
            if (!is_terrain(game, (float)(ox - offset), game->player.y)) {
                game->player.x = (float)(ox - offset); break;
            }
        }
    }
}

bool is_walkable(GameData* game, float x, float y) {
    return !is_terrain(game, x, y);
}

bool is_terrain(GameData* game, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;
    if (ix < 0 || ix >= MAP_WIDTH) return true;
    if (iy >= game->current_phase_height) return true;
    if (iy < 0) return false;
    char cell = game->map[iy][ix];
    return (cell == 'T');
}

void add_highscore(GameData* game, const char* name, int score) {
    if (game->num_highscores < MAX_HIGHSCORES) {
        strcpy(game->highscores[game->num_highscores].name, name);
        game->highscores[game->num_highscores].score = score;
        game->num_highscores++;
    } else {
        strcpy(game->highscores[MAX_HIGHSCORES - 1].name, name);
        game->highscores[MAX_HIGHSCORES - 1].score = score;
    }
    for (int i = 0; i < game->num_highscores - 1; i++) {
        for (int j = i + 1; j < game->num_highscores; j++) {
            if (game->highscores[i].score < game->highscores[j].score) {
                HighScore temp = game->highscores[i];
                game->highscores[i] = game->highscores[j];
                game->highscores[j] = temp;
            }
        }
    }
    save_highscores(game);
}

bool is_highscore(GameData* game, int score) {
    if (game->num_highscores < MAX_HIGHSCORES) return true;
    return score > game->highscores[MAX_HIGHSCORES - 1].score;
}
