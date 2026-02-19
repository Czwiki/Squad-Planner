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

/* Cleanup function - frees all formations and players */
void cleanup_all(void);

/* ========================================================================== */
/* Persistence accessor types and functions                                   */
/* ========================================================================== */

/**
 * @brief Opaque iterator for reading player data during serialization.
 *
 * Used by the persistence module to traverse the global player list
 * without exposing internal data structures.
 */
typedef struct player_iter player_iter;

/**
 * @brief Opaque iterator for reading formation data during serialization.
 *
 * Used by the persistence module to traverse the global formation list
 * without exposing internal data structures.
 */
typedef struct formation_iter formation_iter;

/* Player iteration – used by persistence save */
player_iter* player_iter_first(void);
player_iter* player_iter_next(player_iter* it);
const char*  player_iter_name(const player_iter* it);
int          player_iter_age(const player_iter* it);
int          player_iter_overall(const player_iter* it);
int          player_iter_potential(const player_iter* it);
int          player_iter_own(const player_iter* it);

/* Formation iteration – used by persistence save */
formation_iter* formation_iter_first(void);
formation_iter* formation_iter_next(formation_iter* it);
const char*     formation_iter_name(const formation_iter* it);

/**
 * @brief Get position info for a formation at the given slot (0-23).
 *
 * @param it  Formation iterator
 * @param slot Position slot index (0-23)
 * @param out_name  Set to the position abbreviation, or NULL if slot is empty
 * @param out_count Set to the number of players assigned
 * @return 1 if the slot is occupied, 0 if empty
 */
int formation_iter_position(const formation_iter* it, int slot,
                            const char** out_name, int* out_count);

/**
 * @brief Get the name of the k-th player in a position slot.
 *
 * @param it   Formation iterator
 * @param slot Position slot index (0-23)
 * @param k    Player index within the position's list
 * @return Player name, or NULL on error
 */
const char* formation_iter_pos_player(const formation_iter* it, int slot, int k);

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

#endif /* FORMATION_H */
