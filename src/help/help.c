#include <stdio.h>

int print_help_page_main(void) {
    printf("Main menu Help Page\n");
    printf("-------------------\n");
    printf("Available commands in the main menu:\n");
    printf("1. help - Display this help page\n");
    printf("2. new  - Create a new squad formation\n");
    printf("3. load - Load an existing squad formation\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return -1;
    }
    return 0;
}

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
    printf("13. save      - Save the current formation\n");
    printf("14. back      - Return to the main menu\n");
    printf("For help on explicit commands, type '<command> --help'\n");
    if (fflush(stdout) != 0) {
        perror("Error flushing stdout: ");
        return -1;
    }
    return 0;
}
