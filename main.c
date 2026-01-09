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
    int context = 0; // 0 == main menu, 1 == in squad planner, 2 == help menu, 3 == load menu
    int context_0_ids[4] ={0,1,2,-1}; // Example command IDs valid in context 0
    int context_1_ids[4] = {3,4,5,-1}; // Example command IDs valid in context 1
    int context_2_ids[4] = {6,7,8,-1}; // Example command IDs valid in context 2
    int context_3_ids[4] = {9,10,11,-1}; // Example command IDs valid in context 3
    char *prompt = "squad-planner> ";
    command *cmd = malloc(sizeof(command));
    if (!cmd) {
        perror("Memory allocation error: ");
        exit(EXIT_FAILURE);
    }

    printf("%s", prompt);

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
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

        int ret_val = parse_command(line, cmd);
        switch (ret_val) {
        case -1:
            /* code */
            perror("Error parsing command: ");
            exit(EXIT_FAILURE);
            break;
        case -2:
            /* code */
            fprintf(stderr, "Invalid command format\n");
            break;
        default:
            break;
        }
        if (cmd){
            printf("Command ID: %d, Name: %s\n", cmd->id, cmd->name);
            int *context_ids = NULL;
            switch (context)
            {
            case 0:
                context_ids = context_0_ids;
                break;
            case 1:
                context_ids = context_1_ids;
                break;
            case 2:
                context_ids = context_2_ids;
                break;
            case 3:
                context_ids = context_3_ids;
                break;
            default:
                break;
            }
            int correct_context = 0;
            for (int i = 0; context_ids && context_ids[i] != -1; i++) {
                if (context_ids[i] == cmd->id) {
                    printf("Executing command '%s' in context %d\n", cmd->name, context);
                    correct_context = 1;
                    // Here you would call the function associated with the command
                }
            }
            if (!correct_context) {
                printf("Command '%s' not valid in context %d\n", cmd->name, context);
            }
            else {
                if (execute_command(cmd, context) < 0) {
                    fprintf(stderr, "Error executing command\n");
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

    free(line);
    free(cmd);
    return 0;
}
