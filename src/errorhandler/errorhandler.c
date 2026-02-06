/**
 * @file errorhandler.c
 * @brief Error handling module for the Squad-Planner application.
 * 
 * Provides centralized error handling and error message display
 * functions. Error codes follow a consistent pattern:
 * 
 * Error Code Convention:
 * - -1: General/system error (memory allocation, I/O, etc.)
 * - -2: Invalid arguments or input validation failure
 * - -3: Resource not found (formation, player, position)
 * 
 * Future improvements:
 * - Add logging capability
 * - Add context-specific error messages per command
 * - Add error recovery suggestions
 */

#include "../command.h"
#include <stdio.h>

/**
 * @brief Generic error handler dispatcher.
 * 
 * Routes error handling to context-specific handlers.
 * Currently a placeholder for future expansion.
 * 
 * @param cmd The command that caused the error
 * @param error_code The error code returned by the command
 * @param context Current application context
 * @return The processed error code
 */
int error_handler(command* cmd, int error_code, int context) {
    fprintf(stderr, "Error executing command '%s': ", cmd->name);
    int ret_val = 0;
    switch (context) {
    case 0:  /* Main menu context */
        /* TODO: Add main menu specific error handling */
        break;
    case 1:  /* Formation menu context */
        /* TODO: Add formation-specific error handling */
        break;
    case 2:  /* Saves menu context */
        /* TODO: Add save/load specific error handling */
        break;
    default:
        break;
    }
    return ret_val;
}

/**
 * @brief Error handler for main menu commands.
 * 
 * Displays user-friendly error messages for main menu commands
 * based on the error code.
 * 
 * @param cmd The command that caused the error
 * @param error_code The error code to handle
 * @return The processed error code
 */
int error_handler_main(command* cmd, int error_code) {
    int ret_val = 0;
    switch (error_code) {
    case -1:
        perror("General error: ");
        ret_val = -1;
        break;
    case -2:
        fprintf(stderr, "Invalid arguments provided.\n");
        ret_val = -2;
        break;
    case -3:
        fprintf(stderr, "Resource not found.\n");
        ret_val = -3;
        break;
    default:
        fprintf(stderr, "Unknown error occurred.\n");
        ret_val = -1;
        break;
    }
    return ret_val;
}
