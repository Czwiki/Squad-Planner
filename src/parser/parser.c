/**
 * @file parser.c
 * @brief Command line parser for the Squad-Planner application.
 * 
 * This module is responsible for parsing user input from the command line
 * and converting it into a structured command object that can be executed.
 * 
 * The parser supports three different contexts (menus):
 * - Main menu (context 0): Entry point with basic navigation commands
 * - Formation menu (context 1): Commands for creating and managing formations
 * - Saves menu (context 2): Commands for saving/loading data
 * 
 * Each context has its own set of valid commands. The parser validates
 * the command name against the current context and extracts any options
 * (prefixed with '-') and arguments.
 * 
 * Command Structure:
 * - Commands follow the format: <command_name> [options...] [arguments...]
 * - Options start with '-' (e.g., --help, -reverse)
 * - Arguments are space-separated tokens that don't start with '-'
 * 
 * Error Codes:
 * - Returns  0 on success
 * - Returns -1 on memory allocation failure or internal error
 * - Returns -2 on invalid command format
 * - Returns -3 when command is not recognized in current context
 */

#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/**
 * @brief Valid commands in the main menu context.
 * 
 * Index 0: help      - Display help information
 * Index 1: formation - Enter the formation planning menu
 * Index 2: load      - Enter the saves/load menu
 */
static char* command_inputs_main[3] = {"help", "formation", "load"};

/**
 * @brief Valid commands in the formation menu context.
 * 
 * Index  0: help       - Display formation menu help
 * Index  1: new        - Create a new formation
 * Index  2: newP       - Create a new player
 * Index  3: preference - Adjust player preferences in a position
 * Index  4: add        - Add positions to the formation
 * Index  5: addP       - Add a player to a position
 * Index  6: remove     - Remove a position from the formation
 * Index  7: removeP    - Remove a player from a position
 * Index  8: list       - List players assigned to a position
 * Index  9: listf      - List all formations
 * Index 10: open       - Open an existing formation
 * Index 11: show       - Display the formation in tactical view
 * Index 12: save       - Save the current formation (transitions to saves menu)
 * Index 13: back       - Return to main menu
 */
static char* command_inputs_formation[14] = {
    "help", "new", "newP", "preference", "add", "addP",
    "remove", "removeP", "list", "listf", "open", "show", "save", "back"
};

/**
 * @brief Valid commands in the saves menu context.
 * 
 * Index 0: help - Display saves menu help
 * Index 1: save - Save data to file
 * Index 2: back - Return to main menu
 */
static char* command_inputs_saves[3] = {"help", "save", "back"};

/**
 * @brief Parse a command line string into a structured command object.
 * 
 * This function tokenizes the input line and populates the command structure
 * with the command ID, name, options, and arguments. It also determines if
 * the command should trigger a context change.
 * 
 * Parsing Process:
 * 1. Copy the input line to a working buffer (original is const)
 * 2. Select the valid command list based on current context
 * 3. Extract the first token as the command name
 * 4. Validate the command name against the valid commands list
 * 5. Assign command ID (index in the commands array)
 * 6. Determine if command triggers a context change
 * 7. Parse remaining tokens as options (start with '-') or arguments
 * 8. NULL-terminate the options and args arrays for safe iteration
 * 
 * @param line The input string to parse (will not be modified)
 * @param current_context The current menu context (0=main, 1=formation, 2=saves)
 * @param cmd Pointer to command structure to populate with parsed data
 * 
 * @return 0 on success
 * @return -1 on memory allocation failure or if cmd is NULL
 * @return -2 on invalid command format (e.g., starts with '-')
 * @return -3 when command is not recognized in current context
 * 
 * @note The caller is responsible for freeing the allocated memory in cmd
 *       (name, options array and its elements, args array and its elements)
 */
int parse_command(const char* line, int current_context, command* cmd) {
    /* Validate that we have a valid command structure to populate */
    if (!cmd) {
        return -1;
    }
    
    /* 
     * Create a mutable copy of the input line for tokenization.
     * strtok() modifies the string in place, so we cannot use the original.
     */
    char* buffer = NULL;
    if (!(buffer = strdup(line))) {
        return -1;
    }
    
    /*
     * Select the appropriate command list based on the current context.
     * Each context has different valid commands.
     */
    char** command_inputs;
    int length = 0;
    switch (current_context) {
        case 0:  /* Main menu context */
            command_inputs = command_inputs_main;
            length = 3;
            break;
        case 1:  /* Formation menu context */
            command_inputs = command_inputs_formation;
            length = 14;
            break;
        case 2:  /* Saves menu context */
            command_inputs = command_inputs_saves;
            length = 3;
            break;
        default:
            free(buffer);
            return -2;
    }

    /* Track potential context changes triggered by this command */
    int new_context = current_context;

    /* Initialize command structure fields to safe defaults */
    cmd->id = 0;
    cmd->name = NULL;
    cmd->options = NULL;
    cmd->args = NULL;

    int argv0 = 0;          /* Flag: have we processed the command name yet? */
    int options_count = 0;   /* Number of options parsed so far */
    int args_count = 0;      /* Number of arguments parsed so far */
    
    /*
     * Tokenize the input string by spaces.
     * First token = command name, subsequent tokens = options or arguments.
     */
    char* token = strtok(buffer, " ");
    while (token != NULL) {
        
        if (argv0 == 0) {
            /*
             * Processing the command name (first token).
             * Command names cannot start with '-' (that's an option).
             */
            if (token[0] == '-') {
                free(buffer);
                return -2;
            }
            
            /* Search for the command name in the valid commands list */
            int found = 0;
            for (int i = 0; i < length; i++) {
                if (strcmp(token, command_inputs[i]) == 0) {
                    cmd->id = i;
                    
                    /*
                     * Determine context transitions based on command.
                     * Some commands change the application context/menu.
                     */
                    switch (current_context) {
                    case 0:  /* Main menu */
                        if (i == 1) {
                            new_context = 1;  /* 'formation' -> enter formation menu */
                        }
                        /* Note: 'load' (i==2) no longer changes context - it loads directly */
                        else if (i == 2) {
                            new_context = 2;  /* 'load' -> enter saves/load menu */
                        }
                        break;
                    case 1:  /* Formation menu */
                        if (i == 13) {
                            new_context = 0;  /* 'back' -> return to main menu */
                        }
                        /* Note: 'save' (i==12) no longer changes context - it saves directly */
                        break;
                    case 2:  /* Saves menu */
                        if (i == 2) {
                            new_context = 0;  /* 'back' -> return to main menu */
                        }
                        break;
                    default:
                        break;
                    }
                    found = 1;
                    break;
                }
            }
            
            /* Command not found in valid commands for this context */
            if (!found) {
                free(buffer);
                return -3;
            }
            
            /* Store a copy of the command name */
            cmd->name = strdup(token);
            if (!cmd->name) {
                free(buffer);
                return -1;
            }
            argv0++;  /* Mark that we've processed the command name */
        }
        else {
            /*
             * Processing subsequent tokens (options and arguments).
             * Options start with '-', everything else is an argument.
             */
            if (token[0] == '-') {
                /* This token is an option (e.g., --help, -reverse) */
                char* temp = strdup(token);
                if (!temp) {
                    free(buffer);
                    free(cmd->name);
                    return -1;
                }
                
                /* Grow the options array to accommodate the new option */
                char** new_opts = realloc(cmd->options, sizeof(char*) * (options_count + 1));
                if (!new_opts) {
                    free(buffer);
                    free(temp);
                    return -1;
                }
                cmd->options = new_opts;
                cmd->options[options_count] = temp;
                options_count++;
            } 
            else {
                /* This token is an argument (doesn't start with '-') */
                char* temp = strdup(token);
                if (!temp) {
                    free(buffer);
                    return -1;
                }
                
                /* Grow the args array to accommodate the new argument */
                char** new_args = realloc(cmd->args, sizeof(char*) * (args_count + 1));
                if (!new_args) {
                    free(buffer);
                    free(temp);
                    return -1;
                }
                cmd->args = new_args;
                cmd->args[args_count] = temp;
                args_count++;
            }
        }
        token = strtok(NULL, " ");  /* Get next token */
    }
    
    /*
     * Note: The following validation code was commented out but could be
     * enabled for stricter argument validation per command.
     */
    /*if (cmd->id == 1 && current_context == 1) {
        if (args_count != 1) {
            free(buffer);
            return -2;
        }
        if (strlen(cmd->args[0]) > 32) {
            free(buffer);
            return -2;
        }
    }*/

    /*
     * Check for any system errors that occurred during parsing.
     * EINVAL is acceptable (no conversion in strtok), but other errors indicate failure.
     */
    if (errno != 0 && errno != EINVAL) {
        free(buffer);
        free(cmd->name);
        for (int j = 0; j < options_count; j++) free(cmd->options[j]);
        free(cmd->options);
        for (int j = 0; j < args_count; j++) free(cmd->args[j]);
        free(cmd->args);
        return -1;
    }
    
    /*
     * Add NULL terminators to options and args arrays.
     * This allows safe iteration without needing to track counts separately.
     * Functions can iterate until they hit NULL (similar to argv[argc] == NULL).
     */
    if (options_count > 0) {
        char** new_opts = realloc(cmd->options, sizeof(char*) * (options_count + 1));
        if (!new_opts) {
            free(buffer);
            return -1;
        }
        cmd->options = new_opts;
        cmd->options[options_count] = NULL;  /* NULL terminator */
    }
    if (args_count > 0) {
        char** new_args = realloc(cmd->args, sizeof(char*) * (args_count + 1));
        if (!new_args) {
            free(buffer);
            return -1;
        }
        cmd->args = new_args;
        cmd->args[args_count] = NULL;  /* NULL terminator */
    }
    
    free(buffer);
    cmd->future_context = new_context;  /* Store the determined context transition */
    return 0;  /* Success */
}
