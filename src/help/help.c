/**
 * @file help.c
 * @brief Help system for the Squad-Planner application.
 * 
 * Provides help pages for each application context, listing
 * available commands and their descriptions.
 */

#include <stdio.h>

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
    printf("1. help      - Display this help page\n");
    printf("2. squad     - Enter the squad planning menu\n");
    printf("3. load      - Load saved data from file (not yet implemented)\n");
    printf("These commands allow you to navigate to different parts of the application. They do not accept arguments or options.\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return -1;
    }
    return 0;
}

int print_help_page_squad(void) {
    printf("Squad menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the squad menu:\n");
    printf("1. help - Display this help page\n");
    printf("2. new  - Create a new squad (not yet implemented)\n");
    printf("3. list - List all squads (not yet implemented)\n");
    printf("4. open - Open an existing squad (not yet implemented)\n");
    printf("5. formation - Enter the formation menu\n");
    printf("6. players - Enter the player menu (not yet implemented)\n");
    printf("7. back - Return to main menu\n");
    printf("8. save - Save current squad data\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return -1;
    }
    return 0;
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
    printf("1. help       - Display this help page\n");
    printf("2. new        - Create a new formation\n");
    printf("3. newP       - Create a new player\n");
    printf("4. preference - Adjust player preferences in a position\n");
    printf("5. add        - Add a position to the formation\n");
    printf("6. addP       - Add a player to a position\n");
    printf("7. remove     - Remove a position from the formation\n");
    printf("8. removeP    - Remove a player from a position\n");
    printf("9. list       - List all players assigned to a position\n");
    printf("10. listf     - List all formations\n");
    printf("11. open      - Open a formation file\n");
    printf("12. show      - Show the current formation in tactical view\n");
    printf("13. save      - Save data\n");
    printf("14. back      - Return to the main menu\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return -1;
    }
    return 0;
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
        return -1;
    }
    return 0;
}
