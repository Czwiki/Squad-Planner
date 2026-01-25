#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static char *command_inputs_main[3] ={"help","formation","load"};
static char *command_inputs_formation[12] = {"help","new", "newP","set","add", "addP","remove", "removeP","list","save","show","back"};
static char *command_inputs_saves[3] = {"help","save", "back"};

int parse_command(const char *line, int current_context, command* cmd){
    if (!cmd) {
        return -1;
    }
    char* buffer = NULL;
    if (!(buffer = strdup(line))) {
        return -1;
    }
    char ** command_inputs;
    int length = 0;
    switch (current_context) {
        case 0:
            command_inputs = command_inputs_main;
            length = 3;
            break;
        case 1:
            command_inputs = command_inputs_formation;
            length = 12;
            break;
        case 2:
            command_inputs = command_inputs_saves;
            length = 3;
            break;
        default:
            return -2;
    }

    int new_context = current_context;

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
            int found = 0;
            for (int i = 0; i < length; i++) {
                if (strcmp(token, command_inputs[i]) == 0) {
                    cmd->id = i;
                    switch (current_context) {
                    case 0:
                        if (i == 1) {
                            new_context = 1; // go to formation context from main menu on formation
                        }
                        else if (i == 2) {
                            new_context = 2; // go to load context from main menu on load
                        }
                        break;
                    case 1:
                        if (i == 11) {
                            new_context = 0; // back to main menu from formation
                        }
                        else if (i == 9) {
                            new_context = 2; // go to load menu from formation
                        }
                        break;
                    case 2:
                        if (i == 2) {
                            new_context = 0; // back to main menu from load menu
                        }
                        break;
                    default:
                        break;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found){
                free(buffer);
                return -3;
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
                    free(temp);
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
                    return -1;
                }
                char **new_args = realloc(cmd->args, sizeof(char*) * (args_count + 1));
                if (!new_args) {
                    free(buffer);
                    free(temp);
                    return -1;
                }
                cmd->args = new_args;
                cmd->args[args_count] = temp;
                args_count++;
            }
        }
        token = strtok(NULL, " ");
    }
    if (cmd->id == 1 && current_context == 1) {
        // load command in formation context requires exactly one argument
        if (args_count != 1) {
            free(buffer);
            return -2;
        }
        if (strlen(cmd->args[0]) > 32) {
            free(buffer);
            return -2;
        }
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
    cmd->future_context = new_context;
    return 0;
}