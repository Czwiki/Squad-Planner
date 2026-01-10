#include "formation.h"
#include <stdio.h>

static formation*current_formation = NULL;

int parse_position_string(char *position_str) {
    for (int i = 0; i < 24; i++) {
        if (strcmp(positions[i], position_str) == 0) {
            return i;
        }
    }
    return -1; // Position not found
}

int new_formation(formation* form) {
    if (!form) {
        return -1;
    }
    current_formation = form;
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
        //if (current_formation->positions[i].name && strcmp(current_formation->positions[i].name, position_name) == 0) {
        //    position_id = current_formation->positions[i].id;
        //}
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