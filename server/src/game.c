#include "game.h"
#include <stdlib.h>


PlayerInfo* init_player_info(){
    PlayerInfo* player_info = malloc(sizeof(PlayerInfo));
    player_info -> oro = 0;
    player_info -> ciencia = 0;
    player_info -> comida = 0;
    player_info -> niv_agr = 1;
    player_info -> niv_atq = 1;
    player_info -> niv_def = 1;
    player_info -> niv_ing = 1;
    player_info -> niv_min = 1;
    player_info -> agr = 0;
    player_info -> gue = 0;
    player_info -> ing = 0;
    player_info -> min = 0;
    player_info -> uid = -1;
    player_info -> status = 1; //1: vivo, 0: muerto
    player_info -> first = 1;
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

void add_resources(PlayerInfo* player_info){
    int oro = player_info->min * player_info->niv_min * 2;
    int comida = player_info->agr * player_info->niv_agr * 2;
    int ciencia = player_info->ing * player_info->niv_ing;

    player_info->oro += oro;
    player_info->comida += comida;
    player_info->ciencia += ciencia;
    return;
}

// recibe el tipo de clase que desea crear, va del 1 al 4 // -1: input incorrecto, 0: no cuenta con los recursos, 1: ok
int create_villager(PlayerInfo* player_info, int type){
    int oro_necesario;
    int comida_necesaria;

    // Agricultor
    if(type == 1){
        oro_necesario = 0;
        comida_necesaria = 10;
    // Minero
    } else if(type == 2){
        oro_necesario = 5;
        comida_necesaria = 10;
    // Ingeniero
    } else if(type == 3){
        oro_necesario = 10;
        comida_necesaria = 20;
    // Guerrero
    } else if(type == 4){
        oro_necesario = 10;
        comida_necesaria = 10;
    // input incorrecto
    } else {
        return -1;
    }

    if(player_info->oro >= oro_necesario && player_info->comida >= comida_necesaria){
        player_info->oro -= oro_necesario;
        player_info->comida -= comida_necesaria;
        // Agricultor
        if(type == 1){
            player_info->agr++;
        // Minero
        } else if(type == 2){
             player_info->min++;
        // Ingeniero
        } else if(type == 3){
             player_info->ing++;
        // Guerrero
        } else if(type == 4){
             player_info->gue++;
        }
        return 1;
    // No cuenta con los recursos necesarios
    } else {
        return 0;
    }
}

// recibe el tipo de clase que desea crear, va del 1 al 5 //-2: nivel maximo alcanzado, -1: input incorrecto, 0: no cuenta con los recursos, 2-5: indica el nuevo nivel
int level_up(PlayerInfo* player_info, int type){
    int nivel_actual;

    // Agricultor
    if(type == 1){
        nivel_actual = player_info->niv_agr;
    // Minero
    } else if(type == 2){
        nivel_actual = player_info->niv_min;
    // Ingeniero
    } else if(type == 3){
        nivel_actual = player_info->niv_ing;
    // Ataque
    } else if(type == 4){
        nivel_actual = player_info->niv_atq;
    // Defensa
    } else if(type == 5){
        nivel_actual = player_info->niv_def;
    // input incorrecto
    } else {
        return -1;
    }

    // Se encuentra en el maximo nivel
    if (nivel_actual == 5){
        return -2;
    }

    int recurso_necesario = nivel_actual * 10;;

    if(player_info->oro >= recurso_necesario && player_info->comida >= recurso_necesario && player_info->ciencia >= recurso_necesario){
        player_info->oro -= recurso_necesario;
        player_info->comida -= recurso_necesario;
        player_info->ciencia -= recurso_necesario;
        // Agricultor
        if(type == 1){
            player_info->niv_agr++;
        // Minero
        } else if(type == 2){
             player_info->niv_min++;
        // Ingeniero
        } else if(type == 3){
             player_info->niv_ing++;
        // Guerrero
        } else if(type == 4){
             player_info->niv_atq++;
        // Defensa
        } else if(type == 5){
            player_info->niv_def++;
        }
        return nivel_actual + 1;
    // No cuenta con los recursos necesarios
    } else {
        return 0;
    }
}

// recibe el jugador atacante y el defensor // -1: error id_2, 0: Ataque fallido, 1: Ataque exitoso
int attack(PlayerInfo* player_info, PlayerInfo** players_info, int id_2){
    if(id_2 > 3 || id_2 < 0){
        return -1;
    }

    PlayerInfo* player_info_2 = players_info[id_2];

    int ataque = player_info->gue * player_info->niv_atq;
    int defensa = player_info_2->gue * player_info_2->niv_def * 2;

    // muere player 2
    if (ataque > defensa) {
        int comida = player_info_2->comida;
        int oro = player_info_2->oro;
        int ciencia = player_info_2->ciencia;

        player_info->comida += comida;
        player_info->oro += oro;
        player_info->ciencia += ciencia;

        player_info_2->comida = 0;
        player_info_2->oro = 0;
        player_info_2->ciencia = 0;

        // status muerto
        player_info_2->status = 0;

        return 1;
    } else {
        int guerreos_restantes = player_info->gue/2;
        player_info->gue = guerreos_restantes;
        return 0;
    }
}

// Descuenta el oro de espiar // -1: error id_2, 0: dinero insuficiente, 1: ok
int spy(PlayerInfo* player_info, int id_2){
    if(id_2 > 3 || id_2 < 0){
        return -1;
    }

    if (player_info->oro < 30){
        return 0;
    }

    player_info->oro -= 30;
    return 1;
}

// Roba a otros jugadores // -2 error input materia, -1: error id_2, 0: dinero insuficiente, 1: ok
int steal(PlayerInfo* player_info, PlayerInfo** players_info, int id_2, int type){
    if(id_2 > 3 || id_2 < 0){
        return -1;
    }

    PlayerInfo* player_info_2 = players_info[id_2];

    if (player_info->ciencia < 10){
        return 0;
    }

    player_info->ciencia -= 10;

    // comida
    if (type == 1){
        int comida_robada = player_info_2->comida * 0.1;

        player_info->comida += comida_robada;
        player_info_2->comida -= comida_robada;
    // oro
    } else if(type == 2) {
        int oro_robado = player_info_2->oro * 0.1;

        player_info->oro += oro_robado;
        player_info_2->oro -= oro_robado;
    } else {
        return -2;
    }

    return 1;
}

// rendirse
void surrender(PlayerInfo* player_info){
    // resetear recursos
    player_info -> oro = 0;
    player_info -> ciencia = 0;
    player_info -> comida = 0;
    player_info -> agr = 0;
    player_info -> gue = 0;
    player_info -> ing = 0;
    player_info -> min = 0;

    // setear muerte
    player_info -> status = 0;
}