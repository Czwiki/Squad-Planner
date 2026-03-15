/**
 * @file help.c
 * @brief Help system for the Squad-Planner application.
 * 
 * Provides help pages for each application context, listing
 * available commands and their descriptions.
 */

#include <stdio.h>
#include "../error/error.h"

/**
 * @brief Display the main menu help page.
 * 
 * Shows all commands available in the main menu context.
 * 
 * @return 0 on success, -1 on output error
 */
int print_help_page_main(void) {
    printf("Main menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the main menu:\n");
    printf("1. help  - Display this help page\n");
    printf("2. squad - Enter the squad planning menu\n");
    printf("3. load  - Load saved data from file (transitions to saves menu)\n");
    printf("These commands allow you to navigate to different parts of the application. They do not accept arguments or options.\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}

int print_help_page_squad(void) {
    printf("Squad menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the squad menu:\n");
    printf("1. help      - Display this help page\n");
    printf("2. new       - Create a new squad\n");
    printf("3. list      - List all squads\n");
    printf("4. open      - Open an existing squad\n");
    printf("5. formation - Enter the formation menu\n");
    printf("6. players   - Enter the player menu\n");
    printf("7. back      - Return to main menu\n");
    printf("8. save      - Save current data (transitions to saves menu)\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}

/**
 * @brief Display the formation menu help page.
 * 
 * Shows all commands available in the formation menu context,
 * including formation management and player assignment commands.
 * 
 * @return 0 on success, -1 on output error
 */
int print_help_page_formation(void) {
    printf("Formation menu Help Page\n");
    printf("------------------------\n");
    printf("Available commands in the formation menu:\n");
    printf(" 1. help       - Display this help page\n");
    printf(" 2. new        - Create a new formation\n");
    printf(" 3. preference - Adjust player preferences in a position\n");
    printf(" 4. add        - Add a position to the formation\n");
    printf(" 5. addP       - Add an existing player to a position\n");
    printf(" 6. remove     - Remove a position from the formation\n");
    printf(" 7. removeP    - Remove a player from a position\n");
    printf(" 8. list       - List all players assigned to a position\n");
    printf(" 9. listf      - List all formations\n");
    printf("10. open       - Open a formation for editing\n");
    printf("11. show       - Show the current formation in tactical view\n");
    printf("12. deletef    - Remove an existing formation\n");
    printf("13. save       - Save data (transitions to saves menu)\n");
    printf("14. back       - Return to the squad menu\n");
    printf("To create, edit, or delete players, use the player menu ('players' command in squad menu).\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}

int print_help_page_player(void) {
    printf("Player menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the player menu:\n");
    printf("1. help    - Display this help page\n");
    printf("2. newP    - Create a new player\n");
    printf("3. openP   - Open an existing player\n");
    printf("4. list    - List all players\n");
    printf("5. deleteP - Remove an existing player\n");
    printf("6. editP   - Edit a player's attributes or stats\n");
    printf("             (age, overall, potential, own, goals, assists, appearances, yellow_cards, red_cards)\n");
    printf("7. save    - Save current data (transitions to saves menu)\n");
    printf("8. back    - Return to squad menu\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}

/**
 * @brief Display the saves menu help page.
 *
 * Shows all commands available in the saves/load context.
 *
 * @return 0 on success, -1 on output error
 */
int print_help_page_saves(void) {
    printf("Saves menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the saves menu:\n");
    printf("1. help - Display this help\n");
    printf("2. save [filename] - Save data\n");
    printf("3. load [filename] - Load data\n");
    printf("4. back - Return to main menu\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return SP_ERR_INTERNAL;
    }
    return SP_SUCCESS;
}
