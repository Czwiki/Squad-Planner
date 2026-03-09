#include "stdlib.h"
#include "../error/error.h"



typedef struct Squad Squad;

static Squad* current_squad = NULL;  /**< Pointer to the currently active squad being edited */


int setting_squad(Squad* squad) {
    current_squad = squad;
    return SP_SUCCESS;
}


int list_players();