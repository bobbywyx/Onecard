#ifndef P2_CLI_H
#define P2_CLI_H

#include "data_struct.h"
#define MIN_COLS 92
#define MIN_LINES 20



void cli_get_size(CLI *cli);
Player register_player(int id);
void clear_window();
int request_player_opt(CLI *cli);
void cli_draw_card(Card card,int x,int y);
void cli_display_scoreboard(CLI* cli,Server* server);
void cli_new_line(CLI* cli,int times);
void cli_draw_interface(CLI* cli,Server* server);
void cli_set_pos(CLI* cli);
void cli_display_played_card(CLI* cli,Card card);
void display_game_info(int n, int c, int d, int r, char log[100], FILE *fp);
void request_enter();




#endif //P2_CLI_H
