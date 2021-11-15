#include "game.h"
#include <stdlib.h>


PlayerInfo* init_player_info(){
    PlayerInfo* player_info = malloc(sizeof(PlayerInfo));
    player_info -> oro = 0;
    player_info -> ciencia = 0;
    player_info -> comida = 0;
    player_info -> niv_agr = 1;
    player_info -> niv_gue = 1;
    player_info -> niv_ing = 1;
    player_info -> niv_min = 1;
    player_info -> agr = 0;
    player_info -> gue = 0;
    player_info -> ing = 0;
    player_info -> min = 0;
    player_info -> uid = -1;
    player_info -> aldeanos_initial = 9;
    return player_info;
}


void init_all_player_info(PlayerInfo** players_info){
    players_info[0] = init_player_info();
    players_info[1] = init_player_info();
    players_info[2] = init_player_info();
    players_info[3] = init_player_info();
    return;
}

void free_init_all(PlayerInfo** players_info){
    free(players_info[0]);
    free(players_info[1]);
    free(players_info[2]);
    free(players_info[3]);
}