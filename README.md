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
│   └── error/             # Error handling utilities
│       ├── error.c
│       └── error.h
```

## Architecture

### Module Responsibilities

- **parser**: Tokenizes input, validates commands against current context, extracts options/arguments
- **exec**: Dispatches parsed commands to appropriate handlers based on context
- **formation**: Core domain logic for formations, positions, and players
- **help**: Displays context-sensitive help pages
- **error**: Centralized error codes and messages (see `src/error/error.h`)

### Data Flow

```
User Input → parser (tokenize/validate) → command struct → exec (dispatch) → formation (execute)
```

### Error Handling

The project uses numeric error codes (see `src/error/error.h`).

## Future Improvements

### Planned Features
- [ ] **Data Persistence**: Save/load formations and players to files
- [ ] **Export Options**: Export formations to different formats

### Code Improvements
- [ ] Add unit tests for core functions
- [ ] Implement the saves menu fully

## License

See [LICENSE](LICENSE) file for details.
