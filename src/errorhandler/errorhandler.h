/**
 * @file errorhandler.h
 * @brief Header file for error handling functions.
 * 
 * Error Code Convention:
 * - -1: General/system error
 * - -2: Invalid arguments or input
 * - -3: Resource not found
 */

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include "../command.h"

/**
 * @brief Generic error handler dispatcher.
 * @param cmd The command that caused the error
 * @param error_code The error code to handle
 * @param context Current application context
 * @return The processed error code
 */
int error_handler(command* cmd, int error_code, int context);

/**
 * @brief Error handler for main menu commands.
 * @param cmd The command that caused the error
 * @param error_code The error code to handle
 * @return The processed error code
 */
int error_handler_main(command* cmd, int error_code);

#endif /* ERRORHANDLER_H */