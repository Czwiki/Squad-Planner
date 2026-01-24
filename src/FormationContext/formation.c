#include "formation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static char * positions[] = {"GK", "RB", "RCB", "CB", "LCB", "LB", "RWB", "RCDM", "CDM", "LCDM", "LWB", "RM", "RCM", "CM", "LCM", "LM", "RAM", "RCAM", "CAM", "LCAM", "LAM", "RS", "ST", "LS"};
static player* players[100] = {NULL}; // Dummy initialization


static formation*formation_head = NULL; 
static formation*current_formation = NULL;

int parse_position_string(char *position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1; // Position not found
}
char* get_position_name(int position_id) {
    if (position_id < 0 || position_id >= 24) {
        return NULL;
    }
    return positions[position_id];
}

player* find_player_by_name(char* name) {
    if (!name) {
        return NULL;
    }
    int i = 0;
    while (players[i] != NULL) {
        if (strcmp(players[i]->name, name) == 0) {
            return players[i];
        }
        i++;
    }
    return NULL; // Player not found
}

int open_formation(char* name) {
    if (!name) {
        return -1;
    }
    formation* previous = formation_head;
    formation* current = previous;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Found the formation
            current_formation = current;
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return -2; // Formation not found
}

int new_formation(char* name) {
    if (!name) {
        return -1;
    }
    if (open_formation(name) == 0) {
        return -2; // Formation with this name already exists
    }

    if (formation_head == NULL) {
        formation_head = malloc(sizeof(formation));
        if (!formation_head) {
            return -1;
        }
        formation_head->name = strdup(name);
        if (!formation_head->name) {
            free(formation_head);
            formation_head = NULL;
            return -1;
        }
        formation_head->next = NULL;
        current_formation = formation_head;
    } 
    else {
        formation* current = formation_head;
        while (current->next != NULL) {
            current = current->next;
        }
        formation* new_form = malloc(sizeof(formation));
        if (!new_form) {
            return -1;
        }
        new_form->name = strdup(name);
        if (!new_form->name) {
            free(new_form);
            return -1;
        }
        new_form->next = NULL;
        current->next = new_form;
        current_formation = new_form;
    }

    for (int i = 0; i < 24; i++) {
        current_formation->map_of_positions[i] = NULL; // -1 indicates no player assigned to that position
    }  
    return 0;
}

int add_position_to_formation(char **position_names) {
    if (!current_formation || !position_names) {
        return -1;
    }
    // Find the position ID based on the name
    int k = 0;
    while (position_names[k] != NULL) {
        int position_id = parse_position_string(position_names[k]);
        if (position_id == -1) {
            return -2; // Position not found
        }
        int assigned_count = 0;
        for (int i = 0; i < 24; i++) {
            if (current_formation->map_of_positions[i] != NULL) {
                assigned_count++;
            }
        }
        if (position_id == -1) {
            return -2; // Position not found
        }
        if (assigned_count >= 11) {
            return -2; // Formation already has 11 positions assigned
        }
        position* temp = malloc(sizeof(position));
        if (!temp) {
            return -1; // Memory allocation error
        }
        temp->list_of_players = NULL;
        temp->size_of_list = 0;
        temp->id = position_id;
        temp->name = get_position_name(position_id);
        current_formation->map_of_positions[position_id] = temp;
        k++;
    }
    return 0;
}

int add_player_to_position(char **args) {
    if (!current_formation || !args || !args[0] || !args[1]) {
        return -1;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2; // Position not found
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2; // Position is empty
    }
    player *p = find_player_by_name(args[1]);
    if (!p) {
        return -2; // Player not found
    }
    position *pos = current_formation->map_of_positions[position_id];
    // Check for duplicate
    for (int i = 0; i < pos->size_of_list; i++) {
        if (strcmp(pos->list_of_players[i]->name, p->name) == 0) {
            return -2; // Player already in the position's list
        }
    }
    // Append the player (realloc works with NULL)
    player **new_list = realloc(pos->list_of_players, sizeof(player*) * (pos->size_of_list + 1));
    if (!new_list) {
        return -1; // Memory allocation error
    }
    new_list[pos->size_of_list] = p;
    pos->list_of_players = new_list;
    pos->size_of_list = pos->size_of_list + 1;
    return 0;
}

int new_player(char ** args) {
    if (!args || !args[0]) {
        return -1;
    }
    player *new_p = malloc(sizeof(player));
    if (!new_p) {
        return -1;
    }
    new_p->name = strdup(args[0]);
    if (!new_p->name) {
        free(new_p);
        return -1;
    }
    int age = strtol(args[1], NULL, 10);
    int overall = strtol(args[2], NULL, 10);
    int potential = strtol(args[3], NULL, 10);
    int own = strtol(args[4], NULL, 10);
    if (errno == ERANGE) {
        free(new_p->name);
        free(new_p);
        return -1; // Conversion error
    }
    if (age <= 0 || overall < 0 || potential < 0 || own < 0) {
        free(new_p->name);
        free(new_p);
        return -2; // Invalid values
    }
    new_p->age = age;
    new_p->overall_rating = overall;
    new_p->potential_rating = potential;
    new_p->own_rating = own;

    int i = 0;
    while (players[i] != NULL) i++;
    players[i] = new_p;
    return 0;
}

int list_players_of_position(char **args) {
    if (!current_formation || !args || !args[0]) {
        return -1;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2; // Position not found
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2; // Position is empty
    }

    position *pos = current_formation->map_of_positions[position_id];
    printf("Players for position %s:\n", get_position_name(position_id));

    for (int i = 0; i < pos->size_of_list; i++) {
        printf("- %s\n", pos->list_of_players[i]->name);
    }

    return 0;
}

/*int free_list_of_players(player **list_of_players) {
    if (!list_of_players) {
        return 0;
    }
    int i = 0;
    while (list_of_players[i] != NULL) {
        free(list_of_players[i]->name);
        free(list_of_players[i]);
        i++;
    }
    free(list_of_players);
    return 0;
}

int remove_position_from_formation(char *position_name) {
    if (!current_formation || !position_name) {
        return -1;
    }
    int position_id = parse_position_string(position_name);
    if (position_id == -1) {
        return -2; // Position not found
    }
    if (current_formation->map_of_positions[position_id] == 0) {
        return -3; // Position is already empty
    }
    for (int i = 0; i < 11; i++) {
        if (strcmp(current_formation->positions[i].name, position_name) == 0) {
        }
    }
    current_formation->map_of_positions[position_id] = 0; // 0 indicates position is empty
    return 0;
}*/

/*int add_player_to_position(char *position_name, player *p) {
    if (!current_formation || !position_name || !p) {
        return -1;
    }
    int position_id = parse_position_string(position_name);
    if (position_id == -1) {
        return -2; // Position not found
    }
    if (current_formation->map_of_positions[position_id] == 0) {
        return -3; // Position is empty
    }
    position *pos = &current_formation->positions[position_id];
    // Add player to the position's list of players
    int count = 0;
    if (pos->list_of_players) {
        while (pos->list_of_players[count] != NULL) {
            count++;
        }
    }
    player **new_list = realloc(pos->list_of_players, sizeof(player*) * (count + 2));
    if (!new_list) {
        return -1; // Memory allocation error
    }
    new_list[count] = p;
    new_list[count + 1] = NULL;
    pos->list_of_players = new_list;
    return 0;
}*/

int present_formation() {
    if (!current_formation) {
        return -1;
    }
    char *empty = " ";
    int curr_offset = 0;
    int first0 = 0;
    int first1 = 0;
    char *to_print[30] = {0};
    to_print[0] = empty;
    to_print[1] = empty;
    to_print[3] = empty;
    to_print[4] = empty;
    to_print[25] = empty;
    to_print[29] = empty;

    for (int i = 0; i < 24; i++) {
        if (i >= 0 && !first0){
            curr_offset +=2;
        }
        if (i >= 21 && !first1){
            curr_offset +=1;
            first1 = 1;
        }
        if (current_formation->map_of_positions[i] != NULL) {
            to_print[i + curr_offset] = current_formation->map_of_positions[i]->name;
        }
        else {
            to_print[i + curr_offset] = empty;
        }

        if (!first0){
            curr_offset +=2;
            first0 = 1;
        }
    }
    for (int i = 30; i > 0; i--) {
        if (i % 5 == 0 && i != 0){
            printf("\n");
        }
        printf("%-6s", to_print[i-1]);
    } 
    printf("\n");
    return 0;
}