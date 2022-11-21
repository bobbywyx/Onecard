#ifndef P2_PLAYER_H
#define P2_PLAYER_H

#include "data_struct.h"
#include "server.h"

void player_draw_card(Player *player,Server *server);
Card player_play_card(Player *player,int card_index);
void player_display_card(Player *player, char display_mode, FILE* fp);

#endif //P2_PLAYER_H
