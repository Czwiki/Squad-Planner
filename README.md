# Squad-Planner

A command-line tool for planning and managing soccer squad formations. Create tactical formations, assign players to positions, and manage your team lineup for Football Manager games or real-life coaching.

## Features

- **Squad Management**: Create and switch between multiple squads
- **Formation Management**: Create, open, and visualize multiple formations per squad
- **Position System**: Support for 24 different soccer positions (GK, CB, ST, etc.)
- **Player Database**: Create players with ratings (overall, potential, custom) and match statistics
- **Tactical View**: ASCII-art display of your formation
- **Preference Ranking**: Rank players within each position for rotation planning
- **Persistence**: Save and load all data to/from a JSON file

Feature proposals and roadmap ideas are maintained in `FEATURE_PROPOSALS.md`.

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

Start the application by running `./main`. You'll be presented with a command prompt that changes based on your current context:

```
squad-planner - main>                  ← main menu
squad-planner - 'MySquad' squad>       ← squad menu, squad open
squad-planner - '4-3-3' formation>     ← formation menu, formation open
squad-planner - player>                ← player menu
squad-planner - 'Messi' player>        ← player editing sub-menu
squad-planner - saves>                 ← saves menu
```

Input limit: each command line supports up to **199 characters** (`MAX_INPUT_SIZE = 200`, including newline terminator handling).

### Navigation Overview

```
main ──► squad ──► formation
                └─► players ──► (openP) player editing sub-menu
                └─► save ──► saves
```

---

### Main Menu

| Command  | Description                                     |
|----------|-------------------------------------------------|
| `help`   | Display available commands                      |
| `squad`  | Enter the squad planning menu                   |
| `load`   | Load saved data from file (enters saves menu)   |
| `exit`   | Exit the application (prompts for confirmation) |

---

### Squad Menu

Prompt: `squad-planner - '<name>' squad>`

| Command      | Description                                     |
|--------------|-------------------------------------------------|
| `help`       | Display squad menu help                         |
| `new <name>` | Create a new squad and open it                  |
| `list`       | List all squads                                 |
| `open <name>`| Open an existing squad                          |
| `formation`  | Enter the formation menu for the current squad  |
| `players`    | Enter the player menu for the current squad     |
| `back`       | Return to main menu                             |
| `save`       | Save all data (enters saves menu)               |

Multi-word names: use underscores instead of spaces (e.g. `new Borussia_Dortmund`).

---

### Formation Menu

Prompt: `squad-planner - '<name>' formation>`

| Command                               | Description                                          |
|---------------------------------------|------------------------------------------------------|
| `help`                                | Display formation menu help                          |
| `new <name>`                          | Create a new formation                               |
| `open <name>`                         | Open an existing formation for editing               |
| `listf`                               | List all formations                                  |
| `add <positions...>`                  | Add one or more positions (e.g. `add GK CB ST`)      |
| `remove <position>`                   | Remove a position from the formation                 |
| `addP <position> <player...>`         | Assign one or more existing players to a position    |
| `removeP <position> <player>`         | Remove a player from a position                      |
| `list <position>`                     | List players assigned to a position                  |
| `preference <pos> <players...>`       | Reorder player preference list for a position        |
| `show`                                | Display the formation in tactical view               |
| `deletef <name>`                      | Delete a formation                                   |
| `save`                                | Save all data (enters saves menu)                    |
| `back`                                | Return to squad menu                                 |

Most commands support `--help` for detailed usage information.

---

### Player Menu

Prompt: `squad-planner - player>`

| Command                              | Description                                                    |
|--------------------------------------|----------------------------------------------------------------|
| `help`                               | Display player menu help                                       |
| `newP <name> <age> <ovr> <pot> <own>`| Create a new player with base ratings (0–100, age > 0)         |
| `openP <name>`                       | Open a player → enters the **player editing sub-menu**         |
| `list`                               | List all players in the current squad                          |
| `deleteP <name>`                     | Remove a player and clear them from all formation positions     |
| `editP <name> <attr> <value>`        | Quick-edit a base attribute: `age`, `overall`, `potential`, `own` |
| `save`                               | Save all data (enters saves menu)                              |
| `back`                               | Return to squad menu                                           |

> **Note:** Statistics (`goals`, `assists`, etc.) can **only** be changed via `openP` and the `add` command in the player editing sub-menu – they cannot be set directly with `editP`.

---

### Player Editing Sub-Menu

Entered by running `openP <name>` in the player menu.  
Prompt: `squad-planner - '<name>' player>`

| Command                 | Description                                                                  |
|-------------------------|------------------------------------------------------------------------------|
| `help`                  | Display player editing help                                                  |
| `show`                  | Display all attributes and statistics of the open player                     |
| `set <attr> <value>`    | Set a base attribute: `age`, `overall`, `potential`, `own`                   |
| `add <stat> [amount]`   | **Increment** a statistic by `amount` (default: 1); amount must be positive  |
| `subtract <stat> [amount]` | **Decrement** a statistic by `amount` (default: 1); value cannot drop below 0 |
| `back`                  | Return to player menu                                                        |

Valid stats for `add`/`subtract`: `goals`, `assists`, `appearances`, `yellow_cards`, `red_cards`.

---

### Saves Menu

Prompt: `squad-planner - saves>`

| Command             | Description                                  |
|---------------------|----------------------------------------------|
| `help`              | Display saves menu help                      |
| `save [filename]`   | Save all data to file (default: `squad_planner_data.json`) |
| `load [filename]`   | Load all data from file (default: `squad_planner_data.json`) |
| `back`              | Return to main menu                          |

---

### Position Abbreviations

| Position        | Code(s)              | Position           | Code(s)          |
|-----------------|----------------------|--------------------|------------------|
| Goalkeeper      | `GK`                 | Right Back         | `RB`             |
| Center Back     | `CB`, `RCB`, `LCB`   | Left Back          | `LB`             |
| Wing Back       | `RWB`, `LWB`         | Defensive Mid      | `CDM`, `RCDM`, `LCDM` |
| Central Mid     | `CM`, `RCM`, `LCM`   | Wide Mid           | `RM`, `LM`       |
| Attacking Mid   | `CAM`, `RCAM`, `LCAM`, `RAM`, `LAM` | Striker | `ST`, `RS`, `LS` |

---

## Example Session

### Formation planning

```
squad-planner - main> squad
squad-planner - squad> new Dortmund
Success
squad-planner - 'Dortmund' squad> formation
squad-planner - formation> new 4-3-3
Success
squad-planner - '4-3-3' formation> add GK RB CB LCB LB CM RCM LCM ST RS LS
Success
squad-planner - '4-3-3' formation> show

      LS    ST    RS          
                              
      LCM   CM    RCM         
                              
LB          CB          RB    
            GK                

Success
squad-planner - '4-3-3' formation> back
```

### Player management and statistics

```
squad-planner - 'Dortmund' squad> players
squad-planner - player> newP Messi 37 94 94 98
Success
squad-planner - player> editP Messi age 38
Success
squad-planner - player> openP Messi
Success
squad-planner - 'Messi' player> show
Player: Messi
  Age:        38
  Overall:    94
  Potential:  94
  Own Rating: 98
  --- Stats ---
  Goals:        0
  Assists:      0
  Appearances:  0
  Yellow Cards: 0
  Red Cards:    0
Success
squad-planner - 'Messi' player> set overall 95
Success
squad-planner - 'Messi' player> add goals 3
Success
squad-planner - 'Messi' player> add assists
Success
squad-planner - 'Messi' player> show
Player: Messi
  Age:        38
  Overall:    95
  Potential:  94
  Own Rating: 98
  --- Stats ---
  Goals:        3
  Assists:      1
  Appearances:  0
  Yellow Cards: 0
  Red Cards:    0
Success
squad-planner - 'Messi' player> back
squad-planner - player> back
squad-planner - 'Dortmund' squad> save
squad-planner - saves> save
Success
```
