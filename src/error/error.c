#include "error.h"

const char* sp_error_string(int error_code) {
    switch (error_code) {
        case SP_SUCCESS:        return "Success";
        case SP_ERR_MEMORY:     return "Memory allocation failed";
        case SP_ERR_NULL_PTR:   return "Null pointer error";
        case SP_ERR_INTERNAL:   return "Internal error";
        case SP_ERR_INVALID_CMD: return "Invalid command";
        
        case SP_ERR_INVALID_POSITION: return "Invalid position name";
        case SP_ERR_NOT_ASSIGNED_POSITION: return "Position not assigned in formation";
        case SP_ERR_WRONG_USAGE: return "Wrong command usage (use --help for details)";
        case SP_ERR_INVALID_RANGE: return "Argument value out of valid range";

        case SP_ERR_POS_NOT_FOUND: return "Position not found";
        case SP_ERR_FORMATION_EXISTS: return "Formation already exists";
        case SP_ERR_PLAYER_EXISTS: return "Player already exists";
        case SP_ERR_PLAYER_NOT_FOUND: return "Player not found";
        case SP_ERR_PLAYER_ALREADY_ASSIGNED: return "Player already assigned to a position";
        case SP_ERR_LIMIT: return "Limit exceeded";
        case SP_ERR_NO_FORMATION: return "No formation currently open";
        case SP_ERR_NO_SQUAD: return "No squad currently open";
        case SP_ERR_SQUAD_EXISTS: return "Squad already exists";
        case SP_ERR_SQUAD_NOT_FOUND: return "Squad not found";

        case SP_ERR_FILE_OPEN:  return "File open error";
        case SP_ERR_FILE_READ:  return "File read error";
        case SP_ERR_FILE_WRITE: return "File write error";
        case SP_ERR_FILE_FORMAT: return "Invalid file format";

        default:                return "Unknown error";
    }
}
