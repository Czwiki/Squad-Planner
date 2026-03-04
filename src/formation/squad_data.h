/**
 * @file squad_data.h
 * @brief Defines the top-level squad_data container.
 *
 * squad_data bundles the player and formation linked lists into a
 * single struct so that higher-level modules (persistence, future
 * player menu, etc.) can work with one object instead of separate
 * head pointers.
 */

#ifndef SQUAD_DATA_H
#define SQUAD_DATA_H

#include "player.h"

/* Forward declaration – full definition lives in formation.h */
struct formation;

/**
 * @brief Top-level container for all squad data.
 *
 * Holds the head pointers for the global player and formation
 * linked lists.
 */
typedef struct squad_data {
    player*           player_head;     /**< Head of the player linked list */
    struct formation* formation_head;  /**< Head of the formation linked list */
} squad_data;

#endif /* SQUAD_DATA_H */
