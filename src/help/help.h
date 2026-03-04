/**
 * @file help.h
 * @brief Header file for help system functions.
 */

#ifndef HELP_H
#define HELP_H

/**
 * @brief Display the main menu help page.
 * @return 0 on success, -1 on error
 */
int print_help_page_main(void);

int print_help_page_squad(void);

/**
 * @brief Display the formation menu help page.
 * @return 0 on success, -1 on error
 */
int print_help_page_formation(void);

/**
 * @brief Display the saves menu help page.
 * @return 0 on success, -1 on error
 */
int print_help_page_saves(void);

#endif /* HELP_H */
