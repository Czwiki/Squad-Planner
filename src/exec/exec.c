#include "../command.h"
#include "../help/help.h"
#include "../formation/formation.h"
#include <stdio.h>

int exec_main_command(int cmd_id) {
    // Dummy implementation
    // In a real scenario, this would execute the main menu command based on cmd_id
    int ret_val = 0;
    switch (cmd_id)
    {
    case 0: // help
        ret_val = print_help_page();
        break;
    case 1: // new
        printf("Executing 'new' command in main menu.\n");
        //new_formation("Default Formation");
        break;
    case 2: // load
        printf("Executing 'load' command in main menu.\n");
        break;
    default:
        break;
    }
    return ret_val; // Return 0 on success
}

int exec_formation_command(int cmd_id, char** options, char** args) {
    // Dummy implementation
    // In a real scenario, this would execute the formation command based on cmd_id, options, and args
    int ret_val = 0;
    switch (cmd_id) {
    case 0: // help
        ret_val = print_help_page();
        break;
    case 1: // new
        if (options) {
            return -2;
        }
        ret_val = new_formation(args[0]);
        break;
    case 2: // newP
        ret_val = new_player(args);
        break;
    case 3: // preference
        ret_val = preferences(options, args);
        break;
    case 4: // add
        ret_val = add_position_to_formation(args);
        break;
    case 5: // addP
        ret_val = add_player_to_position(args);
        break;
    case 6: // remove
        ret_val = remove_position_from_formation(args);
        break;
    case 7: // removeP
        ret_val = remove_player_from_position(args);
        break;
    case 8: // list
        ret_val = list_players_of_position(args);
        break;
    case 9: // listf
        ret_val = list_formations();
        break;
    case 10: // open
        ret_val = open_formation(args[0]);
        break;
    case 11: // show
        ret_val = show();
        break;
    case 12: // save
        break;
    default:
        break;
    }
    printf("%i\n", ret_val);
    return ret_val; // Return 0 on success
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
    if (!cmd) {
        return -1;
    }
    if (cmd->id == 0) {
        return print_help_page();
    }
    int ret_val = 0;
    switch (context) {
    case 0:
        /* code */
        ret_val = exec_main_command(cmd->id);
        break;
    case 1:
        ret_val = exec_formation_command(cmd->id, cmd->options, cmd->args);
        break;
    case 2:
        ret_val = exec_load_command(cmd->id, cmd->args);
        break;
    default:
        break;
    }
    return ret_val; // Return 0 on success
}
