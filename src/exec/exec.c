#include "../parser/parser.h"
#include "exec.h"
#include <stdio.h>

int execute_command(command* cmd, int context) {
    // Dummy implementation of command execution
    // In a real scenario, this would call the appropriate function based on cmd->id and context
    printf("Executing command ID %d in context %d\n", cmd->id, context);
    return 0; // Return 0 on success
}