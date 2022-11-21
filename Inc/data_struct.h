#ifndef P2_DATA_STRUCT_H
#define P2_DATA_STRUCT_H
#include <stdbool.h>
#include <stdio.h>

#define USERNAME_MAX_LEN 12

typedef enum _Suit{
    SPADE = 0,
    HEART,
    DIAMOND,
    CLUB
}Suit;

typedef enum _Ranks {
    R_Empty = 0,
    R_2 = 2,
    R_3,
    R_4,
    R_5,
    R_6,
    R_7,
    R_8,
    R_9,
    R_10,
    R_JACK,
    R_QUEEN,
    R_KING,
    R_ACE //14
}Rank;

typedef struct _Cards {
    Rank rank;
    Suit suit;
}Card;


typedef enum _ServerState {
    NORMAL,
    SCOREBOARD,
    PREPARE,
    GAME_OVER
}ServerState;

typedef struct _CLI {
    int lines;
    int cols;
    int lines_remain;
    int cols_remain;
}CLI;

typedef struct _Players{
    int id; // start at 0
    char name[USERNAME_MAX_LEN+1];
    int score;
    int card_num;
    int size_card_list; // start at 1
    Card *cards_in_hand;
    struct _Players *next;
    struct _Players *prev;

}Player;

typedef struct _Server {
    ServerState server_state;
    int round_index; // start at 1
    int rounds;
    Card* card_pool;
    int size_card_pool; // start at 1
    Card* discard_pool;
    int size_discard_pool;
    Player *head_player;
    int player_number;
    int cards_per_player;
    Player* current_player;
    int current_player_id;
    Card current_card;
    bool is_reversed;
    bool is_skipping;
    int addings;
    CLI *cli;
    bool auto_mode;
    FILE* fp_log;
}Server;



#endif //P2_DATA_STRUCT_H
