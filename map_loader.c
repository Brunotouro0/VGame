/*
 * Map Loading and Game Initialization
 */

#include "game.h"

void load_phase(GameData* game, int phase_num) {
    char filename[64];
    sprintf(filename, "sample_maps/fase%d.txt", phase_num);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not load phase file %s\n", filename);
        // Preenche com água se o arquivo não puder ser aberto
        for (int i = 0; i < PHASE_HEIGHT; i++) {
            memset(game->map[i], ' ', MAP_WIDTH);
            game->map[i][MAP_WIDTH] = '\0';
        }
        game->current_phase_height = 10;
        return;
    }

    // Leitura de mapa longo
    int i = 0;
    while (i < PHASE_HEIGHT && fgets(game->map[i], MAP_WIDTH + 2, file) != NULL) {
        game->map[i][strcspn(game->map[i], "\n")] = '\0';
        i++;
    }
    game->current_phase_height = i; // Armazena a altura real
    fclose(file);

    // Preenche o resto do mapa com água (se houver)
    for (; i < PHASE_HEIGHT; i++) {
        memset(game->map[i], ' ', MAP_WIDTH);
        game->map[i][MAP_WIDTH] = '\0';
    }


    // --- MUDANÇA (LINHA DE CHEGADA) ---

    game->finish_line_y = 0;
    bool found_finish_line = false;

    // Procura pela linha de chegada (caractere 'L')
    for (int y = 0; y < game->current_phase_height; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (game->map[y][x] == 'L') {
                game->finish_line_y = y; // Guarda a coordenada Y

                // <-- MUDANÇA AQUI: A linha 'game->map[y][x] = ' ';' FOI REMOVIDA.
                // O 'L' agora permanece no mapa.

                found_finish_line = true;
                break; // Para de procurar na horizontal
            }
        }
        if (found_finish_line) {
            break; // Para de procurar na vertical
        }
    }
    // --- FIM DA MUDANÇA ---


    // Parse enemies and fuel stations from map
    int enemy_count = 0;
    for (int i = 0; i < game->current_phase_height && enemy_count < MAX_ENEMIES; i++) {
        for (int j = 0; j < MAP_WIDTH && enemy_count < MAX_ENEMIES; j++) {
            char cell = game->map[i][j];

            if (cell == 'N') {
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_SHIP;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'X') {
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_HELICOPTER;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'P') {
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_BRIDGE_PIECE;
                game->enemies[enemy_count].active = true;
                game->enemies[enemy_count].width = 1;
                enemy_count++;
            }
            else if (cell == 'G') {
                game->enemies[enemy_count].x = j;
                game->enemies[enemy_count].y = i;
                game->enemies[enemy_count].type = ENTITY_FUEL_STATION;
                game->enemies[enemy_count].active = true;

                int width = 0;
                for (int k = j; k < MAP_WIDTH && game->map[i][k] == 'G'; k++) {
                    width++;
                }
                game->enemies[enemy_count].width = width;
                enemy_count++;
                j += width - 1;
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
    if (game->num_highscores > MAX_HIGHSCORES) {
        game->num_highscores = MAX_HIGHSCORES;
    }

    fread(game->highscores, sizeof(HighScore), game->num_highscores, file);
    fclose(file);

    return true;
}


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


void initialize_game(GameData* game) {
    game->state = STATE_MENU;
    game->current_phase = 1;
    game->player.lives = 3;
    game->player.score = 0;
    game->player.fuel = 100;

    game->player.x = MAP_WIDTH / 2;

    game->player.rapid_fire_cooldown = 0;
    game->menu_selected = 0;
    game->paused = 0;
    game->current_name[0] = '\0';
    game->name_length = 0;

    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }

    load_highscores(game);
}


void reset_level(GameData* game) {
    // 1. Limpa os inimigos e balas
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = false;
    }
    for (int i = 0; i < MAX_BULLETS; i++) {
        game->bullets[i].active = false;
    }

    // 2. Carrega o mapa longo (que agora também procura pelo 'L')
    load_phase(game, game->current_phase);

    // 3. Define a posição da CÂMERA (no fundo do mapa)
    game->camera_y = game->current_phase_height - SCREEN_GRID_HEIGHT;

    // 4. Define a posição X do jogador
    game->player.x = MAP_WIDTH / 2;
    // 5. Define a posição Y do jogador (travada na tela)
    game->player.y = game->camera_y + PLAYER_SCREEN_Y;

    // 6. VERIFICAÇÃO DE SEGURANÇA (para evitar "instant death")
    if (is_terrain(game, game->player.x, game->player.y)) {
        int spawn_y = (int)game->player.y;
        int original_x = (int)game->player.x;

        for (int offset = 1; offset < MAP_WIDTH / 2; offset++) {
            // Verifica à direita
            int x_direita = original_x + offset;
            if (x_direita < MAP_WIDTH && game->map[spawn_y][x_direita] == ' ') {
                game->player.x = x_direita;
                break;
            }
            // Verifica à esquerda
            int x_esquerda = original_x - offset;
            if (x_esquerda >= 0 && game->map[spawn_y][x_esquerda] == ' ') {
                game->player.x = x_esquerda;
                break;
            }
        }
    }
}


bool is_walkable(GameData* game, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;

    if (ix < 0 || ix >= MAP_WIDTH || iy < 0 || iy >= game->current_phase_height) {
        return false;
    }

    char cell = game->map[iy][ix];
    return (cell == ' ' || cell == 'A');
}


bool is_terrain(GameData* game, float x, float y) {
    int ix = (int)x;
    int iy = (int)y;

    if (ix < 0 || ix >= MAP_WIDTH) {
        return true;
    }
    if (iy >= game->current_phase_height) {
        return true;
    }
    if (iy < 0) {
        return false;
    }

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


bool is_highscore(GameData* game, int score) {
    if (game->num_highscores < MAX_HIGHSCORES) {
        return true;
    }
    return score > game->highscores[MAX_HIGHSCORES - 1].score;
}
