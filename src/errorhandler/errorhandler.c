#include "../command.h"
#include <stdio.h>


int error_handler (command* cmd, int error_code, int context) {
    fprintf(stderr, "Error executing command '%s': ", cmd->name);
    int ret_val = 0;
    switch (context) {
    case 0:
        /* code */
        break;
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    default:
        break;
    }
    return ret_val;
}

int error_handler_main(command* cmd, int error_code) {
    int ret_val = 0;
    switch (error_code) {
    case -1:
        perror("General error: ");
        ret_val = -1;
        break;
    case -2:
        fprintf(stderr, "Invalid arguments provided.\n");
        ret_val = -2;
        break;
    case -3:
        fprintf(stderr, "Resource not found.\n");
        ret_val = -3;
        break;
    default:
        fprintf(stderr, "Unknown error occurred.\n");
        ret_val = -1;
        break;
    }
    return ret_val;
}
