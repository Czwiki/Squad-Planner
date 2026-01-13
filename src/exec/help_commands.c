#include <stdio.h>

int print_help_page() {
    printf("Squad Planner Help Page\n");
    printf("-----------------------\n");
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