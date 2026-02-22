/**
 * @file exec.c
 * @brief Command execution module for the Squad-Planner application.
 * 
 * This module is responsible for executing parsed commands based on
 * the current application context. It acts as a dispatcher that routes
 * commands to their appropriate handlers in other modules.
 * 
 * Command execution flow:
 * 1. execute_command() receives a parsed command and context
 * 2. Based on context, it calls the appropriate exec_*_command() function
 * 3. The specific executor calls the actual implementation function
 * 
 * Return values follow a consistent pattern:
 * -  0: Success
 * - -1: General error (memory, null pointer, etc.)
 * - -2: Invalid options/arguments for the command
 */

#include "exec.h"
#include "../command.h"
#include "../help/help.h"
#include "../formation/formation.h"
#include "../persistence/persistence.h"
#include "../error/error.h"
#include <stdio.h>

/**
 * @brief Execute a command in the main menu context.
 * 
 * Handles commands available in the main menu:
 * - help: Display main menu help
 * - formation: (handled via context switch)
 * - load: (handled via context switch to saves menu)
 * 
 * @param cmd_id Command identifier (index in command list)
 */
int exec_main_command(int cmd_id) {
    int ret_val = 0;
    switch (cmd_id) {
    case 0:  /* help */
        ret_val = print_help_page_main();
        break;
    default:
        break;
    }
    return ret_val;
}

/**
 * @brief Execute a command in the formation menu context.
 * 
 * Handles all formation-related commands including creating formations,
 * managing positions, and managing players.
 * 
 * @param cmd_id Command identifier
 * @param options Array of option strings
 * @param args Array of argument strings
 */
int exec_formation_command(int cmd_id, char** options, char** args) {
    int ret_val = 0;
    switch (cmd_id) {
    case 0:   /* help */
        ret_val = print_help_page_formation();
        break;
    case 1:   /* new */
        ret_val = new_formation(args, options);
        break;
    case 2:   /* newP */
        ret_val = new_player(args, options);
        break;
    case 3:   /* preference */
        ret_val = preferences(args, options);
        break;
    case 4:   /* add */
        ret_val = add_position_to_formation(args, options);
        break;
    case 5:   /* addP */
        ret_val = add_player_to_position(args, options);
        break;
    case 6:   /* remove */
        ret_val = remove_position_from_formation(args, options);
        break;
    case 7:   /* removeP */
        ret_val = remove_player_from_position(args, options);
        break;
    case 8:   /* list */
        ret_val = list_players_of_position(args, options);
        break;
    case 9:   /* listf */
        ret_val = list_formations(args, options);
        break;
    case 10:  /* open */
        ret_val = open_formation(args, options);
        break;
    case 11:  /* show */
        ret_val = show(options);
        break;
    case 12:  /* deletef */
        ret_val = delete_formation(args, options);
        break;
    case 13: /* deleteP */
        ret_val = delete_player(args, options);
        break;
    case 14:  /* editP */
        ret_val = edit_player(args, options);
        break;
    case 15:  /* save – persist all data to JSON file */
        ret_val = save_to_file(args ? args[0] : NULL);
        break;
    default:
        break;
    }
    return ret_val;
}

/**
 * @brief Execute a command in the saves/load menu context.
 * 
 * Handles commands for saving and loading persistent data:
 * - help: Display saves menu help
 * - save: Write all data to JSON file
 * - load: Read all data from JSON file
 * - back: (handled via context switch)
 * 
 * @param cmd_id Command identifier
 * @param options Array of option strings (may be NULL)
 * @param args Array of argument strings (optional filename)
 */
int exec_load_command(int cmd_id, char** options, char** args) {
    int ret_val = 0;
    switch (cmd_id) {
    case 0:  /* help */
        ret_val = print_help_page_saves();
        break;
    case 1:  /* save – persist all data to JSON file */
        ret_val = save_to_file(args ? args[0] : NULL);
        break;
    case 2:  /* load – restore all data from JSON file */
        ret_val = load_from_file(args ? args[0] : NULL);
        if (ret_val == SP_SUCCESS) {
            setting_no_current_formation();
        }
        break;
    case 3:  /* back – handled via context switch */
        break;
    default:
        break;
    }
    return ret_val;
}

/**
 * @brief Main command execution dispatcher.
 * 
 * Routes a parsed command to the appropriate context-specific
 * executor function based on the current application context.
 * 
 * Special context 99 is used for cleanup on program exit.
 * 
 * @param cmd Pointer to the parsed command structure (may be NULL for cleanup)
 * @param context Current application context (0=main, 1=formation, 2=saves, 99=cleanup)
 */
int execute_command(command* cmd, int context) {
    /* SP_CONTEXT_CLEANUP is the cleanup sentinel – cmd may be NULL */
    if (context == SP_CONTEXT_CLEANUP) {
        cleanup_all();
        return SP_SUCCESS;
    }
    if (!cmd) return SP_ERR_NULL_PTR;

    printf("Executing command ID %d in context %d\n", cmd->id, context);
    int ret_val = 0;
    switch (context) {
    case 0:  /* Main menu */
        ret_val = exec_main_command(cmd->id);
        break;
    case 1:  /* Formation menu */
        ret_val = exec_formation_command(cmd->id, cmd->options, cmd->args);
        break;
    case 2:  /* Saves menu */
        ret_val = exec_load_command(cmd->id, cmd->options, cmd->args);
        break; 
    default:
        break;
    }
    return ret_val;
}

/* interface to limit includes into main.c */
int current_formation_name(char* dest) {
    return get_current_formation_name(dest);
}
