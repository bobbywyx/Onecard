#ifndef P2_SERVER_H
#define P2_SERVER_H

#include "data_struct.h"
#include "cli.h"


void server_init(int player_number,int cards_per_player,int decks,int round,bool auto_mode,char log_file_name[]);
void server_state_machine(Server* server);
void server_init_player_list(Server* server);
void server_add_player(Server *server,int id);
void server_decide_player_seq(Server* server);
Player* server_seek_by_id(Server* server,int id);
Card server_pop_card(Server* server);
void server_recycle_card(Server *server, Card card) ;
Card* create_cards_list(int decks);
void server_shuffle_card(Card* card_list,int card_num);
void server_deal_card(Server* server);
void log_card(FILE *fp, Card card, char mode);
Card* sort_cards(Card* card_list,int size);
void server_main_loop(Server *server);
void server_delete_player(Server *server, Player *p);
Player* get_next_player(Server* server);
Player* get_prev_player(Server *server);
Card* get_playable_cards(Server *server);
int get_card_list_length(Card* list);
int find_card_in_list(Card *list, Card card);
void log_info(FILE* fp,bool if_print_to_cli,const char *__restrict info, ...);
#endif //P2_SERVER_H
