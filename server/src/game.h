typedef struct player_info{
  char * name;
  int player;
  // cantidades de aldeanos de cada clase
  int agr;
  int min; 
  int ing;
  int gue;
  // nidel de los aldeanos de cada clase
  int niv_agr;
  int niv_min; 
  int niv_ing;
  int niv_gue;  

  int comida;
  int oro;
  int ciencia;
  int estado; //onda decir si está habilitado para jugar o no (1 si, 0 no)
} PlayerInfo;

PlayerInfo *init_player_info(int jugador_actual); 