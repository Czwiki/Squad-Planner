
/**
 * @file squad.h
 * @brief Defines the Squad container struct.
 *
 * A Squad bundles the player and formation linked lists so that
 * higher-level modules (persistence, future player menu, etc.)
 * can work with one object instead of separate head pointers.
 */

#ifndef SQUAD_H
#define SQUAD_H

/* Forward declarations – full definitions live in player.h / formation.h */
struct player;
struct formation;

/**
 * @brief Top-level container for squad data.
 *
 * Holds the head pointers for the player and formation linked lists
 * that belong to this squad.
 */
struct Squad {
    char* name;                      /**< Squad name */
    struct player* players;          /**< Head of the player linked list */
    struct formation* formations;    /**< Head of the formation linked list */
    struct Squad* next;              /**< Next squad in the linked list */
};
typedef struct Squad Squad;

Squad* squad_head;  /* Global head pointer for the squad linked list */

int new_squad(char** args, char** options);
int list_squads(void);
int open_squad(char** args, char** options);
int to_formation(void);
int to_players(void);
int save_squad(char** args, char** options);

int get_current_squad_name(char* dest);

/* Set the formations head pointer for the currently active squad.
 * This is the public setter used by other modules (e.g., formation.c)
 * to update the squad's stored formation list.
 */
int set_current_squad_formations(struct formation* head);

#endif /* SQUAD_H */
