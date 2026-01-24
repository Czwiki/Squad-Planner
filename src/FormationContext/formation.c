#include "formation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char * positions[] = {"GK", "RB", "RCB", "CB", "LCB", "LB", "RWB", "RCDM", "CDM", "LCDM", "LWB", "RM", "RCM", "CM", "LCM", "LM", "RAM", "RCAM", "CAM", "LCAM", "LAM", "RS", "ST", "LS"};


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

int add_position_to_formation(char **position_names) {
    if (!current_formation || !position_names) {
        return -1;
    }
    // Find the position ID based on the name
    int k = 0;
    while (position_names[k] != NULL) {
        int position_id = parse_position_string(position_names[k]);
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
        k++;
    }
    return 0;
}

int present_formation() {
    if (!current_formation) {
        return -1;
    }
    char *empty = " ";
    int curr_offset = 0;
    int first0 = 0;
    int first1 = 0;
    char *to_print[30] = {0};
    to_print[0] = empty;
    to_print[1] = empty;
    to_print[3] = empty;
    to_print[4] = empty;
    to_print[25] = empty;
    to_print[29] = empty;

    for (int i = 0; i < 24; i++) {
        if (i >= 0 && !first0){
            curr_offset +=2;
        }
        if (i >= 21 && !first1){
            curr_offset +=1;
            first1 = 1;
        }
        if (current_formation->map_of_positions[i] == 1) {
            to_print[i + curr_offset] = positions[i];
        }
        else {
            to_print[i + curr_offset] = empty;
        }

        if (!first0){
            curr_offset +=2;
            first0 = 1;
        }
    }
    for (int i = 30; i > 0; i--) {
        if (i % 5 == 0 && i != 0){
            printf("\n");
        }
        printf("%-6s", to_print[i-1]);
    } 

    return 0;
}