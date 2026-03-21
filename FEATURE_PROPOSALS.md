# Feature Proposals

## Quick Wins (low effort, high impact)
1. **Player statistics completion**
   - Add `set` for statistics in player sub-menu (currently `add`/`subtract`)
2. **Clearer wrong-usage details**
   - Keep generic error short but include command-specific guidance in each `--help` page
3. **Input robustness**
   - Consider graceful handling for oversized input by showing command-specific hints per context

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
