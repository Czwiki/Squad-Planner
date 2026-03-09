/**
 * @file position.h
 * @brief Defines the position data structure.
 * 
 * A position represents a specific role in a soccer formation
 * (e.g., GK, CB, ST). Each position can have multiple players
 * assigned to it as candidates or rotation options.
 */

#ifndef POSITION_H
#define POSITION_H

#include "../player/player.h"

/**
 * @brief Represents a position in a formation.
 * 
 * Positions are identified by their ID (0-23, matching the position
 * abbreviation array in formation.c). Each position maintains a list
 * of assigned players.
 */
typedef struct position {
    int id;                     /**< Position ID (0-23) */
    char* name;                 /**< Position abbreviation (e.g., "GK", "ST") */
    int size_of_list;           /**< Number of players assigned */
    player** list_of_players;   /**< Array of pointers to assigned players */
} position;

#endif /* POSITION_H */
