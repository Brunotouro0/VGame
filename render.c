#include "game.h"

float get_screen_y(GameData* game, float map_y) {
    return (map_y - game->camera_y) * CELL_SIZE;
}

void render_game(GameData* game) {
    BeginDrawing();
    ClearBackground(DARKBLUE); //Define tudo o que ele nao ler e interpretar no arquivo txt como a cor azul (rio)

    switch (game->state) {
        case STATE_MENU: render_menu(game); break; //se o "estado do jogo" for o menu, ele renderiza a tela do menu com a funcao render_menu
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
            }
            break;
        case STATE_GAME_OVER: render_game_over(game); break;
        case STATE_HIGH_SCORE: render_high_score(game); break;
        case STATE_LEVEL_COMPLETE: render_level_complete(game); break;
        case STATE_GAME_WON: render_game_won(game); break;
        default: break;
    }
    EndDrawing();
}

void render_menu(GameData* game) {
    // Gera o Menu
    DrawText("RIVER-INF", SCREEN_WIDTH/2 - 165, 80, 60, YELLOW);
    Color option1_color = (game->menu_selected == 0) ? YELLOW : WHITE; // ? é um if/else em uma linha só: se o game->menu_selected for 0, desenha em amraleo, se nao, desenha em branco
    Color option2_color = (game->menu_selected == 1) ? YELLOW : WHITE;
    DrawText("NOVO JOGO", SCREEN_WIDTH/2 - 80, 300, 30, option1_color);
    DrawText("SAIR", SCREEN_WIDTH/2 - 50, 360, 30, option2_color);

    DrawText("RECORDES:", 50, 150, 25, LIGHTGRAY);
    for (int i = 0; i < game->num_highscores && i < 10; i++) {
        char score_text[64];
        sprintf(score_text, "%d. %s - %d", i+1, game->highscores[i].name, game->highscores[i].score); //sprintf é usado pra formatar melhor o texto, aqui ele ta mudando o array "score text" na propria memoria pra ser daquela forma
        DrawText(score_text, 50, 180 + i*30, 16, WHITE); //depois que o array é mudado na memoria, "drawtext" desenha ele na tela, ai fica melhor de visualizar
    }
    DrawText("Use W/S para selecionar. Use ENTER para confirmar.", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 40, 14, GRAY);
}

void render_hud(GameData* game) {
    char hud_text[128];
    sprintf(hud_text, "PONTUACAO: %d", game->player.score); //aqui ele faz a mesma coisa, sprintf muda o array hud_text para conter a pontuacao do player (contida em came->player.score) e depois printa na tela
    DrawText(hud_text, 10, 10, 20, YELLOW);

    sprintf(hud_text, "COMBUSTIVEL: %d", (int)game->player.fuel); //mesma coisa
    Color fuelColor = (game->player.fuel < 30.0f) ? RED : LIME;
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, 10, 20, fuelColor);

    DrawRectangleLines(SCREEN_WIDTH/2 - 52, 35, 104, 8, WHITE);
    DrawRectangle(SCREEN_WIDTH/2 - 50, 37, (int)game->player.fuel, 4, fuelColor);

    sprintf(hud_text, "VIDAS: %d", game->player.lives); //mesma coisa
    DrawText(hud_text, SCREEN_WIDTH - 150, 10, 20, RED);

    sprintf(hud_text, "FASE: %d", game->current_phase); //mesma coisa
    DrawText(hud_text, SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT - 30, 16, WHITE);
}

void render_map(GameData* game) { //aqui ele vai lendo o mapa no arquivo txt e interpreta o que cada caractere significa, comecando sempre do ponto onde o player comeca até o final superior do mapa, mesmo que depois da linha de chegada
    int y_start = (int)game->camera_y;
    int y_end = y_start + SCREEN_GRID_HEIGHT + 1;

    for (int y = y_start; y < y_end; y++) {
        if (y < 0 || y >= game->current_phase_height) continue;
        for (int x = 0; x < MAP_WIDTH; x++) {
            char cell = game->map[y][x];
            int px = x * CELL_SIZE;
            int py = (int)get_screen_y(game, (float)y);

            //DESENHO DO TERRENO (Verde)
            if (cell == 'T') {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, DARKGREEN);
                //Opcional: desenhar borda pra ficar bonitinho
                DrawRectangleLines(px, py, CELL_SIZE, CELL_SIZE, (Color){0, 80, 0, 255});
            }
            // DESENHO DO COMBUSTÍVEL NO MAPA ('G')
            // Nota: Os inimigos móveis são desenhados em render_enemies, mas o mapa estático pode conter referências.
            // Como criamos objetos Enemy para o 'G', não precisamos desenhar aqui a menos que seja um 'G' que não virou inimigo (o que não deve acontecer).

            else if (cell == 'L') {
                DrawRectangle(px, py, CELL_SIZE, CELL_SIZE, WHITE);
                if (x == MAP_WIDTH / 2) DrawText("FIM", px + 2, py + 5, 15, BLACK);
            }
        }
    }
}

void render_player(GameData* game) {
    int px = (int)(game->player.x * CELL_SIZE);
    int py = (int)get_screen_y(game, game->player.y);

    // Verifica se a textura foi carregada corretamente
    if (game->texPlayer.id > 0) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)game->texPlayer.width, (float)game->texPlayer.height };
        // Destino define onde e qual tamanho na tela (CELL_SIZE)
        Rectangle destRec = { (float)px, (float)py, (float)CELL_SIZE, (float)CELL_SIZE };
        Vector2 origin = { 0.0f, 0.0f }; // Top-left
        DrawTexturePro(game->texPlayer, sourceRec, destRec, origin, 0.0f, WHITE);
    } else {
        // Fallback se não tiver imagem: Triângulo Amarelo
        Vector2 v1 = {(float)px + CELL_SIZE/2, (float)py + 5};
        Vector2 v2 = {(float)px + 5, (float)py + CELL_SIZE - 5};
        Vector2 v3 = {(float)px + CELL_SIZE - 5, (float)py + CELL_SIZE - 5};
        DrawTriangle(v1, v2, v3, YELLOW);
    }
}

void render_enemies(GameData* game) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) //verifica se o inimigo ta ativo
            continue; //o continue passa pra próxima iteração do loop

        int ex = (int)(game->enemies[i].x * CELL_SIZE);
        int ey = (int)get_screen_y(game, game->enemies[i].y);

        if (ey < -CELL_SIZE || ey > SCREEN_HEIGHT) //verifica se ta fora da tela
            continue;

        Texture2D* texToDraw = NULL;

        switch (game->enemies[i].type) {
            case ENTITY_SHIP:
                texToDraw = &game->texShip;
                break;
            case ENTITY_HELICOPTER:
                texToDraw = &game->texHeli;
                break;
            case ENTITY_BRIDGE_PIECE:
                texToDraw = &game->texBridge;
                break;
            case ENTITY_FUEL_STATION:
                texToDraw = &game->texFuel;
                break;
        }

        if (texToDraw && texToDraw->id > 0) {
             Rectangle sourceRec = { 0.0f, 0.0f, (float)texToDraw->width, (float)texToDraw->height };
             // Inimigos podem ter largura > 1 (tipo o posto de gasolina)
             float width = (float)CELL_SIZE * game->enemies[i].width;
             Rectangle destRec = { (float)ex, (float)ey, width, (float)CELL_SIZE };
             DrawTexturePro(*texToDraw, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
        } else {
            //Se nao conseguir abrir a imagem, desenha retangulos coloridos
            Color c = RED;
            if (game->enemies[i].type == ENTITY_FUEL_STATION) c = ORANGE;
            if (game->enemies[i].type == ENTITY_BRIDGE_PIECE) c = DARKGRAY;
            DrawRectangle(ex, ey, CELL_SIZE * game->enemies[i].width, CELL_SIZE, c);
        }
    }
}

void render_bullets(GameData* game) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!game->bullets[i].active) continue;
        int bx = (int)(game->bullets[i].x * CELL_SIZE);
        int by = (int)get_screen_y(game, game->bullets[i].y);

        if (by < 0 || by > SCREEN_HEIGHT) continue;

        if (game->texBullet.id > 0) {
            //Centraliza o tiro
            int offset = (CELL_SIZE - 10) / 2;
            DrawTexturePro(game->texBullet,
                           (Rectangle){0,0, (float)game->texBullet.width, (float)game->texBullet.height},
                           (Rectangle){(float)bx + offset, (float)by + offset, 10, 10},
                           (Vector2){0,0}, 0, WHITE);
        } else {
            DrawCircle(bx + CELL_SIZE/2, by + CELL_SIZE/2, 3, YELLOW);
        }
    }
}

void render_game_over(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("FIM DE JOGO!", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 80, 60, RED);
    char final_score[64];
    sprintf(final_score, "PONTUACAO: %d", game->player.score);
    DrawText(final_score, SCREEN_WIDTH/2 - 155, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    if (is_highscore(game, game->player.score)) {
        DrawText("NOVO RECORDE!", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 60, 30, LIME);
    }
    DrawText("Aperte ENTER", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT - 50, 20, WHITE);
}

void render_level_complete(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("NIVEL CONCLUIDO!", SCREEN_WIDTH/2 - 230, SCREEN_HEIGHT/2 - 60, 50, GREEN);
    char level_text[64];
    sprintf(level_text, "PROXIMA FASE: %d", game->current_phase);
    DrawText(level_text, SCREEN_WIDTH/2 - 135, SCREEN_HEIGHT/2 + 20, 30, YELLOW);
    DrawText("Aperte ENTER", SCREEN_WIDTH/2 - 70, SCREEN_HEIGHT/2 + 80, 20, WHITE);
}

void render_high_score(GameData* game) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 200});
    DrawText("REGISTRAR RECORDE", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT/2 - 100, 40, LIME);
    DrawText("DIGITE SEU NOME:", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 20, WHITE);
    DrawText(game->current_name, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 30, 24, YELLOW);
}

void render_game_won(GameData* game) {
    ClearBackground(BLACK);
    DrawText("VOCE VENCEU!", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 50, 60, GREEN);
    DrawText("Obrigado por jogar!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 20, 30, WHITE);
}
