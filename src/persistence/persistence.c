/**
 * @file persistence.c
 * @brief Persistence module for the Squad-Planner application.
 *
 * Implements save/load functionality using JSON files via the cJSON library.
 * All players and formations (including position assignments and player
 * preference orders) are serialized to a single JSON file.
 *
 * JSON structure:
 * {
 *   "players": [
 *     { "name": "...", "age": N, "overall": N, "potential": N, "own_rating": N }
 *   ],
 *   "formations": [
 *     {
 *       "name": "...",
 *       "positions": [
 *         { "slot": N, "name": "GK", "players": ["name1", "name2"] }
 *       ]
 *     }
 *   ]
 * }
 *
 * Integration:
 * - save_to_file() is called from exec.c when the user issues the 'save' command.
 * - load_from_file() is called from exec.c when the user issues the 'load' command.
 * - Both functions use the accessor/iterator API from formation.h to avoid
 *   direct access to static internal data in formation.c.
 */

#include "persistence.h"
#include "../formation/formation.h"
#include "../error/error.h"
#include "../../lib/cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========================================================================== */
/* Save: Serialize application state to JSON file                             */
/* ========================================================================== */

/**
 * @brief Serialize all players into a cJSON array.
 *
 * Iterates over the global player list using the player_iter API
 * and creates a JSON array of player objects.
 *
 * @return cJSON array on success, NULL on memory error
 */
static cJSON* serialize_players(void) {
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return NULL;

    for (player_iter* it = player_iter_first(); it; it = player_iter_next(it)) {
        cJSON* obj = cJSON_CreateObject();
        if (!obj) { cJSON_Delete(arr); return NULL; }

        cJSON_AddStringToObject(obj, "name",      player_iter_name(it));
        cJSON_AddNumberToObject(obj, "age",        player_iter_age(it));
        cJSON_AddNumberToObject(obj, "overall",    player_iter_overall(it));
        cJSON_AddNumberToObject(obj, "potential",  player_iter_potential(it));
        cJSON_AddNumberToObject(obj, "own_rating", player_iter_own(it));

        cJSON_AddItemToArray(arr, obj);
    }
    return arr;
}

/**
 * @brief Serialize all formations into a cJSON array.
 *
 * Iterates over formations and their position slots, building a
 * JSON array of formation objects each containing their positions
 * and assigned players (in preference order).
 *
 * @return cJSON array on success, NULL on memory error
 */
static cJSON* serialize_formations(void) {
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return NULL;

    for (formation_iter* it = formation_iter_first(); it;
         it = formation_iter_next(it)) {
        cJSON* f_obj = cJSON_CreateObject();
        if (!f_obj) { cJSON_Delete(arr); return NULL; }

        cJSON_AddStringToObject(f_obj, "name", formation_iter_name(it));

        cJSON* pos_arr = cJSON_CreateArray();
        if (!pos_arr) { cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }

        /* Iterate over all 24 possible position slots */
        for (int slot = 0; slot < 24; slot++) {
            const char* pos_name = NULL;
            int count = 0;
            if (!formation_iter_position(it, slot, &pos_name, &count)) {
                continue;  /* slot unoccupied */
            }

            cJSON* p_obj = cJSON_CreateObject();
            if (!p_obj) { cJSON_Delete(pos_arr); cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }

            cJSON_AddNumberToObject(p_obj, "slot", slot);
            cJSON_AddStringToObject(p_obj, "name", pos_name);

            /* Player names in preference order */
            cJSON* pl_arr = cJSON_CreateArray();
            if (!pl_arr) { cJSON_Delete(p_obj); cJSON_Delete(pos_arr); cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }
            for (int k = 0; k < count; k++) {
                const char* pn = formation_iter_pos_player(it, slot, k);
                if (pn) cJSON_AddItemToArray(pl_arr, cJSON_CreateString(pn));
            }
            cJSON_AddItemToObject(p_obj, "players", pl_arr);
            cJSON_AddItemToArray(pos_arr, p_obj);
        }

        cJSON_AddItemToObject(f_obj, "positions", pos_arr);
        cJSON_AddItemToArray(arr, f_obj);
    }
    return arr;
}

int save_to_file(const char* filename) {
    if (!filename) filename = SP_DEFAULT_SAVE_FILE;

    /* Build root JSON object */
    cJSON* root = cJSON_CreateObject();
    if (!root) return SP_ERR_MEMORY;

    cJSON* players = serialize_players();
    if (!players) { cJSON_Delete(root); return SP_ERR_MEMORY; }
    cJSON_AddItemToObject(root, "players", players);

    cJSON* formations = serialize_formations();
    if (!formations) { cJSON_Delete(root); return SP_ERR_MEMORY; }
    cJSON_AddItemToObject(root, "formations", formations);

    /* Render to formatted JSON string */
    char* json_str = cJSON_Print(root);
    cJSON_Delete(root);
    if (!json_str) return SP_ERR_MEMORY;

    /* Write to file */
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        cJSON_free(json_str);
        return SP_ERR_FILE_OPEN;
    }
    if (fputs(json_str, fp) == EOF) {
        fclose(fp);
        cJSON_free(json_str);
        return SP_ERR_FILE_WRITE;
    }
    fclose(fp);
    cJSON_free(json_str);

    printf("Data saved to '%s'.\n", filename);
    return SP_SUCCESS;
}

/* ========================================================================== */
/* Load: Deserialize application state from JSON file                         */
/* ========================================================================== */

/**
 * @brief Read entire file contents into a dynamically allocated string.
 *
 * @param filename Path to the file
 * @return Heap-allocated string with file contents, or NULL on error.
 *         Caller must free() the returned pointer.
 */
static char* read_file_contents(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    /* Determine file size */
    if (fseek(fp, 0, SEEK_END) != 0) { fclose(fp); return NULL; }
    long length = ftell(fp);
    if (length < 0) { fclose(fp); return NULL; }
    if (fseek(fp, 0, SEEK_SET) != 0) { fclose(fp); return NULL; }

    char* buffer = malloc((size_t)length + 1);
    if (!buffer) { fclose(fp); return NULL; }

    size_t read = fread(buffer, 1, (size_t)length, fp);
    fclose(fp);
    buffer[read] = '\0';
    return buffer;
}

/**
 * @brief Deserialize players from a cJSON array.
 *
 * Creates players using create_player_direct() from formation.h.
 *
 * @param players_arr cJSON array of player objects
 * @return SP_SUCCESS on success, error code otherwise
 */
static int deserialize_players(const cJSON* players_arr) {
    const cJSON* p_obj = NULL;
    cJSON_ArrayForEach(p_obj, players_arr) {
        const cJSON* j_name = cJSON_GetObjectItemCaseSensitive(p_obj, "name");
        const cJSON* j_age  = cJSON_GetObjectItemCaseSensitive(p_obj, "age");
        const cJSON* j_ovr  = cJSON_GetObjectItemCaseSensitive(p_obj, "overall");
        const cJSON* j_pot  = cJSON_GetObjectItemCaseSensitive(p_obj, "potential");
        const cJSON* j_own  = cJSON_GetObjectItemCaseSensitive(p_obj, "own_rating");

        if (!cJSON_IsString(j_name) || !cJSON_IsNumber(j_age) ||
            !cJSON_IsNumber(j_ovr)  || !cJSON_IsNumber(j_pot) ||
            !cJSON_IsNumber(j_own)) {
            return SP_ERR_FILE_FORMAT;
        }

        int ret = create_player_direct(
            j_name->valuestring,
            j_age->valueint,
            j_ovr->valueint,
            j_pot->valueint,
            j_own->valueint
        );
        if (ret != SP_SUCCESS) return ret;
    }
    return SP_SUCCESS;
}

/**
 * @brief Deserialize formations from a cJSON array.
 *
 * Creates formations, adds positions, and assigns players using the
 * direct creation helpers from formation.h.
 *
 * @param formations_arr cJSON array of formation objects
 * @return SP_SUCCESS on success, error code otherwise
 */
static int deserialize_formations(const cJSON* formations_arr) {
    const cJSON* f_obj = NULL;
    cJSON_ArrayForEach(f_obj, formations_arr) {
        const cJSON* j_name = cJSON_GetObjectItemCaseSensitive(f_obj, "name");
        if (!cJSON_IsString(j_name)) return SP_ERR_FILE_FORMAT;

        int ret = create_formation_direct(j_name->valuestring);
        if (ret != SP_SUCCESS) return ret;

        const cJSON* j_positions = cJSON_GetObjectItemCaseSensitive(f_obj, "positions");
        if (!cJSON_IsArray(j_positions)) return SP_ERR_FILE_FORMAT;

        const cJSON* p_obj = NULL;
        cJSON_ArrayForEach(p_obj, j_positions) {
            const cJSON* j_pname = cJSON_GetObjectItemCaseSensitive(p_obj, "name");
            if (!cJSON_IsString(j_pname)) return SP_ERR_FILE_FORMAT;

            ret = add_position_direct(j_pname->valuestring);
            if (ret != SP_SUCCESS) return ret;

            const cJSON* j_players = cJSON_GetObjectItemCaseSensitive(p_obj, "players");
            if (cJSON_IsArray(j_players)) {
                const cJSON* j_pl = NULL;
                cJSON_ArrayForEach(j_pl, j_players) {
                    if (!cJSON_IsString(j_pl)) return SP_ERR_FILE_FORMAT;
                    ret = add_player_to_position_direct(
                        j_pname->valuestring, j_pl->valuestring);
                    if (ret != SP_SUCCESS) return ret;
                }
            }
        }
    }
    return SP_SUCCESS;
}

int load_from_file(const char* filename) {
    if (!filename) filename = SP_DEFAULT_SAVE_FILE;

    char* content = read_file_contents(filename);
    if (!content) return SP_ERR_FILE_OPEN;

    cJSON* root = cJSON_Parse(content);
    free(content);
    if (!root) return SP_ERR_FILE_FORMAT;

    /* Clear existing data before loading */
    cleanup_all();

    /* Deserialize players first (formations reference them) */
    const cJSON* players_arr = cJSON_GetObjectItemCaseSensitive(root, "players");
    if (cJSON_IsArray(players_arr)) {
        int ret = deserialize_players(players_arr);
        if (ret != SP_SUCCESS) {
            cJSON_Delete(root);
            return ret;
        }
    }

    /* Deserialize formations */
    const cJSON* formations_arr = cJSON_GetObjectItemCaseSensitive(root, "formations");
    if (cJSON_IsArray(formations_arr)) {
        int ret = deserialize_formations(formations_arr);
        if (ret != SP_SUCCESS) {
            cJSON_Delete(root);
            return ret;
        }
    }

    cJSON_Delete(root);
    printf("Data loaded from '%s'.\n", filename);
    return SP_SUCCESS;
}
