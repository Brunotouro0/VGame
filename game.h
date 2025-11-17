/*
 * RIVER-INF Game Header
 */

#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// --- Constantes de Física e Jogo ---
#define BULLET_SPEED 0.3f
#define FUEL_CONSUMPTION_RATE 0.00f
#define MAX_RAPID_FIRE_COOLDOWN 8
#define SHIP_POINTS 30
#define HELICOPTER_POINTS 60
#define BRIDGE_POINTS 200
#define FUEL_RECHARGE_AMOUNT 30
#define HELICOPTER_SPEED 0.08f
#define SHIP_SPEED 0.1f
#define SHIP_PATROL_WIDTH 4

// --- Constantes de Auto-Scroll ---
#define BASE_SPEED 0.1f
#define SCROLL_SPEED BASE_SPEED
#define PLAYER_HORZ_SPEED (BASE_SPEED * 2.0f)

// --- Constantes de Tela e Mapa ---
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define MAP_WIDTH 24
#define CELL_SIZE 25

#define PHASE_HEIGHT 500
#define MAX_PHASES 5

#define SCREEN_GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)
#define PLAYER_SCREEN_Y 28

#define MAX_BULLETS 100
#define MAX_ENEMIES 50
#define MAX_HIGHSCORES 10

// Entity Types
typedef enum {
    ENTITY_SHIP,
    ENTITY_HELICOPTER,
    ENTITY_BRIDGE_PIECE,
    ENTITY_FUEL_STATION
} EntityType;

// Game States
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_LEVEL_COMPLETE,
    STATE_HIGH_SCORE,
    STATE_GAME_WON
} GameState;

// Player Structure
typedef struct {
    float x, y;
    int fuel;
    int lives;
    int score;
    int rapid_fire_cooldown;
} Player;

// Bullet Structure
typedef struct {
    float x, y;
    bool active;
} Bullet;

// Enemy Structure
typedef struct {
    float x, y;
    EntityType type;
    bool active;
    int width;
} Enemy;

// High Score Structure
typedef struct {
    char name[32];
    int score;
} HighScore;

// Game State Structure
typedef struct {
    GameState state;
    Player player;
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[MAX_BULLETS];
    int current_phase;

    char map[PHASE_HEIGHT][MAP_WIDTH + 1];
    int current_phase_height;
    float camera_y;

    // <-- MUDANÇA (LINHA DE CHEGADA)
    int finish_line_y; // Guarda a coordenada Y da linha de chegada

    HighScore highscores[MAX_HIGHSCORES];
    int num_highscores;
    int menu_selected;
    int paused;
    char current_name[32];
    int name_length;
} GameData;

// --- Declarações de Funções ---
// (O restante do arquivo não muda)

// Map Loading
void load_phase(GameData* game, int phase_num);
bool load_highscores(GameData* game);
void save_highscores(GameData* game);

// Game Logic
void update_game(GameData* game);
void update_player(GameData* game);
void update_enemies(GameData* game);
void update_bullets(GameData* game);
void handle_collisions(GameData* game);

// Input
void handle_input(GameData* game);
void shoot_bullet(GameData* game);
void handle_menu_input(GameData* game);
void handle_gameplay_input(GameData* game);

// Rendering
void render_game(GameData* game);
void render_menu(GameData* game);
void render_hud(GameData* game);
void render_map(GameData* game);
void render_player(GameData* game);
void render_enemies(GameData* game);
void render_bullets(GameData* game);
void render_high_score(GameData* game);
void render_game_over(GameData* game);
void render_level_complete(GameData* game);
void render_game_won(GameData* game);

// Utilities
void initialize_game(GameData* game);
void reset_level(GameData* game);
bool is_walkable(GameData* game, float x, float y);
bool is_terrain(GameData* game, float x, float y);
void add_highscore(GameData* game, const char* name, int score);
bool is_highscore(GameData* game, int score);

#endif // GAME_H
