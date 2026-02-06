/**
 * @file exec.h
 * @brief Header file for command execution functions.
 * 
 * Declares the main execute_command() function that dispatches
 * parsed commands to their appropriate handlers.
 */

#ifndef EXEC_H
#define EXEC_H

#include "../command.h"

/**
 * @brief Execute a parsed command in the given context.
 * 
 * @param cmd Pointer to the parsed command structure
 * @param context Current application context (0=main, 1=formation, 2=saves)
 * @return 0 on success, negative on error
 */
int execute_command(command* cmd, int context);

#endif /* EXEC_H */
