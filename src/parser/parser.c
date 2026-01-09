#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


int parse_command(const char *line, command* cmd){
    if (!cmd) {
        return -1;
    }
    char* buffer = NULL;
    if (!(buffer = strdup(line))) {
        return -1;
    }

    cmd->id = 0;
    cmd->name = NULL;
    cmd->options = NULL;
    cmd->args = NULL;
    int argv0 = 0;
    int options_count = 0;
    int args_count = 0;
    char *token = strtok(buffer, " ");
    while(token != NULL) {
        // Process each token
        if (argv0 == 0) {
            if (token[0] == '-') {
                free(buffer);
                return -2;
            }    
            if (strcmp(token, "add") == 0) {
                cmd->id = 1;
            } else if (strcmp(token, "remove") == 0) {
                cmd->id = 2;
            } else if (strcmp(token, "list") == 0) {
                cmd->id = 3;
            } else {
                free(buffer);
                return -2;
            }
            cmd->name = strdup(token);
            if (!cmd->name) {
                free(buffer);
                return -1;
            }
            argv0++;
        }
            else {
                if (token[0] == '-') {
                    char * temp = strdup(token);
                    if (!temp) {
                        free(buffer);
                        free(cmd->name);
                        return -1;
                    }
                    char **new_opts = realloc(cmd->options, sizeof(char*) * (options_count + 1));
                    if (!new_opts) {
                        free(buffer);
                        free(cmd->name);
                        free(temp);
                        for (int j = 0; j < options_count; j++) free(cmd->options[j]);
                        free(cmd->options);
                        return -1;
                    }
                    cmd->options = new_opts;
                    cmd->options[options_count] = temp;
                    options_count++;
                } 
                else {
                    char * temp = strdup(token);
                    if (!temp) {
                        free(buffer);
                        free(cmd->name);
                        for (int j = 0; j < options_count; j++) free(cmd->options[j]);
                        free(cmd->options);
                        for (int j = 0; j < args_count; j++) free(cmd->args[j]);
                        free(cmd->args);
                        return -1;
                    }
                    char **new_args = realloc(cmd->args, sizeof(char*) * (args_count + 1));
                    if (!new_args) {
                        free(buffer);
                        free(temp);
                        free(cmd->name);
                        for (int j = 0; j < options_count; j++) free(cmd->options[j]);
                        free(cmd->options);
                        for (int j = 0; j < args_count; j++) free(cmd->args[j]);
                        free(cmd->args);
                        return -1;
                    }
                    cmd->args = new_args;
                    cmd->args[args_count] = temp;
                    args_count++;
                }
            }
        token = strtok(NULL, " ");
    }
    if (errno !=0 && errno != EINVAL) {
        free(buffer);
        free(cmd->name);
        for (int j = 0; j < options_count; j++) free(cmd->options[j]);
        free(cmd->options);
        for (int j = 0; j < args_count; j++) free(cmd->args[j]);
        free(cmd->args);
        return -1;
    }
    free(buffer);
    return 0;
}