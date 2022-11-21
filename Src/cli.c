//
// Created by yixin on 2022/10/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include "main.h"
#include "cli.h"
#include "server.h"

// color define
#define F_RED "\033[31m"
#define F_GREEN "\033[32m"
#define F_YELLOW "\033[33m"
#define F_BLUE "\033[34m"
#define B_WHITE "\033[47m"
#define B_BLACK "\033[40m"
#define B_BROWN "\033[43m"
#define B_CYAN "\033[46m"
#define B_LIGHT_BLUE "\033[104m"
#define B_DARK_GRAY "\033[100m"
#define B_BLACK "\033[40m"
#define B_WHITE "\033[47m"

#define REVERSE "\033[7m"
#define RESET "\033[0m"


void cli_get_size(CLI *cli) {
    int columns = 0, rows = 0;
#if defined(_WIN32) || defined(__CYGWIN__)
    // Windows (x86 or x64)
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

//    printf("columns: %d\n", columns);
//    printf("rows: %d\n", rows);

#elif defined(__linux__)
    // Linux
    struct winsize w = {0};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

//    printf ("lines %d\n", w.ws_row);
//    printf ("columns %d\n", w.ws_col);
    columns = w.ws_col;
    rows = w.ws_row;

#elif defined(__APPLE__) && defined(__MACH__)
    // Mac OS

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
//    printf ("lines %d\n", w.ws_row);
//    printf ("columns %d\n", w.ws_col);
    columns = w.ws_col;
    rows = w.ws_row;

#elif defined(unix) || defined(__unix__) || defined(__unix)
    // Unix like OS
    // not supported yet
#else
#error Unknown environment!
#endif
    cli->cols = columns;
    cli->lines = rows;
    cli->lines_remain = rows;
    cli->cols_remain = columns;
}

Player register_player(int id) {
    //// init player and its id, name, score, card_num, cards_in_hand, size_card_list
    char name[1000] = "";

    Player p;
    p.id = id;

    printf("\nregister id:%d  player, enter a name(no longer than %d)\nname:", id, USERNAME_MAX_LEN);
    if (scanf("%s", name) && strlen(name) <= USERNAME_MAX_LEN) {

        strcpy(p.name, name);
        printf("Player registered successfully:\n"
               "name:  %s    id: %d\n"
               "-------------------------------------------------------\n\n\n", name, id);
    } else {
        printf("wrong input, please try again\n");
        strcpy(p.name, "__WRONG__");
    }
    p.score = 0;
    p.card_num = 0;
    p.size_card_list = 10;
    p.cards_in_hand = (Card *) calloc(10, sizeof(Card));
    if (p.cards_in_hand == NULL) {
        printf("player cards_in_hand calloc failed");
        exit(1);
    }
    return p;
}

void clear_window() {
#if defined(_WIN32) || defined(__CYGWIN__)
    // Windows (x86 or x64)
    system("cls");
#elif defined(__linux__)
    // Linux
    system("clear");
#elif defined(__APPLE__) && defined(__MACH__)
    // Mac OS
    system("clear");
#elif defined(unix) || defined(__unix__) || defined(__unix)
    // Unix like OS

#else
#error Unknown environment!
#endif
}

int request_player_opt(CLI *cli) {
    int opt = -2;
    printf("\033[%d;%dH", cli->lines - cli->lines_remain, 1);
    printf("give your choice(0 to draw card):  ");
    if (scanf("%d", &opt)) {
        printf("your choice: %d\n", opt);
    }
    return opt;
}

void cli_draw_card(Card card, int x, int y) {
//    printf("\033[7m"); // reverse display

    printf(B_BLACK); // background

    switch (card.suit) {
        case SPADE:
            printf(F_RED);
            printf("\033[%d;%dH", y + 2, x);     //set position
            printf("| :/\\: |");
            printf("\033[%d;%dH", y + 3, x);
            printf("| (__) |");
            break;
        case HEART:
            printf(F_BLUE);
            printf("\033[%d;%dH", y + 2, x);
            printf("| (\\/) |");
            printf("\033[%d;%dH", y + 3, x);
            printf("| :\\/: |");
            break;
        case DIAMOND:
            printf(F_GREEN);
            printf("\033[%d;%dH", y + 2, x);
            printf("| :/\\: |");
            printf("\033[%d;%dH", y + 3, x);
            printf("| :\\/: |");
            break;
        case CLUB:
            printf(F_YELLOW);
            printf("\033[%d;%dH", y + 2, x);
            printf("| :(): |");
            printf("\033[%d;%dH", y + 3, x);
            printf("| ()() |");
            break;
    }


    printf("\033[%d;%dH", y, x);
    printf(".------.");
    printf("\033[%d;%dH", y + 5, x);
    printf("`------'");

    // print rank
    printf("\033[%d;%dH", y + 1, x);
    switch (card.rank) {
        case R_10:
            printf("|10--. |");
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--10|");
            break;
        case R_JACK:
            printf("|J.--. |");
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--'J|");
            break;
        case R_QUEEN:
            printf("|Q.--. |");
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--'Q|");
            break;
        case R_KING:
            printf("|K.--. |");
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--'K|");
            break;
        case R_ACE:
            printf("|A.--. |");
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--'A|");
            break;
        default:
            printf("|%d.--. |", (int) card.rank);
            printf("\033[%d;%dH", y + 4, x);
            printf("| '--'%d|", (int) card.rank);
            break;
    }

//    printf(F_RED);
//    printf(B_WHITE);
//    printf("\033[20;20H");
//    printf("cli_draw %d %d\n",card.suit,card.rank);
//    char frame[cli->lines-8][cli->cols+1];


    printf(RESET);
}

void cli_display_scoreboard(CLI *cli, Server *server) {
    clear_window();
    if (!server->auto_mode)request_enter();
    log_info(server->fp_log,true,
             "\n==========================="
             "\n==========================="
             "\n==========================="
             "\nscore board\n");
    cli->cols_remain--;
    for (int i = 0; i < server->player_number; ++i) {
        log_info(server->fp_log,true,"%s : %d scores\n", server_seek_by_id(server, i)->name, server_seek_by_id(server, i)->score);
    }
    log_info(server->fp_log,true,
             "\n==========================="
             "\n==========================="
             "\n===========================\n");

}

void cli_new_line(CLI *cli, int times) {
    cli->cols_remain = cli->cols;
    if (cli->lines_remain == 0) {
        printf(F_RED);
        printf("\nthe window is too small, something might seems weired\n");
        printf(RESET);
    }
    for (int i = 0; i < times; ++i) {
        printf("\n");
        cli->lines_remain--;
    }
}

void cli_draw_interface(CLI *cli, Server *server) {
    clear_window();
    if (!server->auto_mode)request_enter();
    clear_window();
    cli_get_size(cli); // in case someone resize the window
//    clear_window();
    switch (server->server_state) {
        case NORMAL:
            printf("Onecard");
            cli->cols_remain -= 7;
            int names_len = (int) strlen(server->current_player->name)
                            + (int) strlen(get_next_player(server)->name)
                            + (int) strlen(get_prev_player(server)->name)
                            + 6;
            int spaces_to_center = (cli->cols_remain - names_len) / 2;
            if (spaces_to_center < 0)spaces_to_center = 1; // nospaces left, print anyway
            // get to center
            printf("%*s", spaces_to_center, "");
            cli->cols_remain -= spaces_to_center;
            // print user name
            printf("%s->", get_prev_player(server)->name);
            printf(REVERSE);
            printf("%s", server->current_player->name);
            printf(RESET);
            printf("->%s", get_next_player(server)->name);
            cli->cols_remain -= names_len;

            //get to the end
            int score_len = (int) log10(-server->current_player->score + 1);
            int spaces_to_end = cli->cols_remain - score_len - 6; // -6 is "score:"
            if (spaces_to_end < 0)spaces_to_end = 1; // nospaces left, print anyway
            printf("%*s", spaces_to_end, "");

            // print score
            printf("score:%d", server->current_player->score);
            cli->cols_remain -= 2;


            cli->cols_remain = cli->cols;
            cli_new_line(cli, 1);

            // last played card
            printf("last card:  addings:%d", server->addings);
            cli->cols_remain -= 20 + (int) log10(server->addings + 1) - 1;
            int pool_info_len =
                    (int) log10(server->size_card_pool + 1) + (int) log10(server->size_discard_pool + 1) + 18;
            spaces_to_end = cli->cols_remain - pool_info_len;
            if (spaces_to_end < 0)spaces_to_end = 1; // nospaces left, print anyway
            printf("%*s", spaces_to_end, "");
            printf("pool:%d,discard:%d", server->size_card_pool, server->size_discard_pool);

            cli_draw_card(server->current_card, 1, 3);
            cli_new_line(cli, 6);

            // all cards
            cli_set_pos(cli);
            printf("your cards :%d ", server->current_player->card_num);
            int max_in_line = cli->cols / 8;
            if (max_in_line == 0)max_in_line = 1;
            if (server->current_player->card_num / max_in_line > 4) {
                printf(F_RED);
                printf("\ntoo many cards, some might be hidden");
                printf(RESET);
                cli_new_line(cli, 1);
            }

            int x=0,y=0;
            cli_new_line(cli, 1);
            for (int i = 0; i < server->current_player->card_num; ++i) {
//                cli_draw_card(server->current_player->cards_in_hand[i],i*8+1,cli->lines-cli->lines_remain+2);
                if (i / max_in_line > 4)break;
                x = i % max_in_line * 8 + 1;
                y = cli->lines - cli->lines_remain + 2 + (i / max_in_line) * 2;
                cli_draw_card(server->current_player->cards_in_hand[i],x,y);
            }
            cli_new_line(cli, 6);
            cli_new_line(cli, 2 * server->current_player->card_num / max_in_line);


            // playable cards
            cli_set_pos(cli);
            printf("your choice : ");
            cli_new_line(cli, 1);
            Card *playable_cards = get_playable_cards(server);
            if(get_card_list_length(playable_cards) ==0 || server->is_skipping){
                printf("you can't play any card");
            } else {
                for (int i = 0; i < get_card_list_length(playable_cards); ++i) {
//                cli_draw_card(get_playable_cards(server)[i],i*8+1,cli->lines-cli->lines_remain+2);
                    if (i / max_in_line > 4)break;
                    x = i % max_in_line * 8 + 1;
                    y = cli->lines - cli->lines_remain + 2 + (i / max_in_line) * 2;
                    cli_draw_card(playable_cards[i], x, y);
                    printf("\033[%d;%dH", y, x);
                    printf("%d", i + 1);
                }
                cli_new_line(cli, 2 * get_card_list_length(playable_cards) / max_in_line);
                cli_new_line(cli, 6);
            }

            // print the rest of the window
            cli_set_pos(cli);
            cli_new_line(cli, cli->lines_remain - 2);
            // then the request player opt will be called
            free(playable_cards);


            break;
        case SCOREBOARD:
            cli_display_scoreboard(cli,server);
            break;
        case PREPARE:
            break;
        case GAME_OVER:
            break;
    }
}

void cli_set_pos(CLI *cli) {
    printf("\033[%d;%dH", cli->lines - cli->lines_remain + 2, cli->cols - cli->cols_remain);
}

void cli_display_played_card(CLI *cli, Card card) {
    cli_get_size(cli);
    printf("played card:");
    cli_new_line(cli, 1);
    cli_draw_card(card, 1, 2);
    cli_new_line(cli, 6);
}

void display_game_info(int n, int c, int d, int r, char log[100], FILE *fp) {
//    printf("game info:\n");
//    printf("rounds: %d\n", r);
//    printf("players: %d\n", n);
//    printf("initial cards: %d\n", c);
//    printf("decks of cards: %d\n", d);
//    printf("log: %s\n", log);
    log_info(fp,true,"game info:\n");
    log_info(fp,true,"rounds: %d\n", r);
    log_info(fp,true,"players: %d\n", n);
    log_info(fp,true,"initial cards: %d\n", c);
    log_info(fp,true,"decks of cards: %d\n", d);
    log_info(fp,true,"log: %s\n", log);
}

void request_enter() {
    printf("press enter to continue\n");
    getchar();
    getchar();
}


