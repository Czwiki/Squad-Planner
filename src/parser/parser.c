#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


command* parse_command(const char *line, int* ret_val){
    char* buffer = NULL;
    if (!(buffer = strdup(line))) {
        if (ret_val) *ret_val = -1;
        return NULL;
    }
    command *cmd = malloc(sizeof(command));
    if (!cmd) {
        free(buffer);
        if (ret_val) *ret_val = -1;
        return NULL;
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
        printf("Token: %s\n", token); // Debugging line
        if (argv0 == 0) {
            if (token[0] == '-') {
                free(buffer);
                free(cmd);
                if (ret_val) *ret_val = -2;
                return NULL;
            }    
            if (strcmp(token, "add") == 0) {
                cmd->id = 1;
            } else if (strcmp(token, "remove") == 0) {
                cmd->id = 2;
            } else if (strcmp(token, "list") == 0) {
                cmd->id = 3;
            } else {
                free(buffer);
                free(cmd);
                if (ret_val) *ret_val = -2;
                return NULL;
            }
            cmd->name = strdup(token);
            if (!cmd->name) {
                free(buffer);
                free(cmd);
                if (ret_val) *ret_val = -1;
                return NULL;
            }
            argv0++;
            continue;
        }
        else {
            if (token[0] == '-') {
                // It's an option
                // For simplicity, we skip storing options in this example
                char * temp = strdup(token);
                if (!temp) {
                    free(buffer);
                    free(cmd->name);
                    free(cmd);
                    if (ret_val) *ret_val = -1;
                    return NULL;
                }
                options_count++;
                cmd->options = realloc(cmd->options, sizeof(char*) * (options_count));
                if (!cmd->options) {
                    free(buffer);
                    free(cmd->name);
                    free(temp);
                    free(cmd);
                    if (ret_val) *ret_val = -1;
                    return NULL;
                }
                cmd->options[options_count - 1] = temp;
            } 
            else {
                // It's an argument
                // For simplicity, we skip storing arguments in this example
                char * temp = strdup(token);
                if (!temp) {
                    free(buffer);
                    free(cmd->name);
                    for (int j = 0; j < options_count; j++) {
                        free(cmd->options[j]);
                    }
                    free(cmd->options);
                    free(cmd);
                    if (ret_val) *ret_val = -1;
                    return NULL;
                }
                args_count++;
                cmd->args = realloc(cmd->args, sizeof(char*) * (args_count));
                if (!cmd->args) {
                    free(buffer);
                    free(cmd->name);
                    for (int j = 0; j < options_count; j++) {
                        free(cmd->options[j]);
                    }
                    free(cmd->options);
                    free(temp);
                    free(cmd);
                    if (ret_val) *ret_val = -1;
                    return NULL;
                }
                cmd->args[args_count - 1] = temp;
            }
        }
        token = strtok(NULL, " ");
    }
    if (errno !=0 && errno != EINVAL) {
        free(buffer);
        free(cmd->name);
        for (int j = 0; j < options_count; j++) {
            free(cmd->options[j]);
        }
        free(cmd->options);
        for (int j = 0; j < args_count; j++) {
            free(cmd->args[j]);
        }
        free(cmd->args);
        free(cmd);
        if (ret_val) *ret_val = -1;
        return NULL;
    }
    free(buffer);
    if (ret_val) *ret_val = 0;
    return cmd;
}