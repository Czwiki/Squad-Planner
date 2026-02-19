#include "src/parser/parser.h"
#include "src/exec/exec.h"
#include "src/error/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

# define MAX_INPUT_SIZE 200

static void die(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int clean_command (command* cmd) {
    if (!cmd) return 0;
    if (cmd->name) {
        free(cmd->name);
        cmd->name = NULL;
    }
    int i = 0;
    if (cmd->options) {
        while (cmd->options && cmd->options[i]) {
                free(cmd->options[i]);
                i++;
            }
        free(cmd->options);
        cmd->options = NULL;
    }
    if (cmd->args) {
        i = 0;
        while (cmd->args && cmd->args[i]) {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
        cmd->args = NULL;
    }
    cmd->id = 0;
    cmd->future_context = 0;
    return 0;
}

int main(int argc, char const* argv[])
{
    // command id -1 == exit
    int context = 0; // 0 == main menu, 1 == in formation planner, 2 == load menu

    char prompt[70] = "squad-planner - main> ";
    command* cmd = malloc(sizeof(command));
    if (!cmd) {
        die("Memory allocation error: ");
    }

    printf("%s", prompt);

    char line[MAX_INPUT_SIZE] = {0};
    while ((fgets(line, MAX_INPUT_SIZE, stdin)) != NULL) {        
        if (line[0] == '\n') {
            printf("%s", prompt);
            continue;
        }
        if (strlen(line) >= MAX_INPUT_SIZE - 1 && line[MAX_INPUT_SIZE - 2] != '\n') {
            fprintf(stderr, "Input line too long, maximum size is %d characters\n", MAX_INPUT_SIZE - 1);
            // Clear the remaining input
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("%s", prompt);
            continue;
        }
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (strcmp(line, "exit") == 0 ) {
            clean_command(cmd);
            free(cmd);
            execute_command(NULL, 99); // cleanup command to free formations and players
            exit(EXIT_SUCCESS);
        }

        int executing = 1;
        
        switch (parse_command(line, context, cmd)) { // all cases mutually exclusive
            case SP_ERR_MEMORY:
                die("Error parsing command: ");
                break;
            case SP_ERR_INTERNAL:
                die(sp_error_string(SP_ERR_INTERNAL));
                break;
            case SP_ERR_INVALID_CMD:
                fprintf(stderr, "%s\n", sp_error_string(SP_ERR_INVALID_CMD));
                executing = 0;
                break;
            default: // just for the top 4 cases to change context
                break;
        }

        if (cmd->future_context != context) {
            switch (cmd->future_context) {
                case 0:
                    if (snprintf(prompt, 70, "squad-planner - main> ") < 0) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 1:
                    if (snprintf(prompt, 70, "squad-planner - formation> ") < 0) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 2:
                    if (snprintf(prompt, 70, "squad-planner - saves> ") < 0) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                default:
                    break;
            }
            executing = 0;
        }

        if (executing){      
            printf("Command ID: %d, Name: %s\n", cmd->id, cmd->name);
            int ret_val = execute_command(cmd, context);
            printf("%s\n", sp_error_string(ret_val));
            if (ret_val == SP_SUCCESS) {
                if (context == 1) { // formation command 'new' or 'open'
                    char in[40] = {0};
                    int result = current_formation_name(in);
                    if (result == SP_SUCCESS) {
                        if (snprintf(prompt, 70, "squad-planner - '%s' formation> ", in) < 0) {
                            die("Error setting prompt: ");
                        }
                    } 
                    else {
                        die("Error getting current formation name: ");
                    }
                }
            }
            printf("%s", prompt);
            clean_command(cmd);
        }
    }
    
    if (ferror(stdin)) {
        die("Error reading line: ");
    }

    clean_command(cmd);
    free(cmd);
    execute_command(NULL, 99); // cleanup command to free formations and players
    return 0;
}

