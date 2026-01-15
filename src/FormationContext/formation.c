#include "formation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * positions[] = {"GK", "LWB", "LB", "LCB", "CB", "RCB", "RDB", "RWB", "LCDM", "CDM", "RCDM", "LM", "LCM", "CM", "RCM", "RM", "LAM", "LCAM", "CAM", "RCAM", "RAM", "LS", "ST", "RS"};


static formation*formation_head = NULL; 
static formation*current_formation = NULL;

int parse_position_string(char *position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1; // Position not found
}

int open_formation(char* name) {
    if (!name) {
        return -1;
    }
    formation* previous = formation_head;
    formation* current = previous;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Found the formation
            current_formation = current;
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return -2; // Formation not found
}

int new_formation(char* name) {
    if (!name) {
        return -1;
    }
    if (open_formation(name) == 0) {
        return -2; // Formation with this name already exists
    }

    if (formation_head == NULL) {
        formation_head = malloc(sizeof(formation));
        if (!formation_head) {
            return -1;
        }
        formation_head->name = strdup(name);
        if (!formation_head->name) {
            free(formation_head);
            formation_head = NULL;
            return -1;
        }
        formation_head->next = NULL;
        current_formation = formation_head;
    } 
    else {
        formation* current = formation_head;
        while (current->next != NULL) {
            current = current->next;
        }
        formation* new_form = malloc(sizeof(formation));
        if (!new_form) {
            return -1;
        }
        new_form->name = strdup(name);
        if (!new_form->name) {
            free(new_form);
            return -1;
        }
        new_form->next = NULL;
        current->next = new_form;
        current_formation = new_form;
    }

    for (int i = 0; i < 24; i++) {
        current_formation->map_of_positions[i] = 0; // -1 indicates no player assigned to that position
    }  
    return 0;
}

int add_position_to_formation(char *position_name) {
    if (!current_formation || !position_name) {
        return -1;
    }
    // Find the position ID based on the name
    int position_id = parse_position_string(position_name);
    if (position_id == -1) {
        return -2; // Position not found
    }
    int assigned_count = 0;
    for (int i = 0; i < 24; i++) {
        if (current_formation->map_of_positions[i] != 0) {
            assigned_count++;
        }
    }
    if (position_id == -1) {
        return -2; // Position not found
    }
    if (assigned_count >= 11) {
        return -3; // Formation already has 11 positions assigned
    }
    current_formation->map_of_positions[position_id] = 1; // 1 indicates position is filled
    return 0;
}

int present_formation() {
    if (!current_formation) {
        return -1;
    }
    printf("Current Formation Positions:\n");
    for (int i = 0; i < 24; i++) {
        if (current_formation->map_of_positions[i] == 1) {
            printf("- %s\n", positions[i]);
        }
    }
    return 0;
}