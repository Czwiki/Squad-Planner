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

#endif /* FORMATION_H */
