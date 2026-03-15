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
#include "../squad/squad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Count the number of elements in a NULL-terminated string array.
 * 
 * @param args NULL-terminated array of strings
 * @return Number of elements (not including the NULL terminator)
 */
static int count_tokens(char** args) {
    int i = 0;
    if (!args) return 0;
    while (args[i]) i++;
    return i; 
}

/**
 * @brief Validate input and optionally display help information.
 * 
 * This utility function performs common validation for command handlers:
 * 1. Checks for --help option and displays usage if present
 * 2. Validates that option/argument counts don't exceed limits
 * 
 * @param options Array of option strings (may be NULL)
 * @param args Array of argument strings (may be NULL)
 * @param expected_args Maximum number of arguments allowed
 * @param expected_options Maximum number of options allowed
 * @param usage Usage string to display for help
 * @param description Description string to display for help
 * 
 * @return 0 if validation passes
 * @return 1 if help was displayed (caller should return success)
 * @return -1 if both args and options are NULL
 * @return -2 if too many options or arguments provided
 */
static int sanity_check_and_help(char** args, char** options, int max_args, int min_args, int max_options, int min_options, char* usage, char* description) {
    int args_count = count_tokens(args);
    int options_count = count_tokens(options);
    if (options_count == 1 && options && options[0] && strcmp(options[0], "--help") == 0) {
        printf("Usage: %s\n%s\n", usage, description);
        if (fflush(stdout) != 0) {
            perror("Error flushing stdout: ");
            return SP_ERR_INTERNAL;
        }
        return 1;  /* Help page printed */
    }
    if (args_count > max_args || options_count > max_options || args_count < min_args || options_count < min_options) {
        return SP_ERR_WRONG_USAGE;  /* Too many options or arguments */
    }
    return 0;  /* Valid input */
}

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
static int exec_main_command(int cmd_id) {
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

static int exec_squad_command(int cmd_id, char **args, char **options) {
    int ret_val = 0;
    char *usage = NULL;
    char *description = NULL;
    int sanity_check = 0;
    switch (cmd_id) {
        case 0:  /* help */
            ret_val = print_help_page_squad();
            break;
        case 1: /* new */
            usage = "new <squad_name>[max. 39 characters] [--help]";
            description = "Creates a new squad with the specified name. The name must be unique and not already used by an existing squad. If you wish to use a name consisting of multiple words, use underscores instead of spaces (e.g., 'My_Squad'). The new squad becomes the current squad for editing.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = new_squad(args, options);
            break;
        case 2: /* list */
            usage = "list [--help]";
            description = "Displays a list of all squads that have been created.";
            sanity_check = sanity_check_and_help(args, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = list_squads();
            break;
        case 3: /* open */
            usage = "open <squad_name> [--help]";
            description = "Opens an existing squad.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = open_squad(args, options);
            break;
        case 4:
            /* formation command is handled via context switch, but we can still provide help info here */
            usage = "formation [--help]";
            description = "Enters the formation menu for the current squad. You must have a squad open to use this command.";
            sanity_check = sanity_check_and_help(args, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = to_formation();
            break;
        case 5: /* players command is currently a placeholder for future expansion */
            usage = "players [--help]";
            description = "Enters the player menu for the current squad. This menu is under development and will allow you to manage your player roster in future updates.";
            sanity_check = sanity_check_and_help(args, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = to_players();
            break;
        //case 7: /* save */
        //    usage = "save [--help]";
        //    description = "Saves the current squad data. This command transitions to the saves menu where you can choose to save to a file or return to the squad menu.";
        //    sanity_check = sanity_check_and_help(args, options, 0, 0, 1, 0, usage, description);
        //    if (sanity_check != 0) {
        //        if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
        //        else ret_val = sanity_check;  /* Error */
        //        break;
        //    }
        //    ret_val = save_squad(args, options);
        //    break;
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
static int exec_formation_command(int cmd_id, char** args, char** options) {
    int ret_val = 0;
    char *usage = NULL;
    char *description = NULL;
    int sanity_check = 0;
    switch (cmd_id) {
        case 0:   /* help */
            ret_val = print_help_page_formation();
            break;
        case 1:   /* new */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "new <formation_name>[max. 39 characters] [--help]";
            description = "Creates a new formation with the specified name. The name must be unique and not already used by an existing formation. If you wish to use a name consisting of multiple words, use underscores instead of spaces(e.g., '4_3_3', '4_2_3_1'). The new formation becomes the current formation for editing.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val =  sanity_check;  /* Error */
                break;
            }
            ret_val = new_formation(args, options);
            break;
        case 2:   /* preference */
            if (!args && !options) { 
                ret_val =  SP_ERR_WRONG_USAGE;
                break;
            }
            if (!args || !args[0]) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "preference <position> [player1 player2 ...] [-reverse] [--help]";
            description = "Reorder players in a position by listing preferred players first. Unlisted players keep their existing relative order. Use -reverse to invert only the listed preference order.";
            int arg_num = get_pos_size_of_list(args[0]);  /* position name + all players in the position */
            if (arg_num < 0) {
                ret_val = arg_num;  // errors from formation.c helper function
                break;
            }
            sanity_check = sanity_check_and_help(args, options, 1, arg_num+1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = preferences(args, options);
            break;
        case 3:   /* add */
            if (!args && !options) { // reduce redundancy in code by checking for null pointers at the beginning of the function
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "add <position_names>[1-11] [--help]";
            description = "Adds the specified positions to the current formation. The formation must already exist.";
            sanity_check = sanity_check_and_help(args, options, 11, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = add_position_to_formation(args, options);
            break;
        case 4:   /* addP */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "addP <position_name> <player_names>[1-10] [--help]";
            description = "Adds the specified players to the candidate list for the specified position in the current formation. The position must already be assigned in the formation, and the player must exist in the global player list.";
            sanity_check = sanity_check_and_help(args, options, 11, 2, 1, 0, usage, description);

            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = add_player_to_position(args, options);
            break;
        case 5:   /* remove */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE; // one must be present
                break;
            }
            usage = "remove <position_name>[1-5] [--help]";
            description = "Removes the specified position from the current formation. Position must not have players assigned.";
            sanity_check = sanity_check_and_help(args, options, 5, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = remove_position_from_formation(args, options);
            break;
        case 6:   /* removeP */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "removeP <position_name> <player_names>[1-10] [-all --help]";
            description = "Removes the specified players from the candidate list of the specified position. With -all option, all players from the position are removed. The position must exist in the current formation.";
            sanity_check = sanity_check_and_help(args, options, 11, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = remove_player_from_position(args, options);
            break;
        case 7:   /* list */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }

            usage = "list <position_name> [--help]";
            description = "List all players assigned to the specified position in the current formation.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = list_players_of_position(args, options);
            break;
        case 8:   /* listf */
            if (args && args[0] != NULL) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "listf [--help]";
            description = "Displays the names of all formations in the formation list.";
            sanity_check = sanity_check_and_help(NULL, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = list_formations(options);
            break;
        case 9:   /* open */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;  /* No arguments or options provided */
                break;
            }
            usage = "open <formation_name> [--help]";
            description = "Opens the specified formation for editing. The formation must already exist.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);

            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = open_formation(args, options);
            break;
        case 10:  /* show */
            if (args && args[0] != NULL) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "show [--help]";
            description = "Display the current formation in a tactical view.";
            sanity_check = sanity_check_and_help(NULL, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = show(options);
            break;
        case 11:  /* deletef */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "deletef <formation_name> [--help]";
            description = "Removes the specified formation from the formation list. The formation must already exist.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = delete_formation(args, options);
            break;
        default:
            break;
    }
    return ret_val;
}

static int exec_player_command(int cmd_id, char **args, char **options) {
    int ret_val = 0;
    char *usage = NULL;
    char *description = NULL;
    int sanity_check = 0;
    switch (cmd_id) {
        case 0:  /* help */
            ret_val = print_help_page_player();
            break;
        case 1: /* newP */
            if (!args && !options) { // reduce redundancy in code by checking for null pointers at the beginning of the function
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            if (strlen(args[0]) > 40) {
                ret_val = SP_ERR_WRONG_USAGE;  /* name too large */
                break;
            }
            usage = "newP <name> <age> <overall> <potential> <own> [--help]";
            description = "Creates a new player with the specified attributes and adds them to the global player list. If you wish to add a player with names consisting of multiople words, use underscores instead (e.g., 'Ter_Stegen', 'Christiano_Ronaldo'). All other non-alphabetical characters are treated as valid input. Name can only have 40 characters maximum. Age must be greater than 0, and ratings must be between 0 and 100.";
            sanity_check = sanity_check_and_help(args, options, 5, 5, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = new_player(args, options);
            break;
        case 2: /* openP */
            usage = "openP <player_name> [--help]";
            description = "Opens an existing player for editing. The player must already exist.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = open_player(args, options);
            break;
        case 3: /* list */
            usage = "list [--help]";
            description = "Displays a list of all players that have been created.";
            sanity_check = sanity_check_and_help(args, options, 0, 0, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = list_players();
            break;
        case 4: /* deleteP */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "deleteP <player_name> [--help]";
            description = "Removes the specified player from the global player list and all positions they are assigned to. The player must already exist.";
            sanity_check = sanity_check_and_help(args, options, 1, 1, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = delete_player(args, options);
            break;
        case 5: /* editP */
            if (!args && !options) {
                ret_val = SP_ERR_WRONG_USAGE;
                break;
            }
            usage = "editP <player_name> <attribute> <new_value> [--help]";
            description = "Edits the specified attribute of a player. Attributes can be 'age', 'overall', 'potential', 'own', 'goals', 'assists', 'appearances', 'yellow_cards', or 'red_cards'. The player must already exist.";
            sanity_check = sanity_check_and_help(args, options, 3, 3, 1, 0, usage, description);
            if (sanity_check != 0) {
                if (sanity_check == 1) ret_val = SP_SUCCESS;  /* Help displayed */
                else ret_val = sanity_check;  /* Error */
                break;
            }
            ret_val = edit_player(args, options);
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
static int exec_load_command(int cmd_id, char** args, char** options) {
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
        clear_all_squads();
        return SP_SUCCESS;
    }
    if (!cmd) return SP_ERR_NULL_PTR;

    int ret_val = 0;
    switch (context) {
        case 0:  /* Main menu */
            ret_val = exec_main_command(cmd->id);
            break;
        case 1:
            ret_val = exec_squad_command(cmd->id, cmd->args, cmd->options);
            break;
        case 2:  /* Formation menu */
            ret_val = exec_formation_command(cmd->id, cmd->args, cmd->options);
            break;
        case 3: /* player menu */
            ret_val = exec_player_command(cmd->id, cmd->args, cmd->options);
            break;
        case 4:  /* Saves menu */
            ret_val = exec_load_command(cmd->id, cmd->args, cmd->options);
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

int current_squad_name(char* dest) {
    return get_current_squad_name(dest);
}
