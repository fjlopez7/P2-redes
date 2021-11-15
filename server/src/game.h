typedef struct player_info{
  char name[256];
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
  int uid;
} PlayerInfo;


void init_all_player_info(PlayerInfo** players_info);
void free_init_all(PlayerInfo** players_info);