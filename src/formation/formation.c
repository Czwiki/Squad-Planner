/**
 * @file formation.c
 * @brief Formation management module for the Squad-Planner application.
 * 
 * This module handles all operations related to creating, managing, and
 * manipulating squad formations. A formation consists of positions on a
 * soccer pitch, with each position able to hold multiple candidate players.
 * 
 * Features:
 * - Create and open formations
 * - Add/remove positions (GK, ST, CB, etc.) to formations
 * - Assign players to positions
 * - Manage player preferences/rankings within positions
 * - Display formations in a tactical view
 * 
 * Functions are ordered by their command ID in parser.c:
 * - ID 1: new (new_formation)
 * - ID 2: newP (new_player)
 * - ID 3: preference (preferences)
 * - ID 4: add (add_position_to_formation)
 * - ID 5: addP (add_player_to_position)
 * - ID 6: remove (remove_position_from_formation)
 * - ID 7: removeP (remove_player_from_position)
 * - ID 8: list (list_players_of_position)
 * - ID 9: listf (list_formations)
 * - ID 10: open (open_formation)
 * - ID 11: show (show)
 *
 * Data Structures:
 * - Formations are stored as a linked list (formation_head)
 * - Each formation has an array of 24 possible positions
 * - Each position can hold multiple players (for rotation/substitution planning)
 * - Players are stored in a separate global linked list (player_head)
 */

#include "formation.h"
#include "../error/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * @brief Array of valid position abbreviations.
 * 
 * Indices correspond to typical soccer positions:
 * 0: GK (Goalkeeper), 1-5: Defenders, 6-10: Defensive Midfielders,
 * 11-15: Midfielders, 16-20: Attacking Midfielders, 21-23: Forwards
 */
static char* positions[] = {
    "GK", "RB", "RCB", "CB", "LCB", "LB",           /* Defense */
    "RWB", "RCDM", "CDM", "LCDM", "LWB",            /* Defensive Mid / Wingbacks */
    "RM", "RCM", "CM", "LCM", "LM",                 /* Midfield */
    "RAM", "RCAM", "CAM", "LCAM", "LAM",            /* Attacking Mid */
    "RS", "ST", "LS"                                 /* Forwards */
};

/** @brief Head of the global player linked list */
static player* player_head = NULL;

/** @brief Head of the global formation linked list */
static formation* formation_head = NULL;

/** @brief Pointer to the currently active formation being edited */
static formation* current_formation = NULL;

/* ========================================================================== */
/* Static Helper Functions                                                    */
/* ========================================================================== */

/**
 * @brief Count the number of elements in a NULL-terminated string array.
 * 
 * @param args NULL-terminated array of strings
 * @return Number of elements (not including the NULL terminator)
 */
static int count_tokens(char** args) {
    int i = 0;
    if (!args) return 0;
    while (args[i]) i++;
    return i; 
}

/**
 * @brief Validate input and optionally display help information.
 * 
 * This utility function performs common validation for command handlers:
 * 1. Checks for --help option and displays usage if present
 * 2. Validates that option/argument counts don't exceed limits
 * 
 * @param options Array of option strings (may be NULL)
 * @param args Array of argument strings (may be NULL)
 * @param expected_args Maximum number of arguments allowed
 * @param expected_options Maximum number of options allowed
 * @param usage Usage string to display for help
 * @param description Description string to display for help
 * 
 * @return 0 if validation passes
 * @return 1 if help was displayed (caller should return success)
 * @return -1 if both args and options are NULL
 * @return -2 if too many options or arguments provided
 */
static int sanity_check_and_help(char** args, char** options, int max_args, int min_args, int max_options, int min_options, char* usage, char* description) {
    int args_count = count_tokens(args);
    int options_count = count_tokens(options);
    if (options_count == 1 && options && options[0] && strcmp(options[0], "--help") == 0) {
        printf("Usage: %s\n%s\n", usage, description);
        if (fflush(stdout) != 0) {
            perror("Error flushing stdout: ");
            return SP_ERR_INTERNAL;
        }
        return 1;  /* Help page printed */
    }
    if (args_count > max_args || options_count > max_options || args_count < min_args || options_count < min_options) {
        return SP_ERR_WRONG_USAGE;  /* Too many options or arguments */
    }
    return 0;  /* Valid input */
}

/**
 * @brief Convert a position abbreviation string to its numeric ID.
 * 
 * @param position_str Position abbreviation (e.g., "GK", "ST", "CB")
 * @return Position ID (0-23) on success, -1 if not found
 */
static int parse_position_string(char* position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1;  /* Position not found */
}

/**
 * @brief Replace underscores with spaces in a player name string.
 * 
 * @param name Player name string to clean
 * @return SP_SUCCESS on success, SP_ERR_MEMORY if name is NULL
 */
static int clean_player_name(char* name) {
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
static void cleanup_players(void) {
    player* current = player_head;
    while (current != NULL) {
        player* next = current->next;
        if (current->name) {
            free(current->name);
        }
        free(current);
        current = next;
    }
    player_head = NULL;
}

/**
 * @brief Free all memory associated with formations.
 * 
 * Iterates through the global formation list and frees all
 * formation structures, their positions, and player lists.
 */
static void cleanup_formations(void) {
    formation* current = formation_head;
    while (current != NULL) {
        formation* next = current->next;
        /* Free all positions in this formation */
        for (int i = 0; i < 24; i++) {
            if (current->map_of_positions[i] != NULL) {
                /* Free the player pointer array (not the players themselves) */
                if (current->map_of_positions[i]->list_of_players != NULL) {
                    free(current->map_of_positions[i]->list_of_players);
                }
                free(current->map_of_positions[i]);
            }
        }
        if (current->name) {
            free(current->name);
        }
        free(current);
        current = next;
    }
    formation_head = NULL;
    current_formation = NULL;
}

/* ========================================================================== */
/* Utility Functions (exported)                                               */
/* ========================================================================== */

/**
 * @brief Get the name string for a position ID.
 * 
 * @param position_id Position ID (0-23)
 * @return Position name string, or NULL if ID is invalid
 */
char* get_position_name(int position_id) {
    if (position_id < 0 || position_id >= 24) {
        return NULL;
    }
    return positions[position_id];
}

/**
 * @brief Find a player by name in the global player list.
 * 
 * @param name Player name to search for
 * @return Pointer to player struct if found, NULL otherwise
 */
player* find_player_by_name(char* name) {
    if (!name) {
        return NULL;
    }
    player* current = player_head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;  /* Player not found */
}

/* ========================================================================== */
/* Command Functions - Ordered by Command ID (parser.c)                       */
/* ========================================================================== */

/**
 * @brief Create a new formation with the given name.
 * 
 * Creates a new empty formation and adds it to the formation list.
 * The new formation becomes the current formation for editing.
 * 
 * @param args Array containing the formation name
 * @param options Array of options (supports --help)
 * 
 * @return 0 on success, SP_ERR_DUPLICATE if name already exists
 */
int new_formation(char** args, char** options) {
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE;
    }
    char *usage = "new <formation_name> [--help]";
    char *description = "Creates a new formation with the specified name. The name must be unique and not already used by an existing formation.";
    int sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;  /* Help displayed */
        return sanity_check;  /* Error */
    }

    /* Check if formation with this name already exists */
    if (open_formation(args, NULL) == 0) {
        return SP_ERR_FORMATION_EXISTS;  /* Formation with this name already exists */
    }

    /* Create the first formation in the list */
    if (formation_head == NULL) {
        formation_head = malloc(sizeof(formation));
        if (!formation_head) {
            return SP_ERR_MEMORY;
        }
        formation_head->name = strdup(args[0]);
        if (!formation_head->name) {
            free(formation_head);
            formation_head = NULL;
            return SP_ERR_MEMORY;
        }
        formation_head->next = NULL;
        current_formation = formation_head;
    } 
    else {
        /* Append to the end of the formation list */
        formation* current = formation_head;
        while (current->next != NULL) {
            if (strcmp(current->name, args[0]) == 0) {
                return SP_ERR_FORMATION_EXISTS;  /* Formation with this name already exists */
            }
            current = current->next;
        }
        formation* new_form = malloc(sizeof(formation));
        if (!new_form) {
            return SP_ERR_MEMORY;
        }
        new_form->name = strdup(args[0]);
        if (!new_form->name) {
            free(new_form);
            return SP_ERR_MEMORY;
        }
        new_form->next = NULL;
        current->next = new_form;
        current_formation = new_form;
    }

    /* Initialize all positions to empty */
    for (int i = 0; i < 24; i++) {
        current_formation->map_of_positions[i] = NULL;
    }  
    return SP_SUCCESS;
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
    if (!args && !options) { // reduce redundancy in code by checking for null pointers at the beginning of the function
        return SP_ERR_WRONG_USAGE;
    }
    char *usage = "newP <name> <age> <overall> <potential> <own> [--help]";
    char *description = "Creates a new player with the specified attributes and adds them to the global player list. If you wish to add a player with names consisting of multiople words, use underscores instead (e.g., 'Ter_Stegen', 'Christiano_Ronaldo'). All other non-alphabetical characters are treated as valid input. Age must be greater than 0, and ratings must be between 0 and 100.";
    int sanity_check = sanity_check_and_help(args, options, 5, 1, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
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
    errno = 0;
    int age = strtol(args[1], NULL, 10);
    int overall = strtol(args[2], NULL, 10);
    int potential = strtol(args[3], NULL, 10);
    int own = strtol(args[4], NULL, 10);

    if (errno == ERANGE) {
        free(new_p->name);
        free(new_p);
        return SP_ERR_MEMORY;  /* Conversion error */
    }
    
    /* Validate attribute ranges */
    if (age <= 0 || overall < 0 || potential < 0 || own < 0 || 
        overall > 100 || potential > 100 || own > 100) {
        free(new_p->name);
        free(new_p);
        return SP_ERR_INVALID_RANGE;  /* Invalid values */
    }
    
    new_p->age = age;
    new_p->overall_rating = overall;
    new_p->potential_rating = potential;
    new_p->own_rating = own;

    /* Add to end of player list */
    player* current = player_head;
    if (!current) {
        player_head = new_p;
    } else {
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_p;
    }
    new_p->next = NULL;
    return SP_SUCCESS;
}

/**
 * @brief Reorder players in a position based on preference order.
 * 
 * Reorders the player list for a position based on the specified
 * preference order. Optionally reverses the order with -reverse flag.
 * 
 * @param options Array of options (supports -reverse to reverse order)
 * @param args Array: args[0] = position, args[1..n] = player names in order
 */
int preferences(char** args, char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;  /* No formation currently open */
    }
    // because the size of the player list is a required argument for the sanity check, these other ones must be concluded beforehand
    if (!args && !options) { 
        return SP_ERR_WRONG_USAGE;
    }
    if (!args || !args[0]) {
        return SP_ERR_WRONG_USAGE;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned */
    }
    position* pos = current_formation->map_of_positions[position_id];
    char *usage = "preference <position> [player1 player2 ...] [-reverse] [--help]";
    char *description = "Reorder players in a position based on specified preference order. Use -reverse to invert the order.";
    int sanity = sanity_check_and_help(args, options, pos->size_of_list+1, 2, 1, 0, usage, description);
    if (sanity != 0) {
        if (sanity == 1) return SP_SUCCESS;
        return sanity;
    }
    /* Check for -reverse option */
    int backwards = 0;
    if (options && options[0]) {
        if (strcmp(options[0], "-reverse") == 0) {
            backwards = 1;
        }
    }
    if (pos->size_of_list == 0) {
        return SP_ERR_INVALID_CMD;  /* No players to reorder */
    }
    
    if (pos->size_of_list == 1) {
        return SP_SUCCESS;  /* No reordering needed */
    }

    /* Build preference order array from args */
    int *preference = malloc(sizeof(int) * pos->size_of_list);
    if (!preference) {
        return SP_ERR_MEMORY;
    }

    int i = 1;
    int p_i = 0;
    int found = 0;
    while (args[i] != NULL) {
        found = 0;
        for (int j = 0; j < pos->size_of_list; j++) {
            if (strcmp(pos->list_of_players[j]->name, args[i]) == 0) {
                preference[p_i] = j;
                p_i++;
                found = 1;
                break;
            }
        }
        if (!found) {
            free(preference);
            return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found in position's list */
        }
        i++;
    }
    
    /* Create new list with preferred order */
    player** new_list = malloc(sizeof(player*) * pos->size_of_list);
    if (!new_list) {
        free(preference);
        return SP_ERR_MEMORY;
    }
    for (int k = 0; k < pos->size_of_list; k++) {
        if (backwards) {
            new_list[k] = pos->list_of_players[preference[pos->size_of_list - 1 - k]];
        } else {
            new_list[k] = pos->list_of_players[preference[k]];
        }
    }
    free(pos->list_of_players);
    pos->list_of_players = new_list;
    free(preference);
    return SP_SUCCESS;
}

/**
 * @brief Add positions to the current formation.
 * 
 * Adds one or more positions (up to 11) to the currently active formation.
 * Each position is identified by its abbreviation (e.g., "GK", "ST").
 * A formation can have at most 11 positions assigned.
 * 
 * @param args Array of position abbreviations to add
 * @param options Array of options (supports --help)
 * 
 */
int add_position_to_formation(char** args, char** options) {
    if (!args && !options) { // reduce redundancy in code by checking for null pointers at the beginning of the function
        return SP_ERR_WRONG_USAGE;
    }
    char *usage = "add <position_names>[1-11] [--help]";
    char *description = "Adds the specified positions to the current formation. The formation must already exist.";
    int sanity_check = sanity_check_and_help(args, options, 11, 1, 1, 0, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }

    if (!formation_head || !current_formation) {
        return SP_ERR_NO_FORMATION;  /* No formations available  or none selected*/
    }
    
    /* Add each position specified in args */
    int k = 0;
    while (args[k] != NULL) {
        int position_id = parse_position_string(args[k]);
        if (position_id == -1) {
            return SP_ERR_INVALID_POSITION;  /* Position not found */
        }
        
        /* Count currently assigned positions */
        int assigned_count = 0;
        for (int i = 0; i < 24; i++) {
            if (current_formation->map_of_positions[i] != NULL) {
                assigned_count++;
            }
        }
        
        if (assigned_count >= 11) {
            return SP_ERR_TOO_MANY_POSITIONS;  /* Formation already has 11 positions assigned */
        }
        
        /* Create and initialize the new position */
        position* temp = malloc(sizeof(position));
        if (!temp) {
            return SP_ERR_MEMORY;
        }
        temp->list_of_players = NULL;
        temp->size_of_list = 0;
        temp->id = position_id;
        temp->name = get_position_name(position_id);
        current_formation->map_of_positions[position_id] = temp;
        k++;
    }
    return SP_SUCCESS;
}

/**
 * @brief Add a player to a specific position in the current formation.
 * 
 * Assigns an existing player to a position's candidate list.
 * The position must exist in the formation and the player must exist
 * in the global player list.
 * 
 * @param args Array: args[0] = position name, args[1] = player name
 */
int add_player_to_position(char** args, char **options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;
    }
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE;
    }

    char *usage = "addP <position_name> <player_names>[1-10] [--help]";
    char *description = "Adds the specified players to the candidate list for the specified position in the current formation. The position must already be assigned in the formation, and the player must exist in the global player list.";
    int sanity_check = sanity_check_and_help(args, options, 11, 2, 1, 0, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }

    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned in formation */
    }
    int i = 1;
    while (args[i] != NULL) {
        if (i > 10) {
            return SP_ERR_WRONG_USAGE;  /* Too many players specified */
        }
        player* p = find_player_by_name(args[i]);
        if (!p) {
            return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found */
        }
        position* pos = current_formation->map_of_positions[position_id];
        
        /* Check for duplicate - player already in position */
        for (int j = 0; j < pos->size_of_list; j++) {
            if (strcmp(pos->list_of_players[j]->name, p->name) == 0) {
                return SP_ERR_PLAYER_ALREADY_ASSIGNED;  /* Player already in the position's list */
            }
        }
        
        /* Append the player to the position's list */
        player** new_list = realloc(pos->list_of_players, sizeof(player*) * (pos->size_of_list + 1));
        if (!new_list) {
            return SP_ERR_MEMORY;
        }
        new_list[pos->size_of_list] = p;
        pos->list_of_players = new_list;
        pos->size_of_list = pos->size_of_list + 1;
        i++;
    }
    return SP_SUCCESS;
}

/**
 * @brief Remove a position from the current formation.
 * 
 * Removes a position from the formation. The position must not have
 * any players assigned to it (remove players first).
 * 
 * @param args Array: args[0] = position abbreviation
 */
int remove_position_from_formation(char** args, char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;  /* No formation currently open */
    }
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE; // one must be present
    }

    char *usage = "remove <position_name>[1-5] [--help]";
    char *description = "Removes the specified position from the current formation. Position must not have players assigned.";
    int sanity_check = sanity_check_and_help(args, options, 5, 1, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return 0;
        return sanity_check;
    }
    int i = 0;
    while (args[i] != NULL) {
        if (i > 5) {
            return SP_ERR_WRONG_USAGE;  /* Too many positions specified */
        }
        int position_id = parse_position_string(args[i]);
        if (position_id == -1) {
            return SP_ERR_INVALID_POSITION;  /* Position not found */
        }
        if (current_formation->map_of_positions[position_id] == NULL) {
            return SP_SUCCESS;  /* Position is already empty */
        }
        if (current_formation->map_of_positions[position_id]->list_of_players != NULL) {
            return SP_ERR_INVALID_CMD;  /* Cannot remove position with assigned players */
        }
        free(current_formation->map_of_positions[position_id]->list_of_players);
        free(current_formation->map_of_positions[position_id]);
        current_formation->map_of_positions[position_id] = NULL;
        i++;
    }
    return SP_SUCCESS;
}

/**
 * @brief Remove a player from a position's candidate list.
 * 
 * Removes the specified player from the position's player list.
 * The player is not deleted from the global player list.
 * 
 * @param args Array: args[0] = position name, args[1] = player name
 */
int remove_player_from_position(char** args, char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;
    }
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE;
    }

    char *usage = "removeP <position_name> <player_names>[1-10] [-all --help]";
    char *description = "Removes the specified players from the candidate list of the specified position. With -all option, all players from the position are removed. The position must exist in the current formation.";
    int sanity_check = sanity_check_and_help(args, options, 11, 0, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }

    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned in formation */
    }
    
    position* pos = current_formation->map_of_positions[position_id];
    // options[0] cannot be --help due to previous sanity check
    if (options && options[0]) {
        if (strcmp(options[0], "-all") == 0) {
            /* Remove all players from the position */
            if (args && args[1] != NULL) {
                return SP_ERR_WRONG_USAGE;  /* No player names should be specified with -all */
            }
            free(pos->list_of_players);
            pos->list_of_players = NULL;
            pos->size_of_list = 0;
            return SP_SUCCESS;
        }
    }
    int i = 1;
    while (args[i] != NULL) {
         if (i > 10) {
            return SP_ERR_WRONG_USAGE;  /* Too many players specified */
        }
        int found_index = -1;
        
        /* Find the player in the position's list */
        for (int j = 0; j < pos->size_of_list; j++) {
            if (strcmp(pos->list_of_players[j]->name, args[i]) == 0) {
                found_index = j;
                break;
            }
        }
        if (found_index == -1) {
            return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found in position's list */
        }
        
        /* Shift players to remove the player at found_index */
        for (int j = found_index; j < pos->size_of_list - 1; j++) {
            pos->list_of_players[j] = pos->list_of_players[j + 1];
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
        i++;  /* Move to next player argument */
    }
    return SP_SUCCESS;
}

/**
 * @brief List all players assigned to a specific position.
 * 
 * Displays the names of all players in a position's candidate list.
 * 
 * @param args Array: args[0] = position abbreviation
 */
int list_players_of_position(char** args, char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;
    }
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE;
    }

    char *usage = "list <position_name> [--help]";
    char *description = "List all players assigned to the specified position in the current formation.";
    int sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned in formation */
    }

    position* pos = current_formation->map_of_positions[position_id];
    printf("Players for position %s:\n", get_position_name(position_id));

    for (int i = 0; i < pos->size_of_list; i++) {
        char *cleaned_name = strdup(pos->list_of_players[i]->name);
        if (!cleaned_name) {
            return SP_ERR_MEMORY;
        }
        if (clean_player_name(cleaned_name) != SP_SUCCESS) {
            free(cleaned_name);
            return SP_ERR_MEMORY;
        }
        printf("- %s\n", cleaned_name);
        free(cleaned_name);
    }

    return SP_SUCCESS;
}

/**
 * @brief List all available formations.
 * 
 * Displays the names of all formations in the formation list.
 */
int list_formations(char** options) {
    /* listing formations does not require a currently opened formation */
    
    char *usage = "listf [--help]";
    char *description = "Displays the names of all formations in the formation list.";
    int sanity_check = sanity_check_and_help(NULL, options, 0, 0, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }
    formation* current = formation_head;
    if (!current) {
        printf("No formations available.\n");
        return SP_SUCCESS;
    }
    printf("Available formations:\n");
    while (current != NULL) {
        printf("- %s\n", current->name);
        current = current->next;
    }
    return SP_SUCCESS;
}

/**
 * @brief Open an existing formation for editing.
 * 
 * Sets the specified formation as the current formation, allowing
 * subsequent commands to modify it.
 * 
 * @param args Array containing the formation name to open
 * @param options Array of options (supports --help)
 * 
 * @return 0 on success, -2 if formation not found or no formations exist
 */
int open_formation(char** args, char** options) {
    if (!args && !options) {
        return SP_ERR_WRONG_USAGE;  /* No arguments or options provided */
    }
    if (!formation_head) {
        return SP_ERR_NO_FORMATION;  /* No formations available */
    }    
    char *usage = "open <formation_name> [--help]";
    char *description = "Opens the specified formation for editing. The formation must already exist.";
    int sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return SP_SUCCESS;
        return sanity_check;
    }

    formation* previous = formation_head;
    formation* current = previous;
    while (current != NULL) {
        if (strcmp(current->name, args[0]) == 0) {
            current_formation = current;  /* Found - set as current */
            return SP_SUCCESS;
        }
        previous = current;
        current = current->next;
    }
    return SP_ERR_NO_FORMATION;  /* Formation not found */
}

/**
 * @brief Display the current formation in a tactical view.
 * 
 * Renders the formation as a text-based tactical view showing
 * all assigned positions in their approximate field locations.
 * 
 * The display is a 6x5 grid representing a soccer pitch from
 * goal to goal, with positions placed according to their roles.
 */
int show(char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;  /* No formation currently open */
    }

    char *usage = "show [--help]";
    char *description = "Display the current formation in a tactical view.";
    int sanity_check = sanity_check_and_help(NULL, options, 0, 0, 1, 0, usage, description);
    if (sanity_check != 0) {
        if (sanity_check == 1) return 0;
        return sanity_check;
    }

    /* Fixed 6 rows x 5 columns grid to ensure column alignment.
       Rows: 0 (forwards) ... 5 (goalkeeper). Columns 0..4.
       Forwards occupy middle 3 columns; GK sits behind central CB.
    */
    char empty_str[] = " ";
    char* grid[6][5];
    /* Helper to get position name or empty */
    #define POS_NAME(idx) (current_formation->map_of_positions[idx] ? current_formation->map_of_positions[idx]->name : empty_str)

    /* Row 0: Forwards (RS, ST, LS) centered */
    grid[0][0] = empty_str;
    grid[0][1] = POS_NAME(21); /* RS */
    grid[0][2] = POS_NAME(22); /* ST */
    grid[0][3] = POS_NAME(23); /* LS */
    grid[0][4] = empty_str;

    /* Fill all cells with empty by default */
    for (int r = 0; r < 6; r++) {
        for (int c = 0; c < 5; c++) {
            grid[r][c] = empty_str;
        }
    }

    /* Row 0: Forwards (RS, ST, LS) centered in columns 1..3 */
    grid[0][1] = POS_NAME(21);
    grid[0][2] = POS_NAME(22);
    grid[0][3] = POS_NAME(23);

    /* Rows 1..4: contiguous 5-position blocks starting at 16,11,6,1 */
    for (int r = 1; r <= 4; r++) {
        int start_index = 16 - (r - 1) * 5; /* r=1->16, r=2->11, r=3->6, r=4->1 */
        for (int c = 0; c < 5; c++) {
            grid[r][c] = POS_NAME(start_index + c);
        }
    }

    /* Row 5: Goalkeeper centered behind central defender (CB at col 2) */
    grid[5][2] = POS_NAME(0); /* GK */

    /* Print rows top (forwards) to bottom (goalkeeper) with fixed column width */
    for (int r = 0; r < 6; r++) {
        for (int c = 4; c >= 0; c--) {
            printf("%-6s", grid[r][c]);
        }
        printf("\n");
    }
    #undef POS_NAME
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}

/* ========================================================================== */
/* Cleanup Function (exported)                                                */
/* ========================================================================== */

/**
 * @brief Free all application resources.
 * 
 * Calls cleanup functions for all global data structures.
 * Should be called before program exit.
 */
void cleanup_all(void) {
    cleanup_formations();
    cleanup_players();
}
