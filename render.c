/*
 * Rendering and Display System
 */

#include "game.h"
#include <stdio.h>

// Converte a coordenada Y do MAPA para a coordenada Y da TELA
float get_screen_y(GameData* game, float map_y) {
    return (map_y - game->camera_y) * CELL_SIZE;
}

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
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 128});
                DrawText("PAUSADO", SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 20, 40, YELLOW);
                DrawText("Aperte ENTER para continuar", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 20, WHITE);
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
        case STATE_GAME_WON:
            render_game_won(game);
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
    DrawText("RIVER-INF", SCREEN_WIDTH/2 - 165, 80, 60, YELLOW);
    Color option1_color = (game->menu_selected == 0) ? YELLOW : WHITE;
    Color option2_color = (game->menu_selected == 1) ? YELLOW : WHITE;
    DrawText("NOVO JOGO", SCREEN_WIDTH/2 - 80, 300, 30, option1_color);
    DrawText("SAIR", SCREEN_WIDTH/2 - 50, 360, 30, option2_color);
    DrawText("RECORDES:", 50, 150, 25, LIGHTGRAY);
    for (int i = 0; i < game->num_highscores && i < 10; i++) {
        char score_text[64];
        sprintf(score_text, "%d. %s - %d", i+1, game->highscores[i].name, game->highscores[i].score);
        DrawText(score_text, 50, 180 + i*30, 16, WHITE);
    }
    DrawText("Use W/S para selecionar. Use ENTER para confirmar.", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 40, 14, GRAY);
}

/**
 * Render HUD (score and fuel)
 */
void render_hud(GameData* game) {
    char hud_text[128];
    sprintf(hud_text, "PONTUACAO: %d", game->player.score);
    DrawText(hud_text, 10, 10, 20, YELLOW);
    sprintf(hud_text, "COMBUSTIVEL: %d", game->player.fuel);
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, 10, 20, (game->player.fuel < 20) ? RED : LIME);
    sprintf(hud_text, "VIDAS: %d", game->player.lives);
    DrawText(hud_text, SCREEN_WIDTH - 150, 10, 20, RED);
    sprintf(hud_text, "FASE: %d", game->current_phase);
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT - 30, 16, WHITE);
}

/**
 * Render map (terrain and obstacles)
 */
void render_map(GameData* game) {
    int y_start = (int)game->camera_y;
    int y_end = y_start + SCREEN_GRID_HEIGHT + 1;

    for (int y = y_start; y < y_end; y++) {
        if (y < 0 || y >= game->current_phase_height) continue;

        for (int x = 0; x < MAP_WIDTH; x++) {
            char cell = game->map[y][x];
            int px = x * CELL_SIZE;
            int py = (int)get_screen_y(game, y);

            if (cell == 'T') {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, DARKGREEN);
            } else if (cell == 'G') {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, ORANGE);
                DrawText("G", px + 12, py + 10, 20, WHITE);
            } else if (cell == 'P') {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, BROWN);
            }
            // <-- MUDANÇA (LINHA DE CHEGADA)
            else if (cell == 'L') {
                // 1. Desenha o bloco da linha de chegada
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, WHITE);

                // 2. Desenha o texto "CHEGADA" apenas no bloco do meio
                if (x == MAP_WIDTH / 2) {
                    const char* text = "FIM";
                    int fontSize = 15;
                    int textWidth = MeasureText(text, fontSize);

                    // Centraliza o texto dentro do bloco
                    DrawText(text, px + (CELL_SIZE - textWidth) / 2, py + (CELL_SIZE - fontSize) / 2, fontSize, BLACK);
                }
            }
            // --- FIM DA MUDANÇA ---
        }
    }
}

/**
 * Render player
 */
void render_player(GameData* game) {
    int px = (int)(game->player.x * CELL_SIZE);
    int py = (int)get_screen_y(game, game->player.y);

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
        int ey = (int)get_screen_y(game, game->enemies[i].y);

        if (ey < -CELL_SIZE || ey > SCREEN_HEIGHT) continue;

        switch (game->enemies[i].type) {
            case ENTITY_SHIP:
                DrawRectangle(ex, ey, CELL_SIZE, CELL_SIZE, RED);
                DrawRectangleLines(ex, ey, CELL_SIZE, CELL_SIZE, RED);
                DrawText("N", ex + 12, ey + 10, 20, WHITE);
                break;
            case ENTITY_HELICOPTER:
                DrawCircle(ex + CELL_SIZE/2, ey + CELL_SIZE/2, CELL_SIZE/2 - 2, RED);
                DrawCircleLines(ex + CELL_SIZE/2, ey + CELL_SIZE/2, CELL_SIZE/2 - 2, RED);
                DrawText("X", ex + 10, ey + 5, 20, WHITE);
                break;
            case ENTITY_BRIDGE_PIECE:
                DrawRectangle(ex, ey, CELL_SIZE, CELL_SIZE, DARKGRAY);
                DrawRectangleLines(ex, ey, CELL_SIZE, CELL_SIZE, BLACK);
                break;
            case ENTITY_FUEL_STATION:
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
        int by = (int)get_screen_y(game, game->bullets[i].y) + CELL_SIZE/2;

        if (by < 0 || by > SCREEN_HEIGHT) continue;

        DrawCircle(bx, by, 3, YELLOW);
        DrawCircleLines(bx, by, 3, WHITE);
    }
}

/**
 * Render game over screen
 */
void render_game_over(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("FIM DE JOGO!", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 80, 60, RED);
    char final_score[64];
    sprintf(final_score, "PONTUACAO FINAL: %d", game->player.score);
    DrawText(final_score, SCREEN_WIDTH/2 - 155, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    if (is_highscore(game, game->player.score)) {
        DrawText("NOVO RECORDE!", SCREEN_WIDTH/2 - 220, SCREEN_HEIGHT/2 + 80, 30, LIME);
    }
    DrawText("Aperte ENTER para voltar ao menu", SCREEN_WIDTH/2 - 170, SCREEN_HEIGHT - 50, 20, WHITE);
}

/**
 * Render level complete screen
 */
void render_level_complete(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("NIVEL CONCLUIDO!", SCREEN_WIDTH/2 -230, SCREEN_HEIGHT/2 - 60, 50, GREEN);
    char level_text[64];
    sprintf(level_text, "PROXIMA FASE: %d", game->current_phase);
    DrawText(level_text, SCREEN_WIDTH/2 - 135, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    DrawText("Aperte ENTER para continuar", SCREEN_WIDTH/2 - 155, SCREEN_HEIGHT/2 + 80, 20, WHITE);
}

/**
 * Render high score name entry screen
 */
void render_high_score(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("NOVO RECORDE!", SCREEN_WIDTH/2 - 140, SCREEN_HEIGHT/2 - 100, 40, LIME);
    char score_text[64];
    sprintf(score_text, "PONTUACAO: %d", game->player.score);
    DrawText(score_text, SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 24, YELLOW);
    DrawText("DIGITE SEU NOME: (max 31 letras):", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2, 20, WHITE);
    DrawText(game->current_name, SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 + 30, 24, WHITE);
    DrawText("Pressione ENTER para confirmar, BACKSPACE para editar, ESC para cancelar.", SCREEN_WIDTH/2 - 260, SCREEN_HEIGHT/2 + 80, 14, GRAY);
}

/**
 * Render game won screen
 */
void render_game_won(GameData* game) {
    ClearBackground(BLACK);
    const char* msg1 = "VOCE VENCEU!";
    int fontSize1 = 60;
    int textWidth1 = MeasureText(msg1, fontSize1);
    DrawText(msg1, (SCREEN_WIDTH - textWidth1) / 2, SCREEN_HEIGHT / 2 - 80, fontSize1, GREEN);

    char score_msg[100];
    sprintf(score_msg, "Obrigado por jogar!");
    int fontSize2 = 30;
    int textWidth2 = MeasureText(score_msg, fontSize2);
    DrawText(score_msg, (SCREEN_WIDTH - textWidth2) / 2, SCREEN_HEIGHT / 2 + 10, fontSize2, WHITE);

    const char* msg3 = "Pressione ENTER para voltar ao Menu";
    int fontSize3 = 20;
    int textWidth3 = MeasureText(msg3, fontSize3);
    DrawText(msg3, (SCREEN_WIDTH - textWidth3) / 2, SCREEN_HEIGHT - 60, fontSize3, GRAY);

}
