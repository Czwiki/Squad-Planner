# Squad-Planner TUI Refactor

## Goal

Wrap the existing C backend in an ncurses single-pane TUI. Replace the `fgets`+`scanf` REPL in `main.c` with an ncurses event loop. Backend modules (parser, exec, persistence, squad, player, formation) remain untouched — they are called through the existing `parse_command()` + `execute_command()` interfaces.

## Branch

`feat/ncurses-tui`

## Key Decisions

- **Single-pane layout** with a **top tab bar** (`Main | Squad | Formation | Player | Saves`) rendered with ncurses in the remaining row.
- **Esc / back navigation** — a back stack tracks navigation depth; Esc pops the stack. Switching tabs resets state.
- **FM-lite drill-down** (Option A) within Formation:
  - Slot list on left → Enter opens occupant list → Enter on player shows player card detail view (`[e]dit` / `[d]elete` / Esc to back).
  - The existing ASCII pitch (`show`) is rendered as a panel within the Formation view.
- **Terminal resize** — `SIGWINCH` handler calls `resizeterm()` and triggers a full redraw. All draw functions use `getmaxyx()` at render time. Enforce minimum 80×24; show a centered "terminal too small" overlay when dimensions are insufficient.
- **Backward compatibility** — `make main` builds the original CLI binary untouched. `make tui` builds the new ncurses binary.

## New Files

```
src/tui/tui.h          — public init/run/teardown API, tab overview
src/tui/views.h        — view render functions (one per tab), selection state
src/tui/views.c        — implementations: main_view, squad_view, formation_view, player_view, saves_view
src/tui/cards.h        — player card and slot-list render helpers
src/tui/cards.c        — render player detail card, position slot list, ASCII pitch wrapper
src/tui/input.h        — key handling, tab switching, back stack
src/tui/input.c        — getch loop, SIGWINCH handler, key-to-command translation
```

## Modified Files

- `main.c` — guard the existing `main()` behind a `CLI_MODE` macro; add a `tui_main()` that calls `tui_run()`. 
- `Makefile` — new target `make tui` linking `-lncurses`; add object rules for all `src/tui/*.c`; keep `make main` unchanged.

## Implementation Order

1. **Scaffold TUI shell** (`src/tui/tui.c`) — init ncurses, raw mode, SIGWINCH handler, getch loop, top tab bar render, minimum size check overlay.
2. **Tab bar + switching** — render tabs, highlight active tab, `Tab`/`Left`/`Right` arrows to switch, back stack init per tab.
3. **Simplest views first** (`saves_view`, `main_view`) — list renders to verify layout plumbing before touching mutable state.
4. **Squad view** — list squads, selection cursor, `Enter` calls `open_squad` via exec path, display feedback.
5. **Formation view** (FM-lite pivot):
   - Render ASCII pitch using existing `show` logic (capture output or extract draw routine; if inline render is too coupled, render the pitch to a buffer and `mvprintw` it).
   - Render position-slot list beside it when width permits; list scrolls independently.
   - `Enter` on slot → push back state → render occupant list view.
   - `Enter` on occupant name → push back state → render player card detail.
6. **Player card detail** — show all attributes and stats, `[e]` to enter edit mode (reuse `set` / `add` / `subtract` exec commands), `[d]` triggers `deleteP`, Esc pops stack.
7. **Back stack polish** — state restoration on pop, cursor reposition, dirty-redraw flag on data mutation.
8. **Makefile** — add `tui` target, ncurses detection (prefer `ncursesw`, fallback `ncurses`), keep `main` untouched.
9. **Validation** — `make tui` builds; run through full create-squad → formation → add positions → assign players → edit stats → save → load → exit flow. Verify `make main` still produces the original CLI binary.

## Risks / Mitigations

- **ncurses not installed** — `make tui` Makefile recipe checks `ncursesw.h` then `ncurses.h`; emits clear error if neither is found.
- **Pitch render coupling** — current `formation_show()` prints directly to stdout. If extracting it is messy, wrap it in a vsnprintf buffer in `cards.c` and blit the buffer via `mvprintw`/`mvaddstr`.
- **Name underscores** — existing `_`→` ` conversion (`clean_name_string()`) is already applied by backend exec functions; TUI just calls through exec so behavior is preserved.
- **Memory** — TUI allocates view-state structs (selection index, scroll offset, back-stack array) on the heap; teeardown frees them before `endwin()`.

## Open Items (execution-agent decisions)

- Whether to capture ASCII pitch via `vsnprintf` or refactor `formation_show()` to accept a FILE* / buffer argument — left to the executor; both paths should be wrapped behind `cards.c` so the rest of the TUI stays decoupled.
- Exact key bindings for edit mode (`e` to enter, `Enter` to submit, `Esc` to cancel) — use defaults unless the user later requests customization.
