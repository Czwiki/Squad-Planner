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
| `load`     | Load saved data from file            |
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
| `save`                         | Save all data to file                    |
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
                         
LB    LCB   CB    RB              
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
- [x] **Data Persistence**: Save/load formations and players to files
- [ ] **Enhanced Error Messages**: Context-aware error descriptions
- [ ] **Player Statistics**: View detailed player info
- [ ] **Formation Templates**: Pre-built common formations (4-4-2, 4-3-3, etc.)
- [ ] **Export Options**: Export formations to different formats

### Code Improvements
- [ ] Move error code definitions to a central header
- [ ] Add unit tests for core functions
- [ ] Implement the saves menu fully
- [ ] Add input validation for player names (no spaces)

## Data Persistence

Data persistence is implemented using a simple text-based file format. Use `save` in the formation menu to save all data, and `load` in the main menu to restore it.

### Save File Format

The save file (`squad_planner_data.txt`) uses a line-based format:

```
# Squad Planner Save File
# Comments start with #

# Players: PLAYER:name:age:overall:potential:own_rating
PLAYER:Messi:36:88:88:95
PLAYER:Ronaldo:38:85:85:90

# Formations
FORMATION:4-3-3

# Positions: POSITION:formation_name:position_id
POSITION:4-3-3:0
POSITION:4-3-3:22

# Player assignments: ASSIGN:formation_name:position_id:player_name
ASSIGN:4-3-3:0:Messi
```

Position IDs map to abbreviations:
- 0: GK, 1: RB, 2: RCB, 3: CB, 4: LCB, 5: LB
- 6: RWB, 7: RCDM, 8: CDM, 9: LCDM, 10: LWB
- 11: RM, 12: RCM, 13: CM, 14: LCM, 15: LM
- 16: RAM, 17: RCAM, 18: CAM, 19: LCAM, 20: LAM
- 21: RS, 22: ST, 23: LS

## License

See [LICENSE](LICENSE) file for details.