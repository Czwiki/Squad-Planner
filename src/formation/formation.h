/**
 * @file formation.h
 * @brief Header file for formation management functions.
 * 
 * Defines the formation data structure and declares all functions
 * for creating, managing, and manipulating squad formations.
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

/* Formation management functions */
int open_formation(char** args, char** options);
int new_formation(char** args, char** options);
int add_position_to_formation(char** args, char** options);
int show(char** options);
int list_formations(char** options);

/* Player management functions */
int new_player(char** args, char** options);
int add_player_to_position(char** args, char** options);
int list_players_of_position(char** args, char** options);
int remove_player_from_position(char** args, char** options);
int remove_position_from_formation(char** args, char** options);
int preferences(char** args, char** options);

/* Utility functions */
player* find_player_by_name(char* name);
char* get_position_name(int position_id);

#endif /* FORMATION_H */
