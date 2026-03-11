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
 * - Serialization obtains a Squad snapshot via get_squad() and
 *   traverses the player and formation linked lists from there.
 */

#include "persistence.h"
#include "../formation/formation.h"
#include "../squad/squad.h"
#include "../error/error.h"
#include "../../lib/cJSON/cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/* ========================================================================== */
/* Save: Serialize application state to JSON file                             */
/* ========================================================================== */

/**
 * @brief Serialize all players into a cJSON array.
 *
 * Traverses the player linked list starting at head and creates a
 * JSON array of player objects.
 *
 * @param head Head of the player linked list
 * @return cJSON array on success, NULL on memory error
 */
static cJSON* serialize_players(player* head) {
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return NULL;

    for (player* p = head; p; p = p->next) {
        cJSON* obj = cJSON_CreateObject();
        if (!obj) { cJSON_Delete(arr); return NULL; }

        cJSON_AddStringToObject(obj, "name",      p->name);
        cJSON_AddNumberToObject(obj, "age",        p->age);
        cJSON_AddNumberToObject(obj, "overall",    p->overall_rating);
        cJSON_AddNumberToObject(obj, "potential",  p->potential_rating);
        cJSON_AddNumberToObject(obj, "own_rating", p->own_rating);

        cJSON_AddItemToArray(arr, obj);
    }
    return arr;
}

/**
 * @brief Serialize all formations into a cJSON array.
 *
 * Traverses the formation linked list starting at head and their
 * position slots, building a JSON array of formation objects each
 * containing their positions and assigned players (in preference order).
 *
 * @param head Head of the formation linked list
 * @return cJSON array on success, NULL on memory error
 */
static cJSON* serialize_formations(formation* head) {
    cJSON* arr = cJSON_CreateArray();
    if (!arr) return NULL;

    for (formation* f = head; f; f = f->next) {
        cJSON* f_obj = cJSON_CreateObject();
        if (!f_obj) { cJSON_Delete(arr); return NULL; }

        cJSON_AddStringToObject(f_obj, "name", f->name);

        cJSON* pos_arr = cJSON_CreateArray();
        if (!pos_arr) { cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }

        /* Iterate over all 24 possible position slots */
        for (int slot = 0; slot < 24; slot++) {
            position* pos = f->map_of_positions[slot];
            if (!pos) continue;  /* slot unoccupied */

            cJSON* p_obj = cJSON_CreateObject();
            if (!p_obj) { cJSON_Delete(pos_arr); cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }

            cJSON_AddNumberToObject(p_obj, "slot", slot);
            cJSON_AddStringToObject(p_obj, "name", pos->name);

            /* Player names in preference order */
            cJSON* pl_arr = cJSON_CreateArray();
            if (!pl_arr) { cJSON_Delete(p_obj); cJSON_Delete(pos_arr); cJSON_Delete(f_obj); cJSON_Delete(arr); return NULL; }
            for (int k = 0; k < pos->size_of_list; k++) {
                if (pos->list_of_players[k]) {
                    cJSON_AddItemToArray(pl_arr, cJSON_CreateString(pos->list_of_players[k]->name));
                }
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

    /* Serialize all squads into an array */
    cJSON* squads_arr = cJSON_CreateArray();
    if (!squads_arr) { cJSON_Delete(root); return SP_ERR_MEMORY; }
    Squad* s_head = get_squad_head();
    for (Squad* s = s_head; s; s = s->next) {
        cJSON* s_obj = cJSON_CreateObject();
        if (!s_obj) { cJSON_Delete(squads_arr); cJSON_Delete(root); return SP_ERR_MEMORY; }
        cJSON_AddStringToObject(s_obj, "name", s->name ? s->name : "");
        cJSON* p_arr = serialize_players(s->players);
        if (!p_arr) { cJSON_Delete(s_obj); cJSON_Delete(squads_arr); cJSON_Delete(root); return SP_ERR_MEMORY; }
        cJSON_AddItemToObject(s_obj, "players", p_arr);
        cJSON* f_arr = serialize_formations(s->formations);
        if (!f_arr) { cJSON_Delete(s_obj); cJSON_Delete(squads_arr); cJSON_Delete(root); return SP_ERR_MEMORY; }
        cJSON_AddItemToObject(s_obj, "formations", f_arr);
        cJSON_AddItemToArray(squads_arr, s_obj);
    }
    cJSON_AddItemToObject(root, "squads", squads_arr);

    /* Save name of currently active squad (if any) */
    char cur_name[40] = {0};
    if (get_current_squad_name(cur_name) == SP_SUCCESS) {
        cJSON_AddStringToObject(root, "current_squad", cur_name);
    } else {
        cJSON_AddStringToObject(root, "current_squad", "");
    }

    /* Render to formatted JSON string */
    char* json_str = cJSON_Print(root);
    cJSON_Delete(root);
    if (!json_str) return SP_ERR_MEMORY;

    /* Write to file */
    char* fname = strdup(filename);
    if (!fname) { 
        cJSON_free(json_str);
         return SP_ERR_MEMORY; 
    }
    char* temp = realloc(fname, strlen(fname) + 5);  /* Add space for ".json" */
    if (!temp) { 
        free(fname);
        cJSON_free(json_str);
        return SP_ERR_MEMORY;
    }
    fname = temp;
    int len = strlen(fname);
    fname[len] = '.';
    fname[len+1] = 'j';
    fname[len+2] = 's';
    fname[len+3] = 'o';
    fname[len+4] = 'n';
    fname[len+5] = '\0';  /* Ensure null-termination */
    /* Ensure the saves directory exists (create if missing) */
    struct stat st = {0};
    if (stat("./saves", &st) != 0) {
        if (mkdir("./saves", 0755) != 0) {
            cJSON_free(json_str);
            free(fname);
            return SP_ERR_FILE_OPEN;  /* Could not create saves directory */
        }
    } else if (!S_ISDIR(st.st_mode)) {
        cJSON_free(json_str);
        free(fname);
        return SP_ERR_FILE_OPEN; /* Path exists but is not a directory */
    }

    /* Build full path: ./saves/<fname> */
    size_t path_len = strlen("./saves/") + strlen(fname) + 1;
    char* path = malloc(path_len);
    if (!path) { cJSON_free(json_str); free(fname); return SP_ERR_MEMORY; }
    int wn = snprintf(path, path_len, "./saves/%s", fname);
    if (wn < 0 || (size_t)wn >= path_len) { cJSON_free(json_str); free(fname); free(path); return SP_ERR_FILE_OPEN; }
    FILE* fp = fopen(path, "w");
    if (!fp) {
        cJSON_free(json_str);
        free(fname);
        free(path);
        return SP_ERR_FILE_OPEN;
    }
    if (fputs(json_str, fp) == EOF) {
        fclose(fp);
        cJSON_free(json_str);
        free(fname);
        free(path);
        return SP_ERR_FILE_WRITE;
    }
    fclose(fp);
    cJSON_free(json_str);
    free(fname);
    free(path);

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
    char* fname = strdup(filename);
    if (!fname) {
        return NULL;
    }
    char* temp = realloc(fname, strlen(fname) + 5);  /* Add space for ".json" */
    if (!temp) {
        free(fname);
        return NULL;
    }
    fname = temp;
    int len = strlen(fname);
    fname[len] = '.';
    fname[len+1] = 'j';
    fname[len+2] = 's';
    fname[len+3] = 'o';
    fname[len+4] = 'n';
    fname[len+5] = '\0';  /* Ensure null-termination */
    /* Build path ./saves/<fname>.json and open without changing cwd */
    struct stat st = {0};
    if (stat("./saves", &st) != 0 || !S_ISDIR(st.st_mode)) {
        free(fname);
        return NULL; /* saves directory missing or not a dir */
    }
    size_t path_len = strlen("./saves/") + strlen(fname) + 1;
    char* path = malloc(path_len);
    if (!path) { free(fname); return NULL; }
    int rn = snprintf(path, path_len, "./saves/%s", fname);
    if (rn < 0 || (size_t)rn >= path_len) { free(fname); free(path); return NULL; }
    FILE* fp = fopen(path, "r");
    if (!fp) {
        free(fname);
        free(path);
        return NULL;
    }

    /* Determine file size */
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        free(fname);
        return NULL;
    }
    long length = ftell(fp);
    if (length < 0) { 
        fclose(fp);
        free(fname);
        return NULL; 
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        free(fname); 
        return NULL; 
    }

    char* buffer = malloc((size_t)length + 1);
    if (!buffer) {
        fclose(fp);
        free(fname);
        return NULL; 
    }

    size_t read = fread(buffer, 1, (size_t)length, fp);
    fclose(fp);
    buffer[read] = '\0';
    free(fname);
    free(path);
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

    /* If file contains a `squads` array -> multi-squad format */
    const cJSON* squads_arr = cJSON_GetObjectItemCaseSensitive(root, "squads");
    if (cJSON_IsArray(squads_arr)) {
        /* Clear existing data (remove all squads and their contents) */
        clear_all_squads();

        const cJSON* s_obj = NULL;
        cJSON_ArrayForEach(s_obj, squads_arr) {
            const cJSON* j_name = cJSON_GetObjectItemCaseSensitive(s_obj, "name");
            if (!cJSON_IsString(j_name)) { cJSON_Delete(root); return SP_ERR_FILE_FORMAT; }
            char* args_new[2]; args_new[0] = (char*)j_name->valuestring; args_new[1] = NULL;
            int ret = new_squad(args_new, NULL);
            if (ret == SP_ERR_SQUAD_EXISTS) {
                ret = open_squad(args_new, NULL);
                if (ret != SP_SUCCESS) { 
                    cJSON_Delete(root);
                    return ret;
                }
            } 
            else if (ret != SP_SUCCESS) {
                cJSON_Delete(root); return ret;
            }

            const cJSON* j_players = cJSON_GetObjectItemCaseSensitive(s_obj, "players");
            if (cJSON_IsArray(j_players)) {
                int r = deserialize_players(j_players);
                if (r != SP_SUCCESS) { 
                    cJSON_Delete(root); 
                    return r; 
                }
            }

            const cJSON* j_forms = cJSON_GetObjectItemCaseSensitive(s_obj, "formations");
            if (cJSON_IsArray(j_forms)) {
                int r = deserialize_formations(j_forms);
                if (r != SP_SUCCESS) { 
                    cJSON_Delete(root); 
                    return r; 
                }
            }
        }

        /* Restore current squad if present; otherwise clear selection */
        /* Do not auto-select a current squad after loading; leave selection to the user */
        setting_no_current_squad();

        cJSON_Delete(root);
        printf("Data loaded from '%s'.\n", filename);
        return SP_SUCCESS;
    }

    /* Legacy single-squad format: clear existing data and recreate target squad if named */
    const cJSON* j_squad_name = cJSON_GetObjectItemCaseSensitive(root, "squad_name");
    clear_all_squads();
    if (cJSON_IsString(j_squad_name) && j_squad_name->valuestring && j_squad_name->valuestring[0] != '\0') {
        char* args_new[2]; args_new[0] = (char*)j_squad_name->valuestring; args_new[1] = NULL;
        int ret = new_squad(args_new, NULL);
        if (ret == SP_ERR_SQUAD_EXISTS) {
            ret = open_squad(args_new, NULL);
            if (ret != SP_SUCCESS) { 
                cJSON_Delete(root);
                return ret;
            }
        } 
        else if (ret != SP_SUCCESS) {
            cJSON_Delete(root); return ret;
        }
        /* Do not automatically select a squad after legacy load */
        setting_no_current_squad();
    } else {
        setting_no_current_squad();
    }

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
