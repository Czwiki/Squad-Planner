#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

static char *command_inputs_main[3] ={"help","new","load"};
static char *command_inputs_help[2] = {"help","back"};
static char *command_inputs_formation[7] = {"help","add","remove","list","save","show", "back"};
static char *command_inputs_saves[1] = {"save"};

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
            command_inputs = command_inputs_help;
            length = 2;
            break;
        case 2:
            command_inputs = command_inputs_formation;;
            length = 7;
            break;
        case 3:
            command_inputs = command_inputs_saves;
            length = 1;
            break;
        default:
            return -2;
    }

    int new_context = current_context;

    cmd->id = 0;
    cmd->context = current_context;
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
                    if (current_context == 0){
                        if (i == 0) {
                            new_context = 1; // help menu
                        }
                        else if (i == 1) {
                            new_context = 2; // formation menu
                        }
                        else if (i == 2) {
                            new_context = 3; // load menu
                        }
                        else {
                            new_context = 0; // stay in main menu
                        }
                    }
                    else if (current_context == 1 && i == 1) {
                        new_context = 0; // back to main menu from help
                    }
                    else if (current_context == 2) {
                        if (i == 6) {
                            new_context = 0; // back to main menu from formation
                        }
                        else if (i == 4) {
                            new_context = 3; // go to load menu from formation on save
                        }
                        else {
                            new_context = 2; // stay in formation menu
                        }
                    }
                    else {
                        new_context = current_context;
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