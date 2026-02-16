/**
 * @file command.h
 * @brief Defines the command data structure used throughout the application.
 * 
 * A command represents a parsed user input, containing the command name,
 * any options (flags starting with '-'), and arguments.
 */

#ifndef COMMAND_H
#define COMMAND_H

/**
 * @brief Represents a parsed command from user input.
 * 
 * - The command's ID (index in the context's command array)
 * - The raw command name string
 * - Any options (NULL-terminated array of strings starting with '-')
 * - Any arguments (NULL-terminated array of other strings)
 * - The context the command should transition to
 */
typedef struct command {
    int id;               /**< Command ID (index in command list for context) */
    int future_context;   /**< Context to transition to after this command */
    char* name;           /**< Command name as entered by user */
    char** options;       /**< NULL-terminated array of options (start with '-') */
    char** args;          /**< NULL-terminated array of arguments */
} command;
#endif /* COMMAND_H */
