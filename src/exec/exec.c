#include "../command.h"
#include "help_commands.h"
#include <stdio.h>

int exec_main_command(int cmd_id) {
    // Dummy implementation
    // In a real scenario, this would execute the main menu command based on cmd_id
    switch (cmd_id)
    {
    case 0: // help
        break; // context change, exec_help_command will be executed
    case 1: // new
        printf("Executing 'new' command in main menu.\n");
        break;
    case 2: // load
        printf("Executing 'load' command in main menu.\n");
        break;
    default:
        break;
    }
    return 0; // Return 0 on success
}

int exec_help_command(int cmd_id) {
    // Dummy implementation
    // In a real scenario, this would execute the help menu command based on cmd_id
    switch (cmd_id)
    {
    case 0:
        return print_help_page();
        break;
    case 1:
        // back command
        break;
    default:
        break;
    }
    return 0; // Return 0 on success
}

int exec_formation_command(int cmd_id, char** options, char** args) {
    // Dummy implementation
    // In a real scenario, this would execute the formation command based on cmd_id, options, and args
    printf("Executing formation command ID %d with options and args.\n", cmd_id);
    return 0; // Return 0 on success
}

int exec_load_command(int cmd_id, char** args) {
    // Dummy implementation
    // In a real scenario, this would execute the load command based on cmd_id and args
    printf("Executing load command ID %d with args.\n", cmd_id);
    return 0; // Return 0 on success
}


int execute_command(command* cmd, int context) {
    // Dummy implementation of command execution
    // In a real scenario, this would call the appropriate function based on cmd->id and context
    printf("Executing command ID %d in context %d\n", cmd->id, context);
    int ret_val = 0;
    switch (context) {
    case 0:
        /* code */
        ret_val = exec_main_command(cmd->id);
        break;
    case 1:
        ret_val = exec_help_command(cmd->id);
        break;
    case 2:
        ret_val = exec_formation_command(cmd->id, cmd->options, cmd->args);
        break;
    case 3:
        ret_val = exec_load_command(cmd->id, cmd->args);
        break;
    default:
        break;
    }
    if (ret_val < 0) {
        return -1;
    }

    return ret_val; // Return 0 on success
}