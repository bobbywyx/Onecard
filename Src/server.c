#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include "data_struct.h"
#include "server.h"
#include "player.h"
#include "cli.h"

#define WINDOW_REQ

void server_init(int player_number, int cards_per_player, int decks, int round,bool auto_mode,char log_file_name[]) {
    srand((unsigned int) time(NULL));  // each game only need to generate once


    // pre-check
    if ((cards_per_player + 1 + 10) * player_number > decks * 52) {
        // there is possibility that every player play some +2 then the game has no cards in pile
        printf("too many players or too many cards per player\n");
        exit(1);
    }
    CLI cli = {.lines =  0, .cols =  0, .lines_remain = 0, .cols_remain = 0};
    cli_get_size(&cli);

#ifdef WINDOW_REQ
    if(cli.cols <= MIN_COLS || cli.lines <= MIN_LINES){
        printf("cli window too small, the minimum requirement is:\n%d columns and %d lines\n"
               "current:%d columns and %d lines\n",MIN_COLS,MIN_LINES,cli.cols,cli.lines);
        exit(4);
    }
    printf("press enter to start\n");
    getchar();
    clear_window();
#endif


    Card *cards_list = create_cards_list(decks);
    Card *discard_pool = (Card *) calloc((size_t) decks * 52, sizeof(Card));
    if (discard_pool == NULL) {
        printf("calloc failed");
        exit(1);
    }
    // finally, all cards may go to the discard poll so the size can be decided now.
    server_shuffle_card(cards_list, decks * 52);

    FILE *fp = fopen(log_file_name,"w");
    if(fp == NULL)printf("write log file error");

    Server server = {
            .server_state = PREPARE,
            .round_index = 1,
            .rounds =  round,
            .card_pool =  cards_list,
            .size_card_pool =52 * decks,
            .discard_pool = discard_pool,
            .size_discard_pool = 0,
            .head_player =  NULL,
            .player_number =  player_number,
            .cards_per_player =  cards_per_player,
            .current_player = NULL,
            .current_player_id =  0,
            .current_card = {.suit = SPADE, .rank = R_Empty},
            .is_reversed = false,
            .is_skipping = false,
            .addings = 0,
            .cli = &cli,
            .auto_mode = auto_mode,
            .fp_log = fp
    };



    server_init_player_list(&server);
    for (int i = 1; i < player_number; ++i) {
        server_add_player(&server, i);
    }
    server.current_player = server.head_player;
    server.current_player_id = 0; // reset to the head player

    // log basic info
    display_game_info(player_number, cards_per_player, decks, round, log_file_name, server.fp_log);

    if(!auto_mode)request_enter();

    server_main_loop(&server);
}

void server_state_machine(Server *server) {
    Player *current_player = server_seek_by_id(server, server->current_player_id);
    switch (server->server_state) {
        case NORMAL:
            current_player->cards_in_hand = sort_cards(current_player->cards_in_hand, current_player->card_num);

            cli_draw_interface(server->cli, server);
            log_info(server->fp_log,false,"------------------------\n");
            log_info(server->fp_log,false,"Player: %s\n", server->current_player->name);
            player_display_card(current_player,'s',server->fp_log);

            if (server->is_skipping) {
                log_info(server->fp_log,true,"you are being skipped, press enter to continue\n");
                if(!server->auto_mode)getchar();
                server->is_skipping = false;
            } else {
                Card *playable_cards = get_playable_cards(server);
                int opt = 0;
                if(server->auto_mode){
                    opt = get_card_list_length(playable_cards);
                } else {
                    do {
                        opt = request_player_opt(server->cli);
                    } while (opt == -2 || opt >= get_card_list_length(playable_cards) + 1); // -2 means invalid input
                }

                if (opt == 0) {  // draw a card
                    if (server->addings > 0) { // if there is +2 or +3 or more
                        for (; server->addings > 0; server->addings--) {
                            player_draw_card(current_player, server);
                        }
                    } else {
                        player_draw_card(current_player, server);
                    }

                } else { // play card
                    opt--; // start form 1 --> 0
                    Card played_card = player_play_card(current_player,
                                                        find_card_in_list(server->current_player->cards_in_hand,
                                                                          playable_cards[opt]));
                    server_recycle_card(server, server->current_card);
                    if (played_card.rank == R_2) {
                        server->addings += 2;
                    } else if (played_card.rank == R_3) {
                        server->addings += 3;
                    } else if (played_card.rank == R_7) {
                        server->addings = 0;
                    } else if (played_card.rank == R_QUEEN) { // reverse
                        server->is_reversed = !server->is_reversed;
                    } else if (played_card.rank == R_JACK) { // skip
                        if(server->current_card.rank == R_2 || server->current_card.rank == R_3){
                            server->is_skipping = false; // give the addings to next player
                        } else{
                            server->is_skipping = true;
                        }
                    }
                    server->current_card = played_card;
                    cli_display_played_card(server->cli, played_card);
                    log_info(server->fp_log,true,"player %s play card:  ",server->current_player->name);
                    log_card(server->fp_log,played_card,'s');
                }
                free(playable_cards);
            }

            if (current_player->card_num == 0) {
                // this player win
                server->server_state = SCOREBOARD;
                break;
            }

            server->current_player = get_next_player(server);
            server->current_player_id = server->current_player->id;

            break;
        case SCOREBOARD:
            for (int i = 0; i < server->player_number; ++i) {
                server_seek_by_id(server, i)->score -= server_seek_by_id(server, i)->card_num;
            }
            cli_display_scoreboard(server->cli, server);
            server->server_state = PREPARE;
            scanf("press enter to continue");
            server->round_index++;
            break;
        case PREPARE:
//            for (int i = 0; i < server->player_number; ++i) {
//                server_pop_card()
//            }
            server_deal_card(server);
            server_decide_player_seq(server);
            server->current_card = server_pop_card(server);
            server->server_state = NORMAL;
            break;
        case GAME_OVER:

            printf("game over\nPress enter to continue");
            if(!server->auto_mode)request_enter();
            break;
    }

}

void server_init_player_list(Server *server) {
    //// this function will init the double linked list of players and set the current player to the first player
    Player *head = (Player *) calloc(1, sizeof(Player));
    if (head == NULL) {
        printf("failed to allocate head player");
        exit(1);
    }
    *head = register_player(0);
    head->next = head;
    head->prev = head;
    server->current_player_id = 0;
    server->current_player = head;
    server->head_player = head; // this will always point to the head of the list
}

void server_add_player(Server *server, int id) {
    Player *new_player = (Player *) calloc(1, sizeof(Player));
    if (new_player == NULL) {
        printf("failed to allocate new player");
        exit(1);
    }
    *new_player = register_player(id);
    server->current_player->next->prev = new_player;
    new_player->next = server->current_player->next;
    new_player->prev = server->current_player;
    server->current_player->next = new_player;
    server->current_player = server->current_player->next;
    server->current_player_id++;
}

void server_decide_player_seq(Server *server) {
    Card *card_list = calloc((size_t) server->player_number, sizeof(Card));
    if(card_list == NULL){
        printf("failed to allocate card list");
        exit(1);
    }
//    int order[(size_t)server->player_number];
    int *order = calloc((size_t) server->player_number, sizeof(int));
    if(order == NULL){
        printf("calloc error");
        exit(1);
    }
    for (int i = 0; i < server->player_number; ++i) {
        order[i] = i;
        card_list[i] = server_pop_card(server);
        server_recycle_card(server,card_list[i]);

    }
#ifdef DEBUG3
    // before sort
    for (int i = 0; i < server->player_number; ++i) {
        display_card(card_list[i], 's');
    }
#endif
    // sort
    for (int i = 0; i < server->player_number; ++i) {
        for (int j = i + 1; j < server->player_number; ++j) {
            if (card_list[order[i]].suit > card_list[order[j]].suit) {
                int tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            } else if (card_list[order[i]].suit == card_list[order[j]].suit) {
                if (card_list[order[i]].rank > card_list[order[j]].rank) {
                    int tmp = order[i];
                    order[i] = order[j];
                    order[j] = tmp;
                }
            }
        }
    }

    Player **tmp_list = calloc((size_t) server->player_number, sizeof(Player *));
    if(tmp_list == NULL){
        printf("calloc error");
        exit(1);
    }
    for (int i = 0; i < server->player_number; ++i) {
        tmp_list[i] = server_seek_by_id(server, order[i]);
    }

    server->current_player_id = order[0]; // the player with the smallest card will start the turn
    server->current_player = tmp_list[0];

    // link player linked list
    server->current_player->next = tmp_list[1];
    server->current_player->prev = tmp_list[server->player_number - 1];

    for (int i = 1; i < server->player_number - 1; ++i) {
        tmp_list[i]->next = tmp_list[i + 1];
        tmp_list[i]->prev = tmp_list[i - 1];
    }
    tmp_list[server->player_number - 1]->next = tmp_list[0];
    tmp_list[server->player_number - 1]->prev = tmp_list[server->player_number - 2];




//    server->player_list[server->player_number-1].next = &server->player_list[order[0]];
    free(order);
    free(card_list);
    free(tmp_list);
}

Player *server_seek_by_id(Server *server, int id) {
    Player *p = server->head_player;
    while (p->id != id) {
        p = p->next;
        if (p->id > server->player_number) {
            printf("error: player id not found");
            exit(3);
        }
    }
    return p;
}

Card server_pop_card(Server *server) {
    // return the last card of the pool and move the num pointer
    server->size_card_pool--;
    Card return_card = server->card_pool[server->size_card_pool];


    Card *tmp_list =NULL;
    if (server->size_card_pool == 0) {
        // the card pool is empty, refill it with the discard pool
        server->size_card_pool = server->size_discard_pool;
        server->size_discard_pool = 0;
        tmp_list = server->discard_pool;
        server->discard_pool = server->card_pool;
        server->card_pool = tmp_list;
    } else {
        server->card_pool = (Card *) realloc(server->card_pool, (size_t) server->size_card_pool * sizeof(Card));
        if (server->card_pool == NULL) {
            printf("realloc error\n");
            exit(1);
        }
    }

    return return_card;
}

void server_recycle_card(Server *server, Card card) {
    server->size_discard_pool++;
    server->discard_pool = (Card *) realloc(server->discard_pool, (size_t) server->size_discard_pool * sizeof(Card));
    if (server->discard_pool == NULL) {
        printf("realloc error\n");
        exit(1);
    }
    server->discard_pool[server->size_discard_pool - 1] = card;
}

Card *create_cards_list(int decks) {
    Card *cards = NULL;
    cards = (Card *) calloc((size_t) decks * 52, sizeof(Card)); // every deck have 52 cards
    if (cards == NULL) {
        printf("calloc failed");
        exit(1);
    }
    int index = 0;
    for (int i = 0; i < decks; ++i) {
        for (int suit = 0; suit < 4; ++suit) {
            for (int rank = 2; rank < 15; ++rank) {
                Card c = {(Rank) rank, (Suit) suit};
                cards[index] = c;
                index++;
            }
        }
    }
    return cards;
}

void server_shuffle_card(Card *card_list, int card_num) {
    for (int i = 0; i < card_num; ++i) {
        int r = rand() % card_num;
        // swap
        Card tmp = card_list[r];
        card_list[r] = card_list[i];
        card_list[i] = tmp;
    }
}

void server_deal_card(Server *server) {
    for (int i = 0; i < server->player_number; ++i) {
        for (int j = 0; j < server->cards_per_player; ++j) {
            player_draw_card(server_seek_by_id(server, i), server);
        }
    }
}

void log_card(FILE *fp, Card card, char mode) {
    // i for integer    s for simple
    switch (mode) {
        case 's':
            switch (card.suit) {
                case SPADE:
                    log_info(fp,false,"SPADE    ");
                    break;
                case HEART:
                    log_info(fp,false,"HEART    ");
                    break;
                case DIAMOND:
                    log_info(fp,false,"DIAMOND  ");
                    break;
                case CLUB:
                    log_info(fp,false,"CLUB     ");
                    break;
            }
            switch (card.rank) {
                case R_ACE:
                    log_info(fp,false,"ACE\n");
                    break;
                case R_JACK:
                    log_info(fp,false,"JACK\n");
                    break;
                case R_QUEEN:
                    log_info(fp,false,"QUEEN\n");
                    break;
                case R_KING:
                    log_info(fp,false,"KING\n");
                    break;
                default:
                    log_info(fp,false,"%d\n", card.rank);
            }
            break;
        case 'i':    // integer output
            break;
        default:
            log_info(fp,false,"%d %d\n", card.suit, card.rank);
    };
}

Card *sort_cards(Card *card_list, int size) {
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            if (card_list[i].suit > card_list[j].suit) {
                Card tmp = card_list[i];
                card_list[i] = card_list[j];
                card_list[j] = tmp;
            } else if (card_list[i].suit == card_list[j].suit) {
                if (card_list[i].rank > card_list[j].rank) {
                    Card tmp = card_list[i];
                    card_list[i] = card_list[j];
                    card_list[j] = tmp;
                }
            }
        }
    }
    return card_list;
}

void server_main_loop(Server *server) {
    // one loop is one player turn
    while (server->round_index <= server->rounds) {
        server_state_machine(server);
    }
    free(server->card_pool);
    free(server->discard_pool);
    fclose(server->fp_log);
//    free_players(server->head_player);
    while (server->player_number > 0) {
        server_delete_player(server, server->head_player->next);
    }

}

void server_delete_player(Server *server, Player *p) {
    // delete a player from the server
    if (server->head_player == p) {
        server->head_player = p->next;
    }
    p->prev->next = p->next;
    p->next->prev = p->prev;
    free(p->cards_in_hand);
    free(p);
    p = NULL;
    server->player_number--;
}

Player *get_next_player(Server *server) {
    if (server->is_reversed) {
        return server->current_player->prev;
    } else {
        return server->current_player->next;
    }
}

Player* get_prev_player(Server *server) {
    if (server->is_reversed) {
        return server->current_player->next;
    } else {
        return server->current_player->prev;
    }
}

Card *get_playable_cards(Server *server) {
    if (server->current_card.rank == R_Empty) {
        printf("no card in pull");
        exit(2);
    }
    Player *current_player = server_seek_by_id(server, server->current_player_id);
    Card *playable_cards = (Card *) calloc((size_t) current_player->card_num + 1, sizeof(Card));
    if (playable_cards == NULL) {
        printf("card calloc error");
        exit(3);
    }
    Card tmp_card;
    int nums = 0; // playable cards nums


    switch (server->current_card.rank) {
        case R_2:
        case R_3:
            if (server->addings > 0) {
                // last player just play attack card and this player have to play function cards
                for (int i = 0; i < current_player->card_num; ++i) {
                    tmp_card = current_player->cards_in_hand[i];
                    if (tmp_card.rank == R_2 || tmp_card.rank == R_3 || tmp_card.rank == R_7 ||
                        (tmp_card.rank == R_QUEEN && tmp_card.suit == server->current_card.suit) ||
                        (tmp_card.rank == R_JACK && tmp_card.suit == server->current_card.suit)) {
                        playable_cards[nums] = tmp_card;
                        nums++;
                    }
                }
            } else {
                // just fix the suit
                for (int i = 0; i < current_player->card_num; ++i) {
                    tmp_card = current_player->cards_in_hand[i];
                    if (tmp_card.suit == server->current_card.suit) {
                        playable_cards[nums] = tmp_card;
                        nums++;
                    }
                }
            }
            break;
        default:
            if (server->addings > 0) {
                // last player just play attack card and this player have to play function cards
                for (int i = 0; i < current_player->card_num; ++i) {
                    tmp_card = current_player->cards_in_hand[i];
                    if (tmp_card.rank == R_2 || tmp_card.rank == R_3 || tmp_card.rank == R_7 ||
                        (tmp_card.rank == R_QUEEN && tmp_card.suit == server->current_card.suit) ||
                        (tmp_card.rank == R_JACK && tmp_card.suit == server->current_card.suit)) {
                        playable_cards[nums] = tmp_card;
                        nums++;
                    }
                }
            } else{
                for (int i = 0; i < current_player->card_num; ++i) {
                    tmp_card = current_player->cards_in_hand[i];
                    if (tmp_card.suit == server->current_card.suit || tmp_card.rank == server->current_card.rank) {
                        playable_cards[nums] = tmp_card;
                        nums++;
                    }
                }
            }
            break;

    }
    playable_cards = (Card *) realloc(playable_cards, (size_t) (nums + 1) * sizeof(Card));
    // this list should be shorter after realloc, no need to set to zero, and the last element is (0,0)
    if (playable_cards == NULL) {
        printf("card realloc error");
        exit(3);
    }

    playable_cards[nums].rank = R_Empty;
    return playable_cards;
}

int get_card_list_length(Card *list) {
    // this function is just for the list returned by get_playable_card func (this list don't have a var to store its int)
    int i = 0;
    while (list[i].rank != R_Empty) {
        i++;
    }
    return i;
}

int find_card_in_list(Card *list, Card card) {
    int i = 0;
    while (list[i].rank != R_Empty) {
        if (list[i].rank == card.rank && list[i].suit == card.suit) {
            return i;
        }
        i++;
    }
    return -1;
}

void log_info(FILE* fp,bool if_print_to_cli,const char *__restrict info, ...){
    va_list args;
    va_start(args, info);
    vfprintf(fp, info, args);
    va_end(args);
    if (if_print_to_cli){
        va_start(args, info);
        vprintf(info, args);
        va_end(args);
    }
}
