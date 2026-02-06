# Squad-Planner

A command-line tool for planning and managing soccer squad formations. Create tactical formations, assign players to positions, and manage your team lineup for Football Manager games or real-life coaching.

## Features

- **Formation Management**: Create, open, and visualize multiple formations
- **Position System**: Support for 24 different soccer positions (GK, CB, ST, etc.)
- **Player Database**: Create players with ratings (overall, potential, custom)
- **Tactical View**: ASCII-art display of your formation
- **Preference Ranking**: Rank players within each position for rotation planning

## Building

```bash
# Build the executable
make main

# Build and run immediately
make main_run

# Clean build artifacts
make clean
```

## Usage

Start the application by running `./main`. You'll be presented with a command prompt that changes based on your current context.

### Main Menu Commands

| Command    | Description                          |
|------------|--------------------------------------|
| `help`     | Display available commands           |
| `formation`| Enter the formation planning menu    |
| `load`     | Load saved data from file (TODO)     |
| `exit`     | Exit the application                 |

### Formation Menu Commands

| Command                        | Description                              |
|--------------------------------|------------------------------------------|
| `help`                         | Display formation menu help              |
| `new <name>`                   | Create a new formation                   |
| `open <name>`                  | Open an existing formation               |
| `add <positions...>`           | Add positions (e.g., `add GK CB ST`)     |
| `show`                         | Display formation in tactical view       |
| `newP <name> <age> <ovr> <pot> <own>` | Create a new player             |
| `addP <position> <player>`     | Assign a player to a position            |
| `list <position>`              | List players assigned to a position      |
| `listf`                        | List all formations                      |
| `removeP <position> <player>`  | Remove player from position              |
| `remove <position>`            | Remove position from formation           |
| `preference <pos> <players...>`| Reorder player preferences               |
| `save`                         | Save all data to file (TODO)             |
| `back`                         | Return to main menu                      |

Most commands support `--help` for detailed usage information.

### Position Abbreviations

| Position | Code | Position | Code |
|----------|------|----------|------|
| Goalkeeper | GK | Right Back | RB |
| Center Back | CB, RCB, LCB | Left Back | LB |
| Wing Back | RWB, LWB | Defensive Mid | CDM, RCDM, LCDM |
| Central Mid | CM, RCM, LCM | Wide Mid | RM, LM |
| Attacking Mid | CAM, RCAM, LCAM, RAM, LAM | |
| Striker | ST, RS, LS | | |

## Example Session

```
squad-planner - main> formation
squad-planner - formation> new 4-3-3
squad-planner - '4-3-3' formation> add GK RB CB LCB LB CM RCM LCM ST RS LS
squad-planner - '4-3-3' formation> show

      LS    ST    RS              
                         
      LCM   CM    RCM             
                         
   LB    LCB   RCB    RB              
            GK                    

squad-planner - '4-3-3' formation> newP Messi 36 88 88 95
squad-planner - '4-3-3' formation> addP RS Messi
squad-planner - '4-3-3' formation> list RS
Players for position RS:
- Messi
```

## Project Structure

```
Squad-Planner/
├── main.c                 # Entry point and main loop
├── Makefile               # Build configuration
├── src/
│   ├── command.h          # Command structure definition
│   ├── parser/            # Command line parsing
│   │   ├── parser.c
│   │   └── parser.h
│   ├── exec/              # Command execution/dispatch
│   │   ├── exec.c
│   │   └── exec.h
│   ├── formation/         # Formation and player management
│   │   ├── formation.c
│   │   ├── formation.h
│   │   ├── player.h
│   │   └── position.h
│   ├── help/              # Help system
│   │   ├── help.c
│   │   └── help.h
│   └── errorhandler/      # Error handling utilities
│       ├── errorhandler.c
│       └── errorhandler.h
```

## Architecture

### Module Responsibilities

- **parser**: Tokenizes input, validates commands against current context, extracts options/arguments
- **exec**: Dispatches parsed commands to appropriate handlers based on context
- **formation**: Core domain logic for formations, positions, and players
- **help**: Displays context-sensitive help pages
- **errorhandler**: Centralized error message display (partially implemented)

### Data Flow

```
User Input → parser (tokenize/validate) → command struct → exec (dispatch) → formation (execute)
```

### Error Handling

The project uses numeric error codes:
- `0`: Success
- `-1`: System/memory error
- `-2`: Invalid input/arguments
- `-3`: Resource not found

## Future Improvements

### Planned Features
- [ ] **Data Persistence**: Save/load formations and players to files
- [ ] **Enhanced Error Messages**: Context-aware error descriptions
- [ ] **Player Statistics**: View detailed player info
- [ ] **Formation Templates**: Pre-built common formations (4-4-2, 4-3-3, etc.)
- [ ] **Export Options**: Export formations to different formats

### Code Improvements
- [ ] Move error code definitions to a central header
- [ ] Add unit tests for core functions
- [ ] Implement the saves menu fully
- [ ] Add input validation for player names (no spaces)

---

## Technical Documentation

### Header Guards Explained

You'll notice patterns like this at the top and bottom of header files:

```c
#ifndef FORMATION_H
#define FORMATION_H

/* ... header content ... */

#endif /* FORMATION_H */
```

**What are header guards?**

Header guards (also called include guards) prevent a header file from being included multiple times in a single compilation unit, which would cause "redefinition" errors.

**How they work:**

1. `#ifndef FORMATION_H` - "If FORMATION_H is NOT defined, continue"
2. `#define FORMATION_H` - "Define FORMATION_H so future includes skip this"
3. `#endif` - "End of the conditional block"

**Why we need them:**

Imagine you have:
- `player.h` includes nothing
- `position.h` includes `player.h`
- `formation.h` includes `position.h`
- `main.c` includes both `formation.h` AND `player.h`

Without guards, `player.h` would be processed twice, causing errors like:
```
error: redefinition of 'struct player'
```

With guards, the second include sees `PLAYER_H` is already defined and skips the content.

**Naming convention:** Use the filename in uppercase with underscores: `my_header.h` → `MY_HEADER_H`

---

### Central Error Code Header (Recommended Approach)

Currently, error codes like `-1`, `-2`, `-3` are used throughout the code but not centrally defined. Here's how to implement a central error header:

**1. Create `src/errors.h`:**

```c
/**
 * @file errors.h
 * @brief Central error code definitions for Squad-Planner.
 * 
 * All modules should use these error codes for consistency.
 * Include this header in any file that returns or checks error codes.
 */

#ifndef ERRORS_H
#define ERRORS_H

/* Success */
#define SP_SUCCESS           0

/* General errors (-1 to -9) */
#define SP_ERR_MEMORY       -1   /* Memory allocation failed */
#define SP_ERR_NULL_PTR     -2   /* Null pointer passed */
#define SP_ERR_INTERNAL     -3   /* Internal/unexpected error */

/* Input validation errors (-10 to -19) */
#define SP_ERR_INVALID_ARG  -10  /* Invalid argument value */
#define SP_ERR_MISSING_ARG  -11  /* Required argument missing */
#define SP_ERR_TOO_MANY_ARG -12  /* Too many arguments */

/* Resource errors (-20 to -29) */
#define SP_ERR_NOT_FOUND    -20  /* Resource not found */
#define SP_ERR_DUPLICATE    -21  /* Resource already exists */
#define SP_ERR_LIMIT        -22  /* Limit exceeded (e.g., 11 positions) */

/* File I/O errors (-30 to -39) */
#define SP_ERR_FILE_OPEN    -30  /* Could not open file */
#define SP_ERR_FILE_READ    -31  /* Error reading file */
#define SP_ERR_FILE_WRITE   -32  /* Error writing file */
#define SP_ERR_FILE_FORMAT  -33  /* Invalid file format */

/**
 * @brief Get a human-readable error message.
 * @param error_code The error code to describe
 * @return Static string describing the error
 */
const char* sp_error_string(int error_code);

#endif /* ERRORS_H */
```

**2. Implement in `src/errors.c`:**

```c
#include "errors.h"

const char* sp_error_string(int error_code) {
    switch (error_code) {
        case SP_SUCCESS:        return "Success";
        case SP_ERR_MEMORY:     return "Memory allocation failed";
        case SP_ERR_NULL_PTR:   return "Null pointer error";
        case SP_ERR_NOT_FOUND:  return "Resource not found";
        /* ... etc ... */
        default:                return "Unknown error";
    }
}
```

**Benefits:**
- Single source of truth for error codes
- Easy to add new error codes
- Human-readable error messages
- Consistent error handling across modules
- Self-documenting code

---

### Data Persistence: CSV Format Approach

For implementing save/load functionality, here's a recommended CSV approach:

**Players CSV (`players.csv`):**
```csv
name,age,overall,potential,own_rating
Messi,36,88,88,95
Ronaldo,38,85,85,90
Mbappé,25,91,95,92
```

**Formations CSV (`formations.csv`):**
```csv
formation_name,position_id,player_name
4-3-3,0,
4-3-3,1,
4-3-3,22,Messi
4-3-3,23,Ronaldo
```

**Implementation tips:**

1. **Reading CSV:**
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int load_players_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) return -1;
    
    char line[512];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        /* Skip header row */
        if (line_num == 1) continue;
        
        /* Remove newline */
        line[strcspn(line, "\n")] = 0;
        
        /* Parse CSV fields */
        char* name = strtok(line, ",");
        char* age_str = strtok(NULL, ",");
        char* overall_str = strtok(NULL, ",");
        char* potential_str = strtok(NULL, ",");
        char* own_str = strtok(NULL, ",");
        
        if (!name || !age_str || !overall_str || !potential_str || !own_str) {
            fprintf(stderr, "Warning: Invalid line %d\n", line_num);
            continue;
        }
        
        /* Convert and create player */
        int age = atoi(age_str);
        int overall = atoi(overall_str);
        int potential = atoi(potential_str);
        int own = atoi(own_str);
        
        /* Call your new_player function or create directly */
    }
    
    fclose(file);
    return 0;
}
```

2. **Writing CSV:**
```c
int save_players_csv(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return -1;
    
    /* Write header */
    fprintf(file, "name,age,overall,potential,own_rating\n");
    
    /* Write each player */
    player* p = player_head;
    while (p != NULL) {
        fprintf(file, "%s,%d,%d,%d,%d\n",
                p->name, p->age, p->overall_rating,
                p->potential_rating, p->own_rating);
        p = p->next;
    }
    
    fclose(file);
    return 0;
}
```

**CSV vs Custom Format:**

| Aspect | CSV | Custom (colon-separated) |
|--------|-----|-------------------------|
| Readability | Better (opens in Excel) | Good |
| Parsing | Need to handle commas in names | Simpler with strtok |
| Standards | Well-defined | Your own rules |
| Libraries | Many available | None needed |

**Handling commas in CSV names:**
If player names might contain commas, use quoted fields:
```csv
name,age
"O'Brien, John",25
```

And parse with quote awareness, or use a CSV library.

---

## License

See [LICENSE](LICENSE) file for details.
