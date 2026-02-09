#include "src/parser/parser.h"
#include "src/exec/exec.h"
#include "src/error/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

# define MAX_INPUT_SIZE 200

int clean_command (command* cmd) {
    if (cmd->name) free(cmd->name);
    else return 0;
    int i = 0;
    if (cmd->options) {
    while (cmd->options && cmd->options[i]) {
            free(cmd->options[i]);
            i++;
        }
    free(cmd->options);
    }
    else return 0;
    if (cmd->args) {
    i = 0;
    while (cmd->args && cmd->args[i]) {
        free(cmd->args[i]);
        i++;
    }
    free(cmd->args);
    }
    return 0;
}

int main(int argc, char const* argv[])
{
    // command id -1 == exit
    int context = 0; // 0 == main menu, 1 == in formation planner, 2 == load menu

    char prompt[60] = "squad-planner - main> ";
    command* cmd = malloc(sizeof(command));
    if (!cmd) {
        perror("Memory allocation error: ");
        exit(EXIT_FAILURE);
    }

    printf("%s", prompt);

    char line[MAX_INPUT_SIZE] = {0};
    while ((fgets(line, MAX_INPUT_SIZE, stdin)) != NULL) {        
        if (line[0] == '\n') {
            continue;
        }
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        else {
            fprintf(stderr, "Input line too long, maximum size is %d characters\n", MAX_INPUT_SIZE - 1);
            continue;
        }
        if (strcmp(line, "exit") == 0 ) {
            free(cmd);
            exit(EXIT_SUCCESS);
        }

        int executing = 1;
        
        switch (parse_command(line, context, cmd)) { // all cases mutually exclusive
        case SP_ERR_MEMORY:
            clean_command(cmd);
            perror("Error parsing command: ");
            exit(EXIT_FAILURE);
            break;
        case SP_ERR_INTERNAL:
            clean_command(cmd);
            fprintf(stderr, "Internal error occurred while parsing command\n");
            exit(EXIT_FAILURE);
            break;
        case SP_ERR_INVALID_CMD:
            /* code */
            fprintf(stderr, "Invalid command format\n");
            executing = 0;
            break;
        default: // just for the top 4 cases to change context
            break;
        }

        if (cmd->future_context != context) {
            switch (cmd->future_context) {
            case 0:
                if (snprintf(prompt, 60, "squad-planner - main> ") < 0) {
                    perror("Error setting prompt: ");
                    exit(EXIT_FAILURE);
                }
                context = cmd->future_context;
                break;
            case 1:
                if (snprintf(prompt, 60, "squad-planner - formation> ") < 0) {
                    perror("Error setting prompt: ");
                    exit(EXIT_FAILURE);
                }
                context = cmd->future_context;
                break;
            case 2:
                if (snprintf(prompt, 60, "squad-planner - saves> ") < 0) {
                    perror("Error setting prompt: ");
                    exit(EXIT_FAILURE);
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
                if (context == 1 && (cmd->id == 1 || cmd->id == 10) && cmd->args && cmd->args[0]) { // formation command 'new' or 'open'
                    if (snprintf(prompt, 60, "squad-planner - '%s' formation> ", cmd->args[0]) < 0) {
                        perror("Error setting prompt: ");
                        exit(EXIT_FAILURE);
                    }
                    printf("In formation context\n");
                }
            }
        }
        printf("%s", prompt);
        clean_command(cmd);
    }
    
    if (ferror(stdin)) {
        perror("Error reading line: ");
        exit(EXIT_FAILURE);
    }

    free(cmd);
    return 0;
}

