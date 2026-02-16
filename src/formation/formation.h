/**
 * @file formation.h
 * @brief Header file for formation management functions.
 * 
 * Defines the formation data structure and declares all functions
 * for creating, managing, and manipulating squad formations.
 * Functions are ordered by their command ID in parser.c.
 */

#ifndef FORMATION_H
#define FORMATION_H

#include "position.h"

/**
 * @brief Represents a squad formation with assigned positions.
 * 
 * A formation contains a name and an array of 24 possible positions.
 * Each position can be assigned (non-NULL) or unassigned (NULL).
 * Formations are stored as a linked list.
 */
typedef struct formation {
    char* name;                      /**< Formation name (unique identifier) */
    position* map_of_positions[24];  /**< Array of position pointers (NULL if unassigned) */
    struct formation* next;          /**< Next formation in the linked list */
} formation;

/* Formation commands - ordered by command ID in parser.c */

/* ID 1: new - Create a new formation */
int new_formation(char** args, char** options);

/* ID 2: newP - Create a new player */
int new_player(char** args, char** options);

/* ID 3: preference - Adjust player preferences in a position */
int preferences(char** args, char** options);

/* ID 4: add - Add positions to the formation */
int add_position_to_formation(char** args, char** options);

/* ID 5: addP - Add a player to a position */
int add_player_to_position(char** args, char** options);

/* ID 6: remove - Remove a position from the formation */
int remove_position_from_formation(char** args, char** options);

/* ID 7: removeP - Remove a player from a position */
int remove_player_from_position(char** args, char** options);

/* ID 8: list - List players assigned to a position */
int list_players_of_position(char** args, char** options);

/* ID 9: listf - List all formations */
int list_formations(char** options);

/* ID 10: open - Open an existing formation */
int open_formation(char** args, char** options);

/* ID 11: show - Display the formation in tactical view */
int show(char** options);

/* Utility functions */
player* find_player_by_name(char* name);
char* get_position_name(int position_id);

/* Cleanup function - frees all formations and players */
void cleanup_all(void);

#endif /* FORMATION_H */
