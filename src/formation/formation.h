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
#include "../squad/squad.h"

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

int get_current_formation_name(char* dest);

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

/* ID 12: deletef - Remove an existing formation */
int delete_formation(char** args, char** options);

/* ID 13: deleteP - Remove an existing player */
int delete_player(char** args, char** options);

/* ID 14: editP - Edit an existing player */
int edit_player(char** args, char** options);

/**/

/* Cleanup function - frees all formations and players */
void cleanup_all(void);
int get_pos_size_of_list(char* arg);

/* ========================================================================== */
/* Persistence accessor functions                                             */
/* ========================================================================== */

/**
 * @brief Get a snapshot of all squad data (players and formations).
 *
 * Returns a Squad struct whose fields point to the current heads
 * of the global player and formation linked lists.
 * @return Squad with players and formations set.
 */
Squad get_squad(void);

int setting_squad_formation(Squad* squad);

/**
 * @brief Get the head of the global player linked list.
 *
 * Used by the persistence module for serialization.
 * @return Pointer to the first player, or NULL if list is empty.
 */
player* get_player_head(void);

/**
 * @brief Get the head of the global formation linked list.
 *
 * Used by the persistence module for serialization.
 * @return Pointer to the first formation, or NULL if list is empty.
 */
formation* get_formation_head(void);

/* Direct creation helpers – used by persistence load (bypass arg parsing) */

/**
 * @brief Create a player directly from typed values.
 *
 * @return SP_SUCCESS, SP_ERR_MEMORY, SP_ERR_PLAYER_EXISTS
 */
int create_player_direct(const char* name, int age,
                         int overall, int potential, int own);

/**
 * @brief Create a formation directly from a name.
 *
 * The new formation becomes the current formation.
 * @return SP_SUCCESS, SP_ERR_MEMORY, SP_ERR_FORMATION_EXISTS
 */
int create_formation_direct(const char* name);

/**
 * @brief Add a position slot to the current formation directly.
 *
 * @param position_name Position abbreviation (e.g. "GK", "ST")
 * @return SP_SUCCESS, SP_ERR_INVALID_POSITION, SP_ERR_NO_FORMATION
 */
int add_position_direct(const char* position_name);

/**
 * @brief Assign an existing player to a position in the current formation.
 *
 * @param position_name Position abbreviation
 * @param player_name   Player name (must exist in global list)
 * @return SP_SUCCESS, SP_ERR_PLAYER_NOT_FOUND, SP_ERR_NO_FORMATION
 */
int add_player_to_position_direct(const char* position_name,
                                  const char* player_name);

int setting_no_current_formation(void);

#endif /* FORMATION_H */

