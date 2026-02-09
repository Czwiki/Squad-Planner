#ifndef ERRORS_H
#define ERRORS_H

const char* sp_error_string(int error_code);

/* Success */
#define SP_SUCCESS           0

/* General errors (-1 to -9) */
#define SP_ERR_MEMORY       -1   /* Memory allocation failed */
#define SP_ERR_NULL_PTR     -2   /* Null pointer passed */
#define SP_ERR_INTERNAL     -3   /* Internal/unexpected error */
#define SP_ERR_INVALID_CMD  -4   /* Invalid command */

/* Input validation errors (-10 to -19) */
#define SP_ERR_INVALID_POSITION  -10  /* Invalid position name */
#define SP_ERR_NOT_ASSIGNED_POSITION  -11  /* Position not assigned in formation */
#define SP_ERR_TOO_MANY_POSITIONS -12  /* Too many positions in formation */
#define SP_ERR_MISSING_ARG  -13  /* Required argument missing */
#define SP_ERR_TOO_MANY_ARG -14  /* Too many arguments */
#define SP_ERR_TOO_MANY_OPT -15  /* Too many options */
#define SP_ERR_WRONG_USAGE    -16  /* Command usage error */
#define SP_ERR_INVALID_RANGE    -17  /* Argument value out of valid range */

/* Resource errors (-20 to -29) */
#define SP_ERR_POS_NOT_FOUND    -20  /* Resource not found */
#define SP_ERR_FORMATION_EXISTS    -21  /* Formation already exists */
#define SP_ERR_PLAYER_EXISTS    -22  /* Player already exists */
#define SP_ERR_PLAYER_NOT_FOUND    -23  /* Player not found */
#define SP_ERR_PLAYER_ALREADY_ASSIGNED    -24  /* Player already assigned to a position */
#define SP_ERR_LIMIT        -25  /* Limit exceeded (e.g., 11 positions) */
#define SP_ERR_NO_FORMATION    -26  /* No formation currently open */

// TODO
/* File I/O errors (-30 to -39) */
#define SP_ERR_FILE_OPEN    -30  /* Could not open file */
#define SP_ERR_FILE_READ    -31  /* Error reading file */
#define SP_ERR_FILE_WRITE   -32  /* Error writing file */
#define SP_ERR_FILE_FORMAT  -33  /* Invalid file format */

#endif /* ERRORS_H */
