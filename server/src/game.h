typedef struct player_info{
  char name[30];
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
  int niv_atq;
  int niv_def;
  int status;

  int aldeanos_initial;

  int comida;
  int oro;
  int ciencia;
  int socket;
  int uid;

  int first;

    
} PlayerInfo;


void init_all_player_info(PlayerInfo** players_info);
void free_init_all(PlayerInfo** players_info);
void add_resources(PlayerInfo* player_info);
int create_villager(PlayerInfo* player_info, int type);
int level_up(PlayerInfo* player_info, int type);
int attack(PlayerInfo* player_info, PlayerInfo** players_info, int id_2);
int spy(PlayerInfo* player_info, int id_2);
int steal(PlayerInfo* player_info, PlayerInfo** players_info, int id_2, int type);
void surrender(PlayerInfo* player_info);