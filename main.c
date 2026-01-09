//#define _POSIX_C_SOURCE 200809L
#include "src/parser/parser.h"
# include <stdio.h>
# include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int main(int argc, char const *argv[])
{
    /* code */
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1)
    {
        if (line[0] == '\n') {
            continue;
        }
        int ret_val = 0;
        command *cmd = parse_command(line, &ret_val);
        printf("Return value: %d\n", ret_val);
        switch (ret_val)
        {
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
            free(cmd->name);
            for (int i = 0; i < cmd->id; i++) {
                free(cmd->options[i]);
            }
            free(cmd->options);
            for (int i = 0; i < cmd->id; i++) {
                free(cmd->args[i]);
            }
            free(cmd->args);
            free(cmd);
        }

    }
    free(line);
    
    return 0;
}
