#include "position.h"

typedef struct formation
{
    char* name;
    int map_of_positions[24];
    position positions[11];
    struct formation* next;
} formation;

int open_formation(char* name);
int new_formation(char* name);
int add_position_to_formation(char *position_name);
int present_formation();