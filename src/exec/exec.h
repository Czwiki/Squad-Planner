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

/** @brief Sentinel context value used to trigger resource cleanup on exit. */
#define SP_CONTEXT_CLEANUP 99

/**
 * @brief Execute a parsed command in the given context.
 * 
 * @param cmd Pointer to the parsed command structure (may be NULL for cleanup)
 * @param context Current application context (0=main, 1=formation, 2=saves,
 *                SP_CONTEXT_CLEANUP=free all resources)
 * @return 0 on success, negative on error
 */
int execute_command(command* cmd, int context);
int current_formation_name(char* dest);
int current_squad_name(char* dest);

#endif /* EXEC_H */
