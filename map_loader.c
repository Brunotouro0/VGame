/*
 * Map Loading and Game Initialization
 */

#include "game.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Load a phase from a text file
 * Format: fase1.txt, fase2.txt, etc.
 * Map size: 24x20 characters
 */
void load_phase(GameData* game, int phase_num) {
    char filename[64];
    sprintf(filename, "sample_maps/fase%d.txt", phase_num);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not load phase file %s\n", filename);
        return;
    }

    // Clear map
    for (int i = 0; i < MAP_HEIGHT; i++) {
        memset(game->map[i], ' ', MAP_WIDTH);
        game->map[i][MAP_WIDTH] = '\0';
    }

    // Read map from file

    for (int i = 0; i < MAP_HEIGHT; i++) {
        char line[MAP_WIDTH + 2];
        if (fgets(line, sizeof(line), file) != NULL) {
            strncpy(game->map[i], line, MAP_WIDTH);
            game->map[i][MAP_WIDTH] = '\0';
            // Remove newline if present
            if (game->map[i][strlen(game->map[i]) - 1] == '\n') {
                game->map[i][strlen(game->map[i]) - 1] = '\0';
            }
        }
    }

    fclose(file);

    // Parse enemies and fuel stations from map
    int enemy_count = 0;
    for (int i = 0; i < MAP_HEIGHT && enemy_count < MAX_ENEMIES; i++) {
        for (int j = 0; j < MAP_WIDTH && enemy_count < MAX_ENEMIES; j++) {
            char cell = game->map[i][j];

            if (cell == 'N') {  // Ship
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_SHIP;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'X') {  // Helicopter
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_HELICOPTER;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'P') {  // Bridge
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_BRIDGE_PIECE;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'G') {  // Fuel station
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_FUEL_STATION;
                game->enemies[enemy_count].active = true;
                // Find width of fuel station
                int width = 0;
                for (int k = j; k < MAP_WIDTH && game->map[i][k] == 'G'; k++) {
                    width++;
                }
                game->enemies[enemy_count].width = width;
                enemy_count++;
            }
        }
    }
}

/**
 * Load high scores from binary file
 */
bool load_highscores(GameData* game) {
    FILE* file = fopen("highscore.bin", "rb");
    if (file == NULL) {
        game->num_highscores = 0;
        return false;
    }

    fread(&game->num_highscores, sizeof(int), 1, file);
    if (game->num_highscores > MAX_HIGHSCORES) {
        game->num_highscores = MAX_HIGHSCORES;
    }

    fread(game->highscores, sizeof(HighScore), game->num_highscores, file);
    fclose(file);

    return true;
}

/**
 * Save high scores to binary file
 */
void save_highscores(GameData* game) {
    FILE* file = fopen("highscore.bin", "wb");
    if (file == NULL) {
        printf("Error: Could not save high scores\n");
        return;
    }

    fwrite(&game->num_highscores, sizeof(int), 1, file);
    fwrite(game->highscores, sizeof(HighScore), game->num_highscores, file);
    fclose(file);
}

/**
 * Initialize game state
 */
void initialize_game(GameData* game) {
    game->state = STATE_MENU;
    game->current_phase = 1;
    game->player.lives = 3;
    game->player.score = 0;
    game->player.fuel = 100;
    game->player.x = MAP_WIDTH / 2;
    game->player.y = MAP_HEIGHT - 2;
    game->player.rapid_fire_cooldown = 0;
    game->menu_selected = 0;
    game->paused = 0;
    game->current_name[0] = '\0';
    game->name_length = 0;

    // Clear bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }

    // Load highscores
    load_highscores(game);
}

/**
 * Reset current level
 */
void reset_level(GameData* game) {
    // Clear enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = false;
    }

    // Clear bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }

    // Reset player position
    game->player.x = MAP_WIDTH / 2;
    game->player.y = MAP_HEIGHT - 2;

    // Load the phase map
    load_phase(game, game->current_phase);
}

/**
 * Check if a position is walkable (not terrain or island)
 */
bool is_walkable(GameData* game, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;

    // Check bounds
    if (ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= MAP_HEIGHT) {
        return false;
    }

    char cell = game->map[iy][ix];
    // Can only walk on empty space (river)
    return (cell == ' ' || cell == 'A');
}

/**
 * Check if a position has terrain/forest
 */
bool is_terrain(GameData* game, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;

    if (ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= MAP_HEIGHT) {
        return true;  // Out of bounds = collision
    }

    char cell = game->map[iy][ix];
    return (cell == 'T');
}

/**
 * Add a new high score
 */
void add_highscore(GameData* game, const char* name, int score) {
    if (game->num_highscores < MAX_HIGHSCORES) {
        strcpy(game->highscores[game->num_highscores].name, name);
        game->highscores[game->num_highscores].score = score;
        game->num_highscores++;
    } else {
        // Replace lowest score
        strcpy(game->highscores[MAX_HIGHSCORES - 1].name, name);
        game->highscores[MAX_HIGHSCORES - 1].score = score;
    }

    // Sort in descending order
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

/**
 * Check if score qualifies for high score table
 */
bool is_highscore(GameData* game, int score) {
    if (game->num_highscores < MAX_HIGHSCORES) {
        return true;
    }
    return score > game->highscores[MAX_HIGHSCORES - 1].score;
}
