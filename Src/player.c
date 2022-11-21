#include <stdlib.h>
#include "player.h"
#include "data_struct.h"
#include "server.h"


void player_draw_card(Player *player, Server *server) {
    if (player->size_card_list == player->card_num) {
        // when the list is not long enough, expand it by 1.5
        player->size_card_list = (int) ((double )player->size_card_list * 1.5+1);
        player->cards_in_hand = (Card *) realloc(player->cards_in_hand,(size_t)player->size_card_list * sizeof(Card));
        if (player->cards_in_hand == NULL) {
            printf("realloc failed");
            exit(1);
        }
    }

    player->cards_in_hand[player->card_num] =server_pop_card(server);
    log_info(server->fp_log,false,"Player  %s  draw card   ",player->name);
    log_card(server->fp_log, player->cards_in_hand[player->card_num], 's');
    player->card_num++;
}

Card player_play_card(Player *player, int card_index) {
    Card tmp_card = player->cards_in_hand[card_index];

    Card *new_cards_in_hand = (Card *) calloc((size_t)(player->card_num - 1),sizeof(Card));\
    if (new_cards_in_hand == NULL) {
        printf("calloc failed");
        exit(1);
    }
    player->size_card_list = player->card_num - 1;

    // seam to list together
    for (int i = 0; i < card_index; ++i) {
        new_cards_in_hand[i] = player->cards_in_hand[i];
    }
    for (int i = card_index + 1; i < player->card_num; ++i) {
        new_cards_in_hand[i - 1] = player->cards_in_hand[i];
    }

    free(player->cards_in_hand);

    player->card_num--;
    player->cards_in_hand = new_cards_in_hand;
    return tmp_card;
}

void player_display_card(Player *player, char display_mode,FILE* fp) {
    if(display_mode == 's'){
        for (int i = 0; i < player->card_num; ++i) {
            log_card(fp, player->cards_in_hand[i], 's');
        }
    }
}
