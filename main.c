#include "src/parser/parser.h"
#include "src/exec/exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

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

int main(int argc, char const *argv[])
{
    // command id -1 == exit
    int context = 0; // 0 == main menu, 1 == in squad planner, 2 == help menu, 3 == load menu

    char *prompt = "squad-planner> ";
    command *cmd = malloc(sizeof(command));
    if (!cmd) {
        perror("Memory allocation error: ");
        exit(EXIT_FAILURE);
    }

    printf("%s", prompt);

    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    while ((read = getline(&line, &len, stdin)) != -1) {        
        if (line[0] == '\n') {
            continue;
        }
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "exit") == 0 ) {
            free(line);
            free(cmd);
            exit(EXIT_SUCCESS);
        }

        int executing = 1;
        int ret_val = parse_command(line, context, cmd);
        switch (ret_val) {
        case -1:
            /* code */
            perror("Error parsing command: ");
            exit(EXIT_FAILURE);
            break;
        case -2:
            /* code */
            fprintf(stderr, "Invalid command format\n");
            executing = 0;
            break;
        case -3:
            fprintf(stderr, "Unknown command in this environment\n");
            executing = 0;
            break;
        default:
            break;
        }
        if (ret_val >= 0) {
            if (ret_val == 0) {
                prompt = "squad-planner> ";
            }
            else if (ret_val == 1) {
                prompt = "formation> ";
            }
            else if (ret_val ==2) {
                prompt = "saves> ";
            }
            context = ret_val;
        }

        if (executing){      
            printf("Command ID: %d, Name: %s\n", cmd->id, cmd->name);  
            if (execute_command(cmd, context) < 0) {
            fprintf(stderr, "Error executing command\n");
            }
        }

        printf("%s", prompt);
        clean_command(cmd);
    }
    
    if (ferror(stdin)) {
        perror("Error reading line: ");
        exit(EXIT_FAILURE);
    }

    free(line);
    free(cmd);
    return 0;
}
