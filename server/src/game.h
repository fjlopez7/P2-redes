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

  int aldeanos_initial;

  int comida;
  int oro;
  int ciencia;
  int socket;
} PlayerInfo;


PlayerInfo** init_all_player_info();