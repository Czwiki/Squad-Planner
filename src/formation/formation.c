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
 * - ID 12: deletef (remove_formation)
 * - ID 13: deleteP (remove_player)
 * - ID 14: save (transitions to saves menu)
 * - ID 15: back (returns to main menu)
 *
 * Data Structures:
 * - Formations are stored as a linked list (formation_head)
 * - Each formation has an array of 24 possible positions
 * - Each position can hold multiple players (for rotation/substitution planning)
 * - Players are stored in a separate global linked list (player_head)
 */

#include "formation.h"
#include "position.h"
#include "../error/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// help declaration, needed before it is defined
int open_formation(char** args, char** options);

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
 * @brief Convert a position abbreviation string to its numeric ID.
 * 
 * @param position_str Position abbreviation (e.g., "GK", "ST", "CB")
 * @return Position ID (0-23) on success, -1 if not found
 */
static int parse_position_string(const char* position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1;  /* Position not found */
}

int get_pos_size_of_list(char* arg) {
    int position_id = parse_position_string(arg);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned */
    }
    return current_formation->map_of_positions[position_id]->size_of_list;
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

// helper function for correct prompt in main.c
int get_current_formation_name(char *dest) {
    if (!current_formation) {
        dest[0] = '\0';  // set dest to empty string
        return SP_ERR_NO_FORMATION;  // No formation currently open, return empty name
    }
    char cleaned_name[40];
    strncpy(cleaned_name, current_formation->name, 40);  // copy up to 39 characters + null terminator
    clean_name_string(cleaned_name);
    strncpy(dest, cleaned_name, 40);  // copy up to 39 characters + null terminator
    dest[39] = '\0';  // ensure null termination
    return SP_SUCCESS;
}

/**
 * @brief Free all memory associated with players.
 * 
 * Iterates through the global player list and frees all
 * player structures and their name strings.
 */
static void cleanup_player(player* current) {
    if (current->name) {
        free(current->name);
    }
    free(current);
}

/**
 * @brief Free all memory associated with formations.
 * 
 * Iterates through the global formation list and frees all
 * formation structures, their positions, and player lists.
 */
static void cleanup_formation(formation* current) {
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
}

/**
 * @brief Get the name string for a position ID.
 * 
 * @param position_id Position ID (0-23)
 * @return Position name string, or NULL if ID is invalid
 */
static char* get_position_name(int position_id) {
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
static player* find_player_by_name(const char* name) {
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
/* Command Functions                                                          */
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
    /* Check if formation with this name already exists */
    if (open_formation(args, NULL) == 0) {
        return SP_ERR_FORMATION_EXISTS;  /* Formation with this name already exists */
    }

    if (strlen(args[0]) > 39) {
        return SP_ERR_WRONG_USAGE;  /* Formation name cannot be empty */
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
    errno = 0;
    char *endptr;
    int age = strtol(args[1], &endptr, 10);
    int overall = strtol(args[2], &endptr, 10);
    int potential = strtol(args[3], &endptr, 10);
    int own = strtol(args[4], &endptr, 10);

    if (errno == ERANGE || endptr == args[1] || endptr == args[2] || endptr == args[3] || endptr == args[4]) {
        // invalid integer conversion for one of the arguments
        free(new_p->name);
        free(new_p);
        return SP_ERR_INTERNAL;  /* Conversion error */
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
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    position* pos = current_formation->map_of_positions[position_id];

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

    /* Build preferred index list from args (subset is allowed) */
    int *is_selected = calloc(pos->size_of_list, sizeof(int));
    if (!is_selected) {
        return SP_ERR_MEMORY;
    }

    // TODO: Can be further optimised
    int i = 1;
    while (args[i] != NULL) {
        int found_index = -1;
        for (int j = 0; j < pos->size_of_list; j++) {
            if (strcmp(pos->list_of_players[j]->name, args[i]) == 0) {
                found_index = j;
                break;
            }
        }
        if (found_index == -1) {
            free(is_selected);
            return SP_ERR_PLAYER_NOT_FOUND;  /* Player not found in position's list */
        }
        if (is_selected[found_index]) {
            free(is_selected);
            return SP_ERR_WRONG_USAGE;  /* Duplicate player in preference input */
        }
        is_selected[found_index] = 1;
        i++;
    }

    /* Create new list with preferred order first, then remaining stable order */
    player** new_list = malloc(sizeof(player*) * pos->size_of_list);
    if (!new_list) {
        free(is_selected);
        return SP_ERR_MEMORY;
    }

    int write_index = 0;
    if (backwards) {
        int last = count_tokens(args) - 1;
        for (int k = last; k >= 1; k--) {
            for (int j = 0; j < pos->size_of_list; j++) {
                if (strcmp(pos->list_of_players[j]->name, args[k]) == 0) {
                    new_list[write_index] = pos->list_of_players[j];
                    write_index++;
                    break;
                }
            }
        }
    } else {
        for (int k = 1; args[k] != NULL; k++) {
            for (int j = 0; j < pos->size_of_list; j++) {
                if (strcmp(pos->list_of_players[j]->name, args[k]) == 0) {
                    new_list[write_index] = pos->list_of_players[j];
                    write_index++;
                    break;
                }
            }
        }
    }

    for (int j = 0; j < pos->size_of_list; j++) {
        if (!is_selected[j]) {
            new_list[write_index] = pos->list_of_players[j];
            write_index++;
        }
    }

    free(pos->list_of_players);
    pos->list_of_players = new_list;
    free(is_selected);
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
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return SP_ERR_INVALID_POSITION;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return SP_ERR_NOT_ASSIGNED_POSITION;  /* Position not assigned in formation */
    }

    position* pos = current_formation->map_of_positions[position_id];
    if (pos->size_of_list == 0) {
        printf("No players assigned to position %s.\n", get_position_name(position_id));
        return SP_SUCCESS;
    }
    printf("Players for position %s:\n", get_position_name(position_id));

    for (int i = 0; i < pos->size_of_list; i++) {
        char *cleaned_name = strdup(pos->list_of_players[i]->name);
        if (!cleaned_name) {
            return SP_ERR_MEMORY;
        }
        if (clean_name_string(cleaned_name) != SP_SUCCESS) {
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
 */
int open_formation(char** args, char** options) {
    if (!formation_head) {
        return SP_ERR_NO_FORMATION;  /* No formations available */
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

int delete_formation(char** args, char** options) {
    if (!current_formation) {
        return SP_ERR_NO_FORMATION;  /* No formation currently open */
    }
    formation* current = formation_head;
    formation* previous = NULL;
    while (current != NULL) {
        if (strcmp(current->name, args[0]) == 0) {
            /* Found the formation to remove */
            if (previous == NULL) {
                /* head ist still current, therefore next will be the new head */
                formation_head = current->next;
            } 
            else {
                previous->next = current->next;
            }
            /* If the removed formation is the current formation, update current_formation */
            if (current_formation == current) {
                current_formation = formation_head;  /* Set to new head or NULL if list is empty */
            }
            /* Free the removed formation */
            cleanup_formation(current);
            break;
        }
        // iterative traverse
        previous = current;
        current = current->next;
    }
    return SP_SUCCESS;
}

int delete_player(char** args, char** options) {
    player* current = player_head;
    player* previous = NULL;
    while (current != NULL) {
        if (strcmp(current->name, args[0]) == 0) {
            /* Found the player to remove */
            if (previous == NULL) {
                /* head is still current, therefore next will be the new head */
                player_head = current->next;
            } else {
                previous->next = current->next;
            }
            /* Remove player from all positions in all formations */
            formation* f_current = formation_head;
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
    
    errno = 0;
    char *endptr;
    int new_value = strtol(new_value_str, &endptr, 10);
    if (errno == ERANGE || endptr == new_value_str) {
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



/* ========================================================================== */
/* Cleanup Function (exported)                                                */
/* ========================================================================== */

/**
 * @brief Free all application resources.
 * 
 * Frees all formations, positions, and players. Resets all global
 * pointers (formation_head, player_head, current_formation) to NULL.
 * 
 * @note After this call, current_formation is invalid (NULL).
 *       This function should only be called during application shutdown
 *       or before loading new data from a file (load_from_file).
 */
void cleanup_all(void) {
    while (formation_head != NULL) {
        formation* temp = formation_head;
        formation_head = formation_head->next;
        cleanup_formation(temp);
    }
    current_formation = NULL;
    while (player_head != NULL) {
        player* temp = player_head;
        player_head = player_head->next;
        cleanup_player(temp);
    }
}

/* ========================================================================== */
/* Persistence Accessor Functions                                             */
/* ========================================================================== */

/*
 * These functions expose the static head pointers so that the persistence
 * module can traverse the player and formation linked lists directly.
 * The struct definitions are already public via formation.h / player.h,
 * so no opaque wrappers are needed.
 */

squad_data get_squad_data(void) {
    squad_data data;
    data.player_head    = player_head;
    data.formation_head = formation_head;
    return data;
}

player* get_player_head(void) {
    return player_head;
}

formation* get_formation_head(void) {
    return formation_head;
}

/* --- Direct creation helpers for persistence load --- */

int create_player_direct(const char* name, int age,
                         int overall, int potential, int own) {
    if (!name) return SP_ERR_NULL_PTR;

    /* Check for duplicate */
    if (find_player_by_name((char*)name)) {
        return SP_ERR_PLAYER_EXISTS;
    }

    player* new_p = malloc(sizeof(player));
    if (!new_p) return SP_ERR_MEMORY;
    new_p->name = strdup(name);
    if (!new_p->name) { free(new_p); return SP_ERR_MEMORY; }
    new_p->age = age;
    new_p->overall_rating = overall;
    new_p->potential_rating = potential;
    new_p->own_rating = own;
    new_p->next = NULL;

    /* Append to player list */
    if (!player_head) {
        player_head = new_p;
    } else {
        player* cur = player_head;
        while (cur->next) cur = cur->next;
        cur->next = new_p;
    }
    return SP_SUCCESS;
}

int create_formation_direct(const char* name) {
    if (!name) return SP_ERR_NULL_PTR;

    /* Use a temporary args array to reuse new_formation() logic inline */
    formation* cur = formation_head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return SP_ERR_FORMATION_EXISTS;
        cur = cur->next;
    }

    formation* new_f = malloc(sizeof(formation));
    if (!new_f) return SP_ERR_MEMORY;
    new_f->name = strdup(name);
    if (!new_f->name) { free(new_f); return SP_ERR_MEMORY; }
    new_f->next = NULL;
    for (int i = 0; i < 24; i++) new_f->map_of_positions[i] = NULL;

    /* Append to list */
    if (!formation_head) {
        formation_head = new_f;
    } else {
        formation* tail = formation_head;
        while (tail->next) tail = tail->next;
        tail->next = new_f;
    }
    current_formation = new_f;
    return SP_SUCCESS;
}

int add_position_direct(const char* position_name) {
    if (!current_formation) return SP_ERR_NO_FORMATION;
    if (!position_name) return SP_ERR_NULL_PTR;

    int pid = parse_position_string((char*)position_name);
    if (pid == -1) return SP_ERR_INVALID_POSITION;

    /* If already assigned, skip silently (idempotent) */
    if (current_formation->map_of_positions[pid]) return SP_SUCCESS;

    position* pos = malloc(sizeof(position));
    if (!pos) return SP_ERR_MEMORY;
    pos->id = pid;
    pos->name = get_position_name(pid);
    pos->size_of_list = 0;
    pos->list_of_players = NULL;
    current_formation->map_of_positions[pid] = pos;
    return SP_SUCCESS;
}

int add_player_to_position_direct(const char* position_name,
                                  const char* player_name) {
    if (!current_formation) return SP_ERR_NO_FORMATION;
    if (!position_name || !player_name) return SP_ERR_NULL_PTR;

    int pid = parse_position_string((char*)position_name);
    if (pid == -1) return SP_ERR_INVALID_POSITION;
    if (!current_formation->map_of_positions[pid])
        return SP_ERR_NOT_ASSIGNED_POSITION;

    player* p = find_player_by_name((char*)player_name);
    if (!p) return SP_ERR_PLAYER_NOT_FOUND;

    position* pos = current_formation->map_of_positions[pid];
    player** new_list = realloc(pos->list_of_players,
                                sizeof(player*) * (pos->size_of_list + 1));
    if (!new_list) return SP_ERR_MEMORY;
    new_list[pos->size_of_list] = p;
    pos->list_of_players = new_list;
    pos->size_of_list++;
    return SP_SUCCESS;
}

int setting_no_current_formation(void) {
    current_formation = NULL;
    return SP_SUCCESS;
}
