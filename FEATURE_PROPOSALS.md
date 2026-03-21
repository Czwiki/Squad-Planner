# Feature Proposals

## Quick Wins (low effort, high impact)
1. **README/UX consistency**
   - Keep default filename documentation consistent (`squad_planner_data.json`)
   - Document input length limit (`MAX_INPUT_SIZE = 200`)
2. **Clearer error messages**
   - Provide more specific guidance for invalid command usage (e.g., missing parameters)
3. **Player statistics correction**
   - Add `set`/`subtract` for stats in player sub-menu (currently only incremental `add`)

## Mid-term (moderate effort)
4. **Safer load behavior**
   - Warn before overwrite on `load`, or add an optional merge mode
5. **Player search/filter**
   - Search by name, rating range, age, etc. for larger squads
6. **Improved data maintenance**
   - Better validation/feedback for `preference` and bulk operations

## Larger strategic improvements
7. **Export capabilities**
   - CSV/TXT export for squads, formations, and statistics
8. **Undo/redo**
   - Roll back critical changes (deletions, overwrites)
9. **Comparison views**
   - Compare formations or squads within a club
