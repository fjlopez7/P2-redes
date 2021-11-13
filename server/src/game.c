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
    player_info -> name = malloc(256*sizeof(char));
    player_info -> aldeanos_initial = 9;
    return player_info;
}


PlayerInfo** init_all_player_info(){
    PlayerInfo** player_info = malloc(4*sizeof(PlayerInfo*));
    player_info[0] = init_player_info();
    player_info[1] = init_player_info();
    player_info[2] = init_player_info();
    player_info[3] = init_player_info();
    return player_info;
}