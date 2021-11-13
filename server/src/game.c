
#include "game.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>  
#include <sys/types.h>
#include <stdio.h> 

PlayerInfo *init_player_info(int jugador_actual){
    PlayerInfo* player_info = malloc(4*sizeof(PlayerInfo));
    player_info->player= jugador_actual;
    player_info->agr= 5;
    player_info->min= 5;
    player_info->ing= 5;
    player_info->gue= 5;
    player_info->niv_agr= 1;
    player_info->niv_min= 1;
    player_info->niv_gue= 1;
    player_info->niv_ing= 1;
    player_info->comida= 10;
    player_info->oro= 10;
    player_info->ciencia= 10;
    player_info->estado= 10;
    return player_info;
}