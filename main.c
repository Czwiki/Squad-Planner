#include "src/parser/parser.h"
#include "src/exec/exec.h"
#include "src/error/error.h"
#include "src/compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    int context = 0; // 0 == main menu, 1 == squad planner, 2 == in formation, 3 == player menu, 4 == load menu

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
            if (fflush(stdout) != 0) {
                die("Error flushing stdout: ");
            }
            continue;
        }
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (strcmp(line, "exit") == 0 ) {
            fprintf(stderr, "Did you save everything? If not, press 'c' and Enter to abort and save before exiting. Otherwise, press 'q' and Enter to exit without saving.\n");
            char confirm;
            while (1) {
                int size = scanf("%c", &confirm);
                if (size == EOF) {
                    die("Error reading input: ");
                }
                while (getchar() != '\n');  // Clear the input buffer
                if (confirm == 'q' || confirm == 'c') {
                    break;
                }
                fprintf(stderr, "Invalid input. Press 'c' to abort and save before exiting, or press 'q' to exit without saving.\n");
            }
            if (confirm == 'q') {
                clean_command(cmd);
                free(cmd);
                execute_command(NULL, SP_CONTEXT_CLEANUP);
                exit(EXIT_SUCCESS);
            }
            printf("%s", prompt);
            if (fflush(stdout) != 0) {
                die("Error flushing stdout: ");
            }
            continue;
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
            int ret_val = 0;
            switch (cmd->future_context) {
                case 0:
                    ret_val = snprintf(prompt, 70, "squad-planner - main> ");
                    if (ret_val < 0 || ret_val >= 70) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 1:
                    ret_val = snprintf(prompt, 70, "squad-planner - squad> ");
                    if (ret_val < 0 || ret_val >= 70) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 2:
                    ret_val = snprintf(prompt, 70, "squad-planner - formation> ");
                    if (ret_val < 0 || ret_val >= 70) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 3:
                    ret_val = snprintf(prompt, 70, "squad-planner - player> ");
                    if (ret_val < 0 || ret_val >= 70) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 4:
                    ret_val = snprintf(prompt, 70, "squad-planner - saves> ");
                    if (ret_val < 0 || ret_val >= 70) {
                        die("Error setting prompt: ");
                    }
                    context = cmd->future_context;
                    break;
                case 5:
                    ret_val = snprintf(prompt, 70, "squad-planner - player> ");
                    if (ret_val < 0 || ret_val >= 70) {
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
            int ret_val = execute_command(cmd, context);
            printf("%s\n", sp_error_string(ret_val));
            if (ret_val == SP_SUCCESS) {
                if (context == 1) {
                    // further checks required to allow context switch, therefore here and not in parser
                    if (cmd->id == 4) { // entering formation menu from squad menu
                        context = 2;
                    }
                    else if (cmd->id == 5) { // entering player menu from squad menu
                        context = 3;
                    }
                    char in[40] = {0};
                    int result = current_squad_name(in);
                    if (result == SP_SUCCESS) {
                        ret_val = snprintf(prompt, 70, "squad-planner - '%s' squad> ", in);
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    } 
                    else if (result == SP_ERR_NO_SQUAD) {
                        ret_val = snprintf(prompt, 70, "squad-planner - squad> ");
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    }
                    else {
                        die("Error getting current squad name: ");
                    }
                }
                if (context == 2) { // formation menu dynamic prompt
                    char in[40] = {0};
                    int result = current_formation_name(in);
                    if (result == SP_SUCCESS) {
                        ret_val = snprintf(prompt, 70, "squad-planner - '%s' formation> ", in);
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    } 
                    else if (result == SP_ERR_NO_FORMATION) {
                        ret_val = snprintf(prompt, 70, "squad-planner - formation> ");
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    }
                    else {
                        die("Error getting current formation name: ");
                    }
                }
                if (context == 3) { // openP in player menu: enter player editing sub-menu
                    if (cmd->id == 2) { // openP succeeded – switch to player editing sub-menu
                        context = 5;
                    }
                }
                if (context == 5) { // player editing sub-menu dynamic prompt
                    char in[40] = {0};
                    int result = current_player_name(in);
                    if (result == SP_SUCCESS) {
                        ret_val = snprintf(prompt, 70, "squad-planner - '%s' player> ", in);
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    }
                    else if (result == SP_ERR_PLAYER_NOT_FOUND) {
                        ret_val = snprintf(prompt, 70, "squad-planner - player> ");
                        if (ret_val < 0 || ret_val >= 70) {
                            die("Error setting prompt: ");
                        }
                    }
                    else {
                        die("Error getting current player name: ");
                    }
                }
            }
        }
        printf("%s", prompt);
        if (fflush(stdout) != 0) {
            die("Error flushing stdout: ");
        }
        clean_command(cmd);
    }
    
    if (ferror(stdin)) {
        die("Error reading line: ");
    }

    if (fflush(stdout) != 0) {
        die("Error flushing stdout: ");
    }

    clean_command(cmd);
    free(cmd);
    execute_command(NULL, SP_CONTEXT_CLEANUP);
    return 0;
}

