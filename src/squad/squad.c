/**
 * @brief Valid commands in the squad menu context.
 * 
 * Index 0: help       - Display squad menu help
 * Index 1: new        - Create a new squad
 * Index 2: list       - List all squads
 * Index 3: open       - Open an existing squad
 * Index 4: formation   - Enter the formation menu
 * Index 5: players     - Enter the player menu (not yet implemented)
 * Index 6: back        - Return to main menu
 * Index 7: save        - Save current squad data (transitions to saves menu)
 */

#include "squad.h"
#include "../formation/formation.h"
#include "../formation/player.h"
#include "../error/error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Squad* squad_head = NULL;
Squad* current_squad = NULL;

int new_squad(char** args, char** options) {
    if (squad_head == NULL) {
        squad_head = malloc(sizeof(Squad));
        if (!squad_head) {
            return SP_ERR_MEMORY;
        }
        squad_head->name = strdup(args[0]);
        if (!squad_head->name) {
            free(squad_head);
            return SP_ERR_MEMORY;
        }
        squad_head->players = NULL;
        squad_head->formations = NULL;
        squad_head->next = NULL;
        current_squad = squad_head;
        return SP_SUCCESS;
    }
    Squad* temp = squad_head;
    while (temp->next != NULL) {
        if (strcmp(temp->name, args[0]) == 0) {
            return SP_ERR_SQUAD_EXISTS;  /* Squad with this name already exists */
        }
        temp = temp->next;
    }
    Squad* new_squad = malloc(sizeof(Squad));
    if (!new_squad) {
        return SP_ERR_MEMORY;
    }
    new_squad->name = strdup(args[0]);
    if (!new_squad->name) {
        free(new_squad);
        return SP_ERR_MEMORY;
    }
    new_squad->players = NULL;
    new_squad->formations = NULL;
    new_squad->next = NULL;
    temp->next = new_squad;
    current_squad = new_squad;

    return SP_SUCCESS;
}

int list_squads(void) {
    if (!squad_head) {
        printf("No squads have been created yet.\n");
        return SP_SUCCESS;
    }
    printf("Squads:\n");
    Squad* temp = squad_head;
    while (temp) {
        printf("- %s\n", temp->name);
        temp = temp->next;
    }
    return SP_SUCCESS;
}

int open_squad(char** args, char** options) {
    if (!squad_head) {
        return SP_ERR_NO_SQUAD;
    }
    Squad* temp = squad_head;
    while (temp) {
        if (strcmp(temp->name, args[0]) == 0) {
            current_squad = temp;
            return SP_SUCCESS;
        }
        temp = temp->next;
    }
    return SP_ERR_SQUAD_NOT_FOUND;  /* Squad with this name does not exist */
}

int to_formation(void) {
    if (!current_squad) {
        return SP_ERR_NO_SQUAD;
    }
    setting_formation_head(current_squad->formations);
    return SP_SUCCESS;
}

//int save_squad(char** args, char** options) {
//    if (!current_squad) {
//        return SP_ERR_NO_SQUAD;
//    }
//    setting_formation_head(current_squad->formations);  /* Ensure squad's formation head is up to date before saving */
//    return SP_SUCCESS;
//}

static int clean_name_string(char* name) {
    if (!name) {
        return SP_ERR_MEMORY;
    }
    for (int i = 0; i < strlen(name); i++) {
        if (name[i] == '_') {
            name[i] = ' ';
        }
    }
    return SP_SUCCESS;
}

// helper function for correct prompt in main.c
int get_current_squad_name(char *dest) {
    if (!current_squad) {
        dest[0] = '\0';  // set dest to empty string
        return SP_ERR_NO_SQUAD;  // No squad currently open, return empty name
    }
    char cleaned_name[40];
    strncpy(cleaned_name, current_squad->name, 40);  // copy up to 39 characters + null terminator
    clean_name_string(cleaned_name);
    strncpy(dest, cleaned_name, 40);  // copy up to 39 characters + null terminator
    dest[39] = '\0';  // ensure null termination
    return SP_SUCCESS;
}

/* Public setter used by other modules to update the current squad's
 * formation head pointer. This replaces the previous internal
 * `updating_formation_head` implementation.
 */
int set_current_squad_formations(formation* formation_head) {
    if (!current_squad) {
        return SP_ERR_NO_SQUAD;
    }
    current_squad->formations = formation_head;
    return SP_SUCCESS;
}
