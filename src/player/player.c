#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../squad/squad.h"
#include "../formation/formation.h"
#include "player.h"
#include "../error/error.h"

/**
* @brief Valid commands in the player menu context.
 * 
 * Index 0: help   - Display player menu help
 * Index 1: newP   - Create a new player
 * Index 2: openP  - Open an existing player for editing (not yet implemented)
 * Index 3: list   - List all players
 * Index 4: deleteP- Remove an existing player
 * Index 5: save   - Save current player data (transitions to saves menu)
 * Index 6: back   - Return to squad menu
 */

static Squad* current_squad = NULL;  /**< Pointer to the currently active squad being edited */

static player* current_player = NULL;  /**< Pointer to the currently active player being edited */

int setting_squad_player(Squad* squad) {
    current_squad = squad;
    return SP_SUCCESS;
}

static player* find_player_by_name(const char* name) {
    if (!name) {
        return NULL;
    }
    player* current = current_squad ? current_squad->players : NULL;  /* Search within current squad's player list */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;  /* Player not found */
}

/**
 * @brief Replace underscores with spaces in a player name string.
 * 
 * @param name Player name string to clean
 * @return SP_SUCCESS on success, SP_ERR_MEMORY if name is NULL
 */
static int clean_name_string(char* name) {
    if (!name) {
        return SP_ERR_MEMORY;
    }
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == '_') {
            name[i] = ' ';
        }
    }
    return SP_SUCCESS;
}

/**
 * @brief Free all memory associated with players.
 * 
 * Iterates through the global player list and frees all
 * player structures and their name strings.
 */
void cleanup_player(player* current) {
    if (current->name) {
        free(current->name);
    }
    free(current);
}

/**
 * @brief Create a new player and add to the global player list.
 * 
 * Creates a player with the specified attributes and adds them
 * to the global player linked list.
 * 
 * @param args Array: [0]=name, [1]=age, [2]=overall, [3]=potential, [4]=own_rating
 *             All ratings should be 0-100, age > 0
 */
int new_player(char** args, char** options) {
    if (find_player_by_name(args[0]) != NULL) {
        return SP_ERR_PLAYER_EXISTS;  /* Player with this name already exists */
    }
    printf("Adding player: %s\n", args[0]);
    player* new_p = malloc(sizeof(player));
    if (!new_p) {
        return SP_ERR_MEMORY;
    }
    new_p->name = strdup(args[0]);
    if (!new_p->name) {
        free(new_p);
        return SP_ERR_MEMORY;
    }
    
    /* Parse player attributes from string arguments */
    char *endptr;
    int age = strtol(args[1], &endptr, 10);
    int overall = strtol(args[2], &endptr, 10);
    int potential = strtol(args[3], &endptr, 10);
    int own = strtol(args[4], &endptr, 10);

    if (endptr == args[1] || endptr == args[2] || endptr == args[3] || endptr == args[4]) {
        // invalid integer conversion for one of the arguments
        free(new_p->name);
        free(new_p);
        return SP_ERR_INTERNAL;  /* Conversion error */
    }
    
    /* Validate attribute ranges */
    if (age <= 0 || overall < 0 || potential < 0 || own < 0 || overall > 100 || potential > 100 || own > 100) {
        free(new_p->name);
        free(new_p);
        return SP_ERR_INVALID_RANGE;  /* Invalid values */
    }
    
    new_p->age = age;
    new_p->overall_rating = overall;
    new_p->potential_rating = potential;
    new_p->own_rating = own;

    /* Add to end of player list */
    player* current = current_squad ? current_squad->players : NULL;  /* Add to current squad's player list */
    if (!current) {
        current_squad->players = new_p;
    } else {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_p;
    }
    new_p->next = NULL;
    return SP_SUCCESS;
}

int open_player(char** args, char** options) {
    player* p = find_player_by_name(args[0]);
    if (!p) {
        return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found */
    }
    current_player = p;
    return SP_SUCCESS;
}

int list_players(void) {
    if (!current_squad || !current_squad->players) {
        printf("No players in current squad.\n");
        return SP_SUCCESS;
    }
    char *clean_name = strdup(current_squad->name);
    if (!clean_name) {
        return SP_ERR_MEMORY;
    }
    if (clean_name_string(clean_name) != SP_SUCCESS) {
        free(clean_name);
        return SP_ERR_MEMORY;
    }
    printf("Players in squad %s:\n", clean_name);
    free(clean_name);
    player* temp = current_squad->players;
    while (temp) {
        char *cleaned_name = strdup(temp->name);
        if (!cleaned_name) {
            return SP_ERR_MEMORY;
        }
        if (clean_name_string(cleaned_name) != SP_SUCCESS) {
            free(cleaned_name);
            return SP_ERR_MEMORY;
        }
        printf("- %s\n", cleaned_name);
        free(cleaned_name);
        temp = temp->next;
    }
    return SP_SUCCESS;
}

int delete_player(char** args, char** options) {
    if (current_player && strcmp(current_player->name, args[0]) == 0) {
        current_player = NULL;  /* If the player being deleted is currently open, reset current_player */
    }
    player* current = current_squad ? current_squad->players : NULL;  /* Start from the head of the player list */
    player* previous = NULL;
    while (current != NULL) {
        if (strcmp(current->name, args[0]) == 0) {
            /* Found the player to remove */
            if (previous == NULL) {
                /* head is still current, therefore next will be the new head */
                current_squad->players = current->next;
            } 
            else {
                previous->next = current->next;
            }
            /* Remove player from all positions in all formations */
            formation* f_current = current_squad->formations;
            while (f_current != NULL) {
                for (int i = 0; i < 24; i++) {
                    if (f_current->map_of_positions[i] != NULL) {
                        position* pos = f_current->map_of_positions[i];
                        for (int j = 0; j < pos->size_of_list; j++) {
                            if (strcmp(pos->list_of_players[j]->name, current->name) == 0) {
                                /* Shift players to remove the player at index j */
                                for (int k = j; k < pos->size_of_list - 1; k++) {
                                    pos->list_of_players[k] = pos->list_of_players[k + 1];
                                }
                                pos->size_of_list--;
                                if (pos->size_of_list == 0) {
                                    free(pos->list_of_players);
                                    pos->list_of_players = NULL;
                                } else {
                                    player** new_list = realloc(pos->list_of_players, sizeof(player*) * pos->size_of_list);
                                    if (new_list) {
                                        pos->list_of_players = new_list;
                                    }
                                }
                                break;  /* Player can only be in a position once, so break after finding */
                            }
                        }
                    }
                }
                f_current = f_current->next;
            }
            /* Free the removed player */
            cleanup_player(current);
            break;
        }
        // iterative traverse        
        previous = current;
        current = current->next;
    }
    return SP_SUCCESS;
}

int edit_player(char** args, char** options) {
    player* p = find_player_by_name(args[0]);
    if (!p) {
        return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found */
    }
    char* attribute = args[1];
    char* new_value_str = args[2];
    
    char *endptr;
    int new_value = strtol(new_value_str, &endptr, 10);
    if (endptr == new_value_str) {
        return SP_ERR_INTERNAL;  /* Conversion error */
    }

    if (strcmp(attribute, "age") == 0) {
        if (new_value <= 0) {
            return SP_ERR_INVALID_RANGE;  /* Age must be greater than 0 */
        }
        p->age = new_value;
    } else if (strcmp(attribute, "overall") == 0) {
        if (new_value < 0 || new_value > 100) {
            return SP_ERR_INVALID_RANGE;  /* Ratings must be between 0 and 100 */
        }
        p->overall_rating = new_value;
    } else if (strcmp(attribute, "potential") == 0) {
        if (new_value < 0 || new_value > 100) {
            return SP_ERR_INVALID_RANGE;  /* Ratings must be between 0 and 100 */
        }
        p->potential_rating = new_value;
    } else if (strcmp(attribute, "own") == 0) {
        if (new_value < 0 || new_value > 100) {
            return SP_ERR_INVALID_RANGE;  /* Ratings must be between 0 and 100 */
        }
        p->own_rating = new_value;
    } else {
        return SP_ERR_INVALID_CMD;  /* Invalid attribute */
    }
    return SP_SUCCESS;
}

//int list_players();