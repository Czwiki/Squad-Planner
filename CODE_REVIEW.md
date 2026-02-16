# Code Review: Squad-Planner

## Übersicht

Diese Code-Review analysiert den aktuellen Stand des Squad-Planner Projekts und identifiziert:
1. Fehlende Features
2. Potenzielle Bugs
3. Memory Leaks
4. Empfehlungen für persistente Speicherung

---

## 1. Potenzielle Bugs und Probleme

### 1.1 Memory Leaks

#### **Kritisch: Keine Freigabe von globalen Listen beim Programm-Ende**
- **Datei**: `main.c` / `src/formation/formation.c`
- **Problem**: Die globalen Listen `player_head` und `formation_head` werden beim Beenden des Programms nicht freigegeben.
- **Betroffener Code**: 
  ```c
  // main.c Zeile 72-75 und Zeile 149
  if (strcmp(line, "exit") == 0 ) {
      free(cmd);  // Nur cmd wird freigegeben!
      exit(EXIT_SUCCESS);
  }
  // Aber player_head, formation_head und alle positions werden NIE freigegeben
  ```
- **Fix**: Eine `cleanup()` Funktion hinzufügen, die alle Ressourcen freigibt.

#### **Kritisch: Positionen werden bei formation-Löschung nicht freigegeben**
- **Datei**: `src/formation/formation.c`
- **Problem**: Es gibt keine Funktion zum Löschen einer Formation. Wenn `formation_head` und alle Formationen verloren gehen, bleiben alle `position*` im Speicher.
- **Empfehlung**: `delete_formation()` Funktion implementieren.

#### **Memory Leak in `parser.c` bei Fehlerbehandlung**
- **Datei**: `src/parser/parser.c`, Zeile 238-246
- **Problem**: Bei Memory-Allocation-Fehlern werden nicht alle vorher allokierten Ressourcen freigegeben.
```c
// Zeile 230-246: Bei realloc-Fehler wird temp freigegeben, aber nicht cmd->name!
if (!temp) {
    free(buffer);
    free(cmd->name);  // ✓ Richtig hier
    return SP_ERR_MEMORY;
}
// ... später in Zeile 250-266:
if (!temp) {
    free(buffer);
    free(temp);  // temp freigeben
    return SP_ERR_MEMORY;  // ABER: cmd->name und cmd->options werden nicht freigegeben!
}
```

#### **Memory Leak in `formation.c` - `remove_player_from_position()`**
- **Datei**: `src/formation/formation.c`, Zeile 605-638
- **Problem**: Die while-Schleife hat `i++` nicht implementiert - es wird eine Endlosschleife!
```c
int i = 1;
while (args[i] != NULL) {
    // ... Processing ...
    // FEHLT: i++;  <- Dies fehlt am Ende der Schleife!
}
```

### 1.2 Logik-Bugs

#### **Bug: `remove_position_from_formation()` beginnt bei args[1] statt args[0]**
- **Datei**: `src/formation/formation.c`, Zeile 760-779
- **Problem**: Die Schleife beginnt bei `i = 1` statt `i = 0`, wodurch das erste Argument übersprungen wird.
```c
int i = 1;  // FALSCH - sollte 0 sein!
while (args[i] != NULL) {
    // args[0] wird nie verarbeitet!
```
- **Fix**: `int i = 0;`

#### **Bug: `sanity_check_and_help()` gibt 1 für Help zurück, aber `list_formations()` ruft es mit falschen Parametern auf**
- **Datei**: `src/formation/formation.c`, Zeile 541
- **Problem**: Der Aufruf ist: `sanity_check_and_help(NULL, options, 0, 1, 1, 0, ...)`
  - `min_args = 1` aber `args = NULL`, was immer `SP_ERR_WRONG_USAGE` zurückgibt wenn keine Options gegeben sind
- **Fix**: `min_args` sollte 0 sein für `listf`

#### **Bug: NULL-Pointer-Dereferenzierung in `execute_command()`**
- **Datei**: `src/exec/exec.c`, Zeile 148-150
- **Problem**: Die NULL-Prüfung kommt NACH dem Zugriff auf `cmd->id`:
```c
int execute_command(command* cmd, int context) {
    printf("Executing command ID %d in context %d\n", cmd->id, context);  // Zugriff vor NULL-Check!
    if (!cmd) {  // NULL-Check kommt zu spät!
        return -1;
    }
```
- **Fix**: NULL-Check zuerst durchführen.

### 1.3 Weitere Probleme

#### **Integer Overflow in `strtol()` Handling**
- **Datei**: `src/formation/formation.c`, Zeile 379-389
- **Problem**: `errno == ERANGE` wird geprüft, aber `strtol()` setzt auch `errno` auf andere Werte bei Fehlern. Außerdem sollte man den `endptr` prüfen.
```c
errno = 0;
int age = strtol(args[1], NULL, 10);  // NULL als endptr - keine Validierung ob alles geparst wurde!
```
- **Fix**: `endptr` verwenden und auf komplette Konvertierung prüfen.

#### **Potential Buffer Overflow bei langen Spielernamen**
- **Datei**: `src/formation/formation.c`
- **Problem**: `strdup()` wird ohne Längenbegrenzung verwendet. Sehr lange Namen könnten zu Speicherproblemen führen.

#### **`snprintf` Rückgabewert-Check ist falsch**
- **Datei**: `main.c`, Zeile 101-103
- **Problem**: `snprintf` gibt bei Fehler einen negativen Wert zurück, aber auch bei Truncation einen Wert >= Buffergröße.
```c
if (snprintf(prompt, 60, "squad-planner - main> ") < 0) {
```
- **Fix**: Prüfe auch `>= sizeof(prompt)` für Truncation.

---

## 2. Fehlende Features

### 2.1 Kritisch (für Nutzbarkeit)

1. **Persistente Speicherung (Save/Load)**
   - Status: Nur Platzhalter implementiert
   - Dateien: `exec.c` Zeile 98-100, 124-126
   - Impact: Alle Daten gehen beim Beenden verloren

2. **Formation löschen**
   - Status: Nicht implementiert
   - Impact: Einmal erstellte Formationen können nicht entfernt werden

3. **Spieler löschen**
   - Status: Nicht implementiert
   - Impact: Einmal erstellte Spieler können nicht entfernt werden

4. **Spieler bearbeiten**
   - Status: Nicht implementiert
   - Impact: Fehlerhafte Eingaben können nicht korrigiert werden

### 2.2 Wünschenswert

1. **Duplikat-Check für Spielernamen**
   - Aktuell können mehrere Spieler mit gleichem Namen erstellt werden

2. **Undo/Redo Funktionalität**
   - Keine Möglichkeit, Aktionen rückgängig zu machen

3. **Formation kopieren/duplizieren**
   - Schnelles Erstellen ähnlicher Formationen

4. **Spieler-Statistiken anzeigen**
   - Detailansicht eines Spielers fehlt

5. **Validierung bei `preference` Befehl**
   - Es wird nicht geprüft, ob alle Spieler in der Liste angegeben wurden

---

## 3. Empfehlungen für Persistente Speicherung

### 3.1 Empfohlener Ansatz: JSON

**Warum JSON?**
- Strukturierte Daten (nested objects für Formationen > Positionen > Spieler)
- Menschenlesbar und editierbar
- Standard-Bibliotheken verfügbar (cJSON ist leichtgewichtig)
- Gute Erweiterbarkeit für zukünftige Features

**Beispiel-Struktur:**
```json
{
  "players": [
    {
      "name": "Messi",
      "age": 36,
      "overall_rating": 88,
      "potential_rating": 88,
      "own_rating": 95
    }
  ],
  "formations": [
    {
      "name": "4-3-3",
      "positions": [
        {
          "id": 0,
          "name": "GK",
          "players": ["Ter Stegen"]
        },
        {
          "id": 22,
          "name": "ST",
          "players": ["Messi", "Lewandowski"]
        }
      ]
    }
  ]
}
```

**Empfohlene Bibliothek**: [cJSON](https://github.com/DaveGamble/cJSON) - Single-file, MIT-lizensiert

### 3.2 Alternative: CSV (einfacher)

Zwei getrennte Dateien:

**players.csv:**
```csv
name,age,overall,potential,own_rating
Messi,36,88,88,95
Ronaldo,38,85,85,90
```

**formations.csv:**
```csv
formation_name,position_id,player_rank,player_name
4-3-3,0,1,Ter_Stegen
4-3-3,22,1,Messi
4-3-3,22,2,Lewandowski
```

**Vorteile CSV:**
- Keine externe Bibliothek nötig
- Einfach zu parsen mit `strtok()`
- Excel-kompatibel

**Nachteile CSV:**
- Schwieriger bei Sonderzeichen in Namen
- Weniger flexibel

### 3.3 Implementation Empfehlung

```c
// src/persistence/persistence.h
#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#define DEFAULT_SAVE_FILE "squad_planner_data.json"

int save_all_data(const char* filename);
int load_all_data(const char* filename);

#endif

// Workflow:
// 1. Beim Start: load_all_data(DEFAULT_SAVE_FILE) versuchen
// 2. "save" Befehl: save_all_data(DEFAULT_SAVE_FILE) 
// 3. Beim sauberen Exit: Automatisch speichern (optional)
```

---

## 4. Code-Qualität Empfehlungen

### 4.1 Cleanup-Funktion implementieren

```c
// main.c - Am Ende vor exit:
void cleanup_all(void) {
    // 1. Alle Formationen freigeben
    cleanup_formations();  // Neue Funktion in formation.c
    
    // 2. Alle Spieler freigeben  
    cleanup_players();     // Neue Funktion in formation.c
}

// formation.c:
void cleanup_formations(void) {
    formation* current = formation_head;
    while (current != NULL) {
        formation* next = current->next;
        // Positionen freigeben
        for (int i = 0; i < 24; i++) {
            if (current->map_of_positions[i]) {
                free(current->map_of_positions[i]->list_of_players);
                free(current->map_of_positions[i]);
            }
        }
        free(current->name);
        free(current);
        current = next;
    }
    formation_head = NULL;
    current_formation = NULL;
}

void cleanup_players(void) {
    player* current = player_head;
    while (current != NULL) {
        player* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    player_head = NULL;
}
```

### 4.2 Const-Correctness verbessern

```c
// Besser: const für read-only Parameter
int parse_position_string(const char* position_str);  // const hinzufügen
player* find_player_by_name(const char* name);        // const hinzufügen
```

### 4.3 Unit Tests hinzufügen

Empfehlung: CUnit oder einfach ein test.c mit Assertions:
```c
// tests/test_formation.c
void test_new_formation(void) {
    char* args[] = {"TestFormation", NULL};
    int result = new_formation(args, NULL);
    assert(result == SP_SUCCESS);
    // Cleanup...
}
```

---

## 5. Zusammenfassung

### Priorität 1 (Kritisch - Bugs):
1. ✗ Memory Leaks bei Programmende
2. ✗ Endlosschleife in `remove_player_from_position()`  
3. ✗ NULL-Pointer-Check in `execute_command()` zu spät
4. ✗ `remove_position_from_formation()` überspringt erstes Argument
5. ✗ `list_formations()` - falscher `min_args` Parameter

### Priorität 2 (Features):
1. ✗ Persistente Speicherung (Save/Load)
2. ✗ Formation löschen
3. ✗ Spieler löschen/bearbeiten

### Priorität 3 (Verbesserungen):
1. ✗ Cleanup-Funktionen
2. ✗ Input-Validierung verbessern
3. ✗ Unit Tests

---

*Review erstellt am: 2026-02-16*
