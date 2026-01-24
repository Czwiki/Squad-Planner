#include "position.h"

typedef struct formation
{
    char* name;
    position* map_of_positions[24];
    struct formation* next;
} formation;

int open_formation(char* name);
int new_formation(char* name);
int add_position_to_formation(char **position_names);
int present_formation();
int new_player(char ** args);
int add_player_to_position(char ** args);
int list_players_of_position(char **args);
