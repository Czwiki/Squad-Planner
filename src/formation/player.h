/**
 * @file player.h
 * @brief Defines the player data structure.
 * 
 * A player represents a soccer player with ratings and attributes.
 * Players are stored in a global linked list and can be assigned
 * to multiple positions across different formations.
 */

#ifndef PLAYER_H
#define PLAYER_H

/**
 * @brief Represents a soccer player with ratings.
 * 
 * Players have various rating attributes (0-100 scale):
 * - overall_rating: Current ability level
 * - potential_rating: Maximum possible ability
 * - own_rating: User's personal assessment
 */
typedef struct player {
    char* name;              /**< Player's name (unique identifier) */
    int age;                 /**< Player's age (must be > 0) */
    int overall_rating;      /**< Current overall rating (0-100) */
    int potential_rating;    /**< Potential rating (0-100) */
    int own_rating;          /**< User's personal rating (0-100) */
    struct player* next;     /**< Next player in the linked list */
} player;

#endif /* PLAYER_H */
