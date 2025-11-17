/*
 * RIVER-INF Game Header
 * Inspired by Atari 2600 River Raid (1982)
 * INF01202 - Algoritmos e Programação
 */

#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <stdlib.h>

// Game Constants
#define PLAYER_SPEED 0.1f
#define SCREEN_WIDTH 600         // 24 characters * 40 pixels
#define SCREEN_HEIGHT 960         // 20 characters * 40 pixels
#define MAP_WIDTH 24
#define MAP_HEIGHT 40
#define CELL_SIZE 25
#define MAX_PHASES 5
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
    float x, y;                // Position in grid coordinates
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
    int width;                 // For bridges and fuel stations
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
    int phase_progress;        // Vertical position in phase
    char map[MAP_HEIGHT][MAP_WIDTH + 1];
    HighScore highscores[MAX_HIGHSCORES];
    int num_highscores;
    int menu_selected;
    int paused;
    // Temporary storage for entering new high score name
    char current_name[32];
    int name_length;
} GameData;

// Function Declarations

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

// Rendering
void render_game(GameData* game);
void render_menu(GameData* game);
void render_hud(GameData* game);
void render_map(GameData* game);
void render_player(GameData* game);
void render_enemies(GameData* game);
void render_bullets(GameData* game);
void render_high_score(GameData* game);

// Utilities
void initialize_game(GameData* game);
void reset_level(GameData* game);
bool is_walkable(GameData* game, float x, float y);
bool is_terrain(GameData* game, float x, float y);
void add_highscore(GameData* game, const char* name, int score);
bool is_highscore(GameData* game, int score);

// Input Handling
void handle_menu_input(GameData* game);
void handle_gameplay_input(GameData* game);

// Additional Rendering
void render_game_over(GameData* game);
void render_level_complete(GameData* game);
void render_game_won (GameData* game);

#endif // GAME_H
