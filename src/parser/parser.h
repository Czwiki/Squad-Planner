/**
 * @file parser.h
 * @brief Header file for the command line parser module.
 * 
 * Provides the interface for parsing user input into structured
 * command objects that can be executed by the application.
 */

#ifndef PARSER_H
#define PARSER_H

#include "../command.h"

/**
 * @brief Parse a command line string into a structured command object.
 * 
 * Tokenizes the input line based on spaces, validates the command
 * against the current context, and extracts options and arguments.
 * 
 * @param line The input string to parse
 * @param current_context Current menu context (0=main, 1=formation, 2=saves)
 * @param cmd Pointer to command structure to populate
 * 
 * @return 0 on success, negative value on error
 *         -1: Memory allocation failure or null cmd pointer
 *         -2: Invalid command format
 *         -3: Command not recognized in current context
 */
int parse_command(const char* line, int current_context, command* cmd);

#endif /* PARSER_H */
