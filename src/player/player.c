#include <stdlib.h>
#include "player.h"
#include "../error/error.h"

static Squad* current_squad = NULL;  /**< Pointer to the currently active squad being edited */

int setting_squad_player(Squad* squad) {
    current_squad = squad;
    return SP_SUCCESS;
}

//int list_players();