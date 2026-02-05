#include "position.h"

typedef struct formation {
    char* name;
    position* map_of_positions[24];
    struct formation* next;
} formation;

int open_formation(char** args, char** options);
int new_formation(char** args, char** options);
int add_position_to_formation(char** args, char** options);
int show(void);
int new_player(char ** args);
int add_player_to_position(char ** args);
int list_players_of_position(char **args);
int remove_player_from_position(char **args);
int remove_position_from_formation(char **args);
int preferences(char ** options, char **args);
int list_formations(void);
