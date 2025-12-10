#include "game.h"

void handle_input(GameData* game) {
    if (IsKeyPressed(KEY_ESCAPE)) { //se a tecla ESC esta apertada e estiver jogando, volta pro menu e a musica para
        if (game->state == STATE_PLAYING) {
            game->state = STATE_MENU;
            game->trigger_music_stop = true;
        } else if (game->state == STATE_MENU) {//se ESC for apertada no menu o jogo fecha
            exit(0);
        }
    }

    switch (game->state) { //esse switch/case determina o que deve ser feito de acordo com o "estado" que o jogo está no momento
        case STATE_MENU: //se estiver no menu, chama a funcao que lida com o menu
            handle_menu_input(game);
            break;
        case STATE_PLAYING: //se estiver jogando, chama a funcao que lida com o jogo
            handle_gameplay_input(game);
            break;
        case STATE_PAUSED: //se estiver pausado, chama a funcao que lida com o pause (verifica que se apertar enter dnv ele sai do menu de pause
            if (IsKeyPressed(KEY_ENTER)) game->paused = 0;
            break;
        case STATE_GAME_OVER: //se estiver em GAME OVER, chama a funcao que lida com a tela de game over
            if (IsKeyPressed(KEY_ENTER)) {
                if (is_highscore(game, game->player.score)) { //se apertar ENTER e puder entrar nos highscores, registra o nome do player e chama a funcao que lida com isso
                    game->state = STATE_HIGH_SCORE;
                    game->name_length = 0;
                    game->current_name[0] = '\0';
                } else {
                    game->state = STATE_MENU; //se apertar enter e nao fez um novo recorde, volta pro menu
                    game->menu_selected = 0;
                }
            }
            break;
        case STATE_HIGH_SCORE: //se um novo recorde for feito pelo player, ele registra o nome que o player digitar
            {
                int chr = GetCharPressed();
                while (chr > 0) {
                    if (game->name_length < (int)sizeof(game->current_name) - 1 && chr >= 32 && chr <= 126) { //verifica se a string digitada contem algum caractere (numero, letra, ou especial) e registra
                        game->current_name[game->name_length++] = (char)chr;
                        game->current_name[game->name_length] = '\0';
                    }
                    chr = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && game->name_length > 0) { //se apertar BACKSPACE apaga o ultimo digito
                    game->name_length--;
                    game->current_name[game->name_length] = '\0';
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    if (game->name_length == 0) strcpy(game->current_name, "ANONIMO"); //se nao for escrito nenhum nome, registra como ANONIMO
                    add_highscore(game, game->current_name, game->player.score);
                    game->state = STATE_MENU;
                    game->menu_selected = 0;
                }
            }
            break;
        case STATE_LEVEL_COMPLETE:
            if (IsKeyPressed(KEY_ENTER)) {
                reset_level(game); //se o player passar de nivel, começa o proximo do inicio, por isso tem esse "reset"
                game->state = STATE_PLAYING;
            }
            break;
        case STATE_GAME_WON:
             if (IsKeyPressed(KEY_ENTER)) { //se o player venceu todos os niveis, chama a funcao menu depois de apertar enter
                game->state = STATE_MENU;
                game->menu_selected = 0;
                game->current_phase = 1; //volta para a "fase 1" mesmo que ainda esteja no menu
                initialize_game(game);
            }
            break;
        default: break;
    }
}

void handle_menu_input(GameData* game) { //determina o que os comandos do teclado fazem no menu
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) { //W ou seta para cima se movem "para cima" no menu
        game->menu_selected--;
        if (game->menu_selected < 0) game->menu_selected = 1;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) { //S ou seta para baixo se movem "para baixo" no menu
        game->menu_selected++;
        if (game->menu_selected > 1) game->menu_selected = 0;
    }
    if (IsKeyPressed(KEY_ENTER)) { //se apertar ENTER e estiver na posicao de "Novo jogo", o jogo inicia
        if (game->menu_selected == 0) {
            game->state = STATE_PLAYING;
            game->current_phase = 1;
            game->player.score = 0;
            game->player.lives = 3;
            game->player.fuel = 100.0f;
            game->trigger_music_start = true;
            reset_level(game);
        } else if (game->menu_selected == 1) {//se apertar ENTER e estiver na posicao de "Sair", ele fecha o jogo
            exit(0);
        }
    }
}

void handle_gameplay_input(GameData* game) {
    if (IsKeyPressed(KEY_ENTER)) game->paused = !game->paused; //se apertar ENTER ele pausa o jogo

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) game->player.x -= PLAYER_HORZ_SPEED; //se apertar A ou seta pra esquerda ele diminui a posicao do player (vai pra esuqerda)
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) game->player.x += PLAYER_HORZ_SPEED; //se apertar D ou seta pra diretia ele aumenta a posicao do player (vai pra direita)

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) game->current_scroll_speed = SCROLL_SPEED_FAST; //se apertar W ou seta pra cima ele aumenta a velocidade do scroll da tela
    else if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) game->current_scroll_speed = SCROLL_SPEED_SLOW; //se apertar S ou seta pra baixo ele diminui a velocidade do scroll da tela
    else game->current_scroll_speed = SCROLL_SPEED_NORMAL; //se nem W nem S sao apertados, ele continua com a velocidade normal

    if (IsKeyPressed(KEY_SPACE)|| IsKeyPressed(KEY_K)) shoot_bullet(game); //se aperta espaço ou K atira

    if (IsKeyPressed(KEY_R)) { //se apertar R o level reseta
        reset_level(game);
        game->player.fuel = 100.0f;//combustivel volta pro maximo
    }
}

void shoot_bullet(GameData* game) {
    if (game->player.rapid_fire_cooldown > 0) return;

    for (int i = 0; i < MAX_BULLETS; i++) { //essa funcao determina a os tiros
        if (!game->bullets[i].active) {
            game->bullets[i].x = game->player.x; //determina que o tiro vai sair do mesmo x que o player
            game->bullets[i].y = game->player.y - 1; //determina que o tiro vai sair de "dentro" do player
            game->bullets[i].active = true;
            game->player.rapid_fire_cooldown = MAX_RAPID_FIRE_COOLDOWN;
            game->trigger_shoot_sound = true;
            break;
        }
    }
}
