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
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        int ret_val = 0;
        command *cmd = parse_command(line, &ret_val);
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
            int i = 0;
            while (cmd->options && cmd->options[i]) {
                    free(cmd->options[i]);
                    i++;
             }
            if (cmd->options) free(cmd->options);
            int j = 0;
            while (cmd->args && cmd->args[j]) {
                free(cmd->args[j]);
                j++;
            }
            if (cmd->args) free(cmd->args);
            free(cmd);
        }

    }
    free(line);
    
    return 0;
}
