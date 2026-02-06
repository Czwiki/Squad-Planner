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
 * Data Structures:
 * - Formations are stored as a linked list (formation_head)
 * - Each formation has an array of 24 possible positions
 * - Each position can hold multiple players (for rotation/substitution planning)
 * - Players are stored in a separate global linked list (player_head)
 */

#include "formation.h"
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
    "RWB", "RCDM", "CDM", "LCDM", "LWB",            /* Defensive Mid */
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
static int sanity_check_and_help(char** options, char** args, int expected_args, 
                                  int expected_options, char* usage, char* description) {
    if (!args && !options) { // reduce redundancy in code by checking for null pointers at the beginning of the function
        return -1;
    }
    int options_count = count_tokens(options);
    int args_count = count_tokens(args);
    if (options_count == 1 && strcmp(options[0], "--help") == 0) {
        printf("Usage: %s\n%s\n", usage, description);
        if (fflush(stdout) != 0) {
            perror("Error flushing stdout: ");
            return -1;
        } 
        return 1;  /* Help page printed */
    }
    if (options_count > expected_options || args_count > expected_args) {
        return -2;  /* Too many options or arguments */
    }
    return 0;  /* Valid input */
}

/**
 * @brief Convert a position abbreviation string to its numeric ID.
 * 
 * @param position_str Position abbreviation (e.g., "GK", "ST", "CB")
 * @return Position ID (0-23) on success, -1 if not found
 */
int parse_position_string(char* position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1;  /* Position not found */
}

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
    char *usage = "open <formation_name> [--help]";
    char *description = "Opens the specified formation for editing. The formation must already exist.";
    int sanity_check = sanity_check_and_help(options, args, 1, 1, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return 0;
        return sanity_check;
    }

    if (!formation_head) {
        return -2;  /* No formations available */
    }

    formation* previous = formation_head;
    formation* current = previous;
    while (current != NULL) {
        if (strcmp(current->name, args[0]) == 0) {
            current_formation = current;  /* Found - set as current */
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return -2;  /* Formation not found */
}

/**
 * @brief Create a new formation with the given name.
 * 
 * Creates a new empty formation and adds it to the formation list.
 * The new formation becomes the current formation for editing.
 * 
 * @param args Array containing the formation name
 * @param options Array of options (supports --help)
 * 
 * @return 0 on success, -1 on memory error, -2 if name already exists
 */
int new_formation(char** args, char** options) {
    if (!args && !options) {
        return -1;
    }
    char *usage = "new <formation_name> [--help]";
    char *description = "Creates a new formation with the specified name. The name must be unique and not already used by an existing formation.";
    int sanity_check = sanity_check_and_help(options, args, 1, 1, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return 0;  /* Help displayed */
        return sanity_check;  /* Error */
    }

    /* Check if formation with this name already exists */
    if (open_formation(args, NULL) == 0) {
        return -2;  /* Formation with this name already exists */
    }

    /* Create the first formation in the list */
    if (formation_head == NULL) {
        formation_head = malloc(sizeof(formation));
        if (!formation_head) {
            return -1;
        }
        formation_head->name = strdup(args[0]);
        if (!formation_head->name) {
            free(formation_head);
            formation_head = NULL;
            return -1;
        }
        formation_head->next = NULL;
        current_formation = formation_head;
    } 
    else {
        /* Append to the end of the formation list */
        formation* current = formation_head;
        while (current->next != NULL) {
            if (strcmp(current->name, args[0]) == 0) {
                return -2;  /* Formation with this name already exists */
            }
            current = current->next;
        }
        formation* new_form = malloc(sizeof(formation));
        if (!new_form) {
            return -1;
        }
        new_form->name = strdup(args[0]);
        if (!new_form->name) {
            free(new_form);
            return -1;
        }
        new_form->next = NULL;
        current->next = new_form;
        current_formation = new_form;
    }

    /* Initialize all positions to empty */
    for (int i = 0; i < 24; i++) {
        current_formation->map_of_positions[i] = NULL;
    }  
    return 0;
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
 * @return 0 on success, -1 on memory error, -2 on invalid input
 */
int add_position_to_formation(char** args, char** options) {
    char *usage = "add <position_names>[1-11] [--help]";
    char *description = "Adds the specified positions to the current formation. The formation must already exist.";
    int sanity_check = sanity_check_and_help(options, args, 11, 1, usage, description);

    if (sanity_check != 0) {
        if (sanity_check == 1) return 0;
        return sanity_check;
    }

    if (!formation_head) {
        return -2;  /* No formations available */
    }
    
    if (!current_formation) {
        return -1;
    }
    
    /* Add each position specified in args */
    int k = 0;
    while (args[k] != NULL) {
        int position_id = parse_position_string(args[k]);
        if (position_id == -1) {
            return -2;  /* Position not found */
        }
        
        /* Count currently assigned positions */
        int assigned_count = 0;
        for (int i = 0; i < 24; i++) {
            if (current_formation->map_of_positions[i] != NULL) {
                assigned_count++;
            }
        }
        
        if (assigned_count >= 11) {
            return -2;  /* Formation already has 11 positions assigned */
        }
        
        /* Create and initialize the new position */
        position* temp = malloc(sizeof(position));
        if (!temp) {
            return -1;
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

/**
 * @brief Add a player to a specific position in the current formation.
 * 
 * Assigns an existing player to a position's candidate list.
 * The position must exist in the formation and the player must exist
 * in the global player list.
 * 
 * @param args Array: args[0] = position name, args[1] = player name
 * 
 * @return 0 on success, -1 if null args, -2 if position/player not found
 */
int add_player_to_position(char** args) {
    if (!current_formation || !args || !args[0] || !args[1]) {
        return -1;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2;  /* Position not assigned in formation */
    }
    player* p = find_player_by_name(args[1]);
    if (!p) {
        return -2;  /* Player not found */
    }
    position* pos = current_formation->map_of_positions[position_id];
    
    /* Check for duplicate - player already in position */
    for (int i = 0; i < pos->size_of_list; i++) {
        if (strcmp(pos->list_of_players[i]->name, p->name) == 0) {
            return -2;  /* Player already in the position's list */
        }
    }
    
    /* Append the player to the position's list */
    player** new_list = realloc(pos->list_of_players, sizeof(player*) * (pos->size_of_list + 1));
    if (!new_list) {
        return -1;
    }
    new_list[pos->size_of_list] = p;
    pos->list_of_players = new_list;
    pos->size_of_list = pos->size_of_list + 1;
    return 0;
}

/**
 * @brief Create a new player and add to the global player list.
 * 
 * Creates a player with the specified attributes and adds them
 * to the global player linked list.
 * 
 * @param args Array: [0]=name, [1]=age, [2]=overall, [3]=potential, [4]=own_rating
 *             All ratings should be 0-100, age > 0
 * 
 * @return 0 on success, -1 on memory/null error, -2 on invalid values
 */
int new_player(char** args) {
    if (!args || !args[0] || !args[1] || !args[2] || !args[3] || !args[4]) {
        return -1;
    }
    printf("Adding player: %s\n", args[0]);
    player* new_p = malloc(sizeof(player));
    if (!new_p) {
        return -1;
    }
    new_p->name = strdup(args[0]);
    if (!new_p->name) {
        free(new_p);
        return -1;
    }
    
    /* Parse player attributes from string arguments */
    int age = strtol(args[1], NULL, 10);
    int overall = strtol(args[2], NULL, 10);
    int potential = strtol(args[3], NULL, 10);
    int own = strtol(args[4], NULL, 10);
    
    if (errno == ERANGE) {
        free(new_p->name);
        free(new_p);
        return -1;  /* Conversion error */
    }
    
    /* Validate attribute ranges */
    if (age <= 0 || overall < 0 || potential < 0 || own < 0 || 
        overall > 100 || potential > 100 || own > 100) {
        free(new_p->name);
        free(new_p);
        return -2;  /* Invalid values */
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
    return 0;
}

/**
 * @brief List all players assigned to a specific position.
 * 
 * Displays the names of all players in a position's candidate list.
 * 
 * @param args Array: args[0] = position abbreviation
 * 
 * @return 0 on success, -1 if null args, -2 if position not found
 */
int list_players_of_position(char** args) {
    if (!current_formation || !args || !args[0]) {
        return -1;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2;  /* Position not assigned in formation */
    }

    position* pos = current_formation->map_of_positions[position_id];
    printf("Players for position %s:\n", get_position_name(position_id));

    for (int i = 0; i < pos->size_of_list; i++) {
        printf("- %s\n", pos->list_of_players[i]->name);
    }

    return 0;
}

/**
 * @brief List all available formations.
 * 
 * Displays the names of all formations in the formation list.
 * 
 * @return 0 always
 */
int list_formations(void) {
    formation* current = formation_head;
    if (!current) {
        printf("No formations available.\n");
        return 0;
    }
    printf("Available formations:\n");
    while (current != NULL) {
        printf("- %s\n", current->name);
        current = current->next;
    }
    return 0;
}

/**
 * @brief Remove a player from a position's candidate list.
 * 
 * Removes the specified player from the position's player list.
 * The player is not deleted from the global player list.
 * 
 * @param args Array: args[0] = position name, args[1] = player name
 * 
 * @return 0 on success, -1 if null args, -2 if position/player not found
 */
int remove_player_from_position(char** args) {
    if (!current_formation || !args || !args[0] || !args[1]) {
        return -1;
    }
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2;  /* Position not assigned in formation */
    }
    
    position* pos = current_formation->map_of_positions[position_id];
    int found_index = -1;
    
    /* Find the player in the position's list */
    for (int i = 0; i < pos->size_of_list; i++) {
        if (strcmp(pos->list_of_players[i]->name, args[1]) == 0) {
            found_index = i;
            break;
        }
    }
    if (found_index == -1) {
        return -2;  /* Player not found in position's list */
    }
    
    /* Shift players to remove the player at found_index */
    for (int i = found_index; i < pos->size_of_list - 1; i++) {
        pos->list_of_players[i] = pos->list_of_players[i + 1];
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
    return 0;
}

/**
 * @brief Reorder players in a position based on preference order.
 * 
 * Reorders the player list for a position based on the specified
 * preference order. Optionally reverses the order with -reverse flag.
 * 
 * @param options Array of options (supports -reverse to reverse order)
 * @param args Array: args[0] = position, args[1..n] = player names in order
 * 
 * @return 0 on success, -1 if null args/memory error, -2 if position/player not found
 */
int preferences(char** options, char** args) {
    int backwards = 0;
    if (!current_formation) {
        return -1;
    }
    if (!args || !args[0]) {
        return -1;
    }
    
    /* Check for -reverse option */
    if (options && options[0]) {
        if (strcmp(options[0], "-reverse") == 0) {
            backwards = 1;
        }
    }
    
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -2;  /* Position not assigned */
    }
    
    position* pos = current_formation->map_of_positions[position_id];
    if (pos->size_of_list <= 1) {
        return 0;  /* No reordering needed */
    }

    /* Build preference order array from args */
    int *preference = malloc(sizeof(int) * pos->size_of_list);
    if (!preference) {
        return -1;
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
            return -2;  /* Player not found in position's list */
        }
        i++;
    }
    
    /* Create new list with preferred order */
    player** new_list = malloc(sizeof(player*) * pos->size_of_list);
    if (!new_list) {
        free(preference);
        return -1;
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
    return 0;
}

/**
 * @brief Remove a position from the current formation.
 * 
 * Removes a position from the formation. The position must not have
 * any players assigned to it (remove players first).
 * 
 * @param args Array: args[0] = position abbreviation
 * 
 * @return 0 on success, -1 if null args, -2 if has players, -3 if not found
 */
int remove_position_from_formation(char** args) {
    if (!current_formation || !args || !args[0]) {
        return -1;
    }
    
    /* Check if any position has players assigned */
    for (int i = 0; i < 24; i++) {
        if (current_formation->map_of_positions[i] != NULL) {
            if (current_formation->map_of_positions[i]->list_of_players != NULL) {
                return -2;  /* Cannot remove position with assigned players */
            }
        }
    }
    
    int position_id = parse_position_string(args[0]);
    if (position_id == -1) {
        return -2;  /* Position not found */
    }
    if (current_formation->map_of_positions[position_id] == NULL) {
        return -3;  /* Position is already empty */
    }
    
    free(current_formation->map_of_positions[position_id]->list_of_players);
    free(current_formation->map_of_positions[position_id]);
    current_formation->map_of_positions[position_id] = NULL;
    return 0;
}

/**
 * @brief Display the current formation in a tactical view.
 * 
 * Renders the formation as a text-based tactical view showing
 * all assigned positions in their approximate field locations.
 * 
 * The display is a 6x5 grid representing a soccer pitch from
 * goal to goal, with positions placed according to their roles.
 * 
 * @return 0 on success, -1 if no current formation
 */
int show(void) {
    if (!current_formation) {
        return -1;
    }
    
    char empty = ' ';
    int curr_offset = 0;
    int first0 = 0;
    int first1 = 0;
    char* to_print[30] = {0};
    
    /* Initialize corner slots to empty */
    to_print[0] = &empty;
    to_print[1] = &empty;
    to_print[3] = &empty;
    to_print[4] = &empty;
    to_print[25] = &empty;
    to_print[29] = &empty;

    /* Map positions to display grid */
    for (int i = 0; i < 24; i++) {
        if (i >= 0 && !first0) {
            curr_offset += 2;
        }
        if (i >= 21 && !first1) {
            curr_offset += 1;
            first1 = 1;
        }
        if (current_formation->map_of_positions[i] != NULL) {
            to_print[i + curr_offset] = current_formation->map_of_positions[i]->name;
        }
        else {
            to_print[i + curr_offset] = &empty;
        }

        if (!first0) {
            curr_offset += 2;
            first0 = 1;
        }
    }
    
    /* Print grid from top to bottom (forwards to goalkeeper) */
    for (int i = 30; i > 0; i--) {
        if (i % 5 == 0 && i != 0) {
            printf("\n");
        }
        printf("%-6s", to_print[i-1]);
    } 
    printf("\n");
    return 0;
}
