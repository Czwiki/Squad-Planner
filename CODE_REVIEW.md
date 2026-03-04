# Code Review: Squad-Planner

## Übersicht

Diese Code-Review analysiert den aktuellen Stand des Squad-Planner Projekts und identifiziert:
1. Fehlende Features
2. Potenzielle Bugs
3. Memory Leaks
4. Empfehlungen für persistente Speicherung

---

## 2. Fehlende Features

4. **Spieler bearbeiten**
   - Status: Nicht implementiert
   - Impact: Fehlerhafte Eingaben können nicht korrigiert werden

### 2.2 Wünschenswert

2. **Undo/Redo Funktionalität**
   - Keine Möglichkeit, Aktionen rückgängig zu machen

3. **Formation kopieren/duplizieren**
   - Schnelles Erstellen ähnlicher Formationen

4. **Spieler-Statistiken anzeigen**
   - Detailansicht eines Spielers fehlt

5. **Validierung bei `preference` Befehl**
   - Es wird nicht geprüft, ob alle Spieler in der Liste angegeben wurden

---

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

### Priorität 3 (Verbesserungen):
2. ✗ Input-Validierung verbessern

### 6.2 Aufgabenverteilung Parser ↔ Executor

**Aktuelle Verteilung:**
- **Parser**: Befehlserkennung, Kontextwechsel-Bestimmung, Token-Extraktion
- **Executor**: Befehlsdispatch, Aufruf der Implementierungsfunktionen
- **Formation-Modul**: Validierung von Argumenten (Sanity-Checks, Wertebereichsprüfung)

**Empfehlungen für sauberere Trennung:**

1. **Argumentanzahl-Validierung**: Die Prüfung der Argumentanzahl (`sanity_check_and_help`)
   erfolgt aktuell in jedem Befehlshandler in `formation.c`. Diese Prüfung könnte stattdessen
   im Executor (`exec.c`) zentral erfolgen, da die erwartete Argumentanzahl pro Befehl bekannt
   ist. Dies würde die formation.c-Funktionen vereinfachen.

2. **`--help`-Option**: Die Erkennung von `--help` erfolgt in jedem Befehlshandler über
   `sanity_check_and_help()`. Dies könnte im Executor abgefangen werden, bevor der Handler
   aufgerufen wird, da `--help` für alle Befehle gleich funktioniert.

4. **`!args && !options` Check**: Jeder Befehlshandler prüft `!args && !options` am Anfang.
   Dies könnte zentral im Executor erfolgen – für Befehle die mindestens ein Argument benötigen.

---

## 7. Windows-Kompatibilität

### 7.2 Verbleibende Windows-Hinweise

1. **Makefile**: Verwendet `rm -f` (Unix). Unter Windows/MSVC muss `del /Q` verwendet werden,
   oder MinGW/MSYS2 bereitstellen, wo `rm` verfügbar ist. Ein Kommentar im Makefile weist
   darauf hin.

2. **`CC = gcc`**: Unter MSVC ist der Compiler `cl`. MinGW stellt `gcc` bereit.
   Für maximale Portabilität könnte CMake statt Make verwendet werden.

3. **`-lm` (libm)**: Wird für cJSON benötigt (`math.h`-Funktionen). Unter MSVC ist
   die Math-Bibliothek standardmäßig eingebunden, `-lm` wird nicht benötigt.

4. **Pfadtrenner**: Das Projekt verwendet keine hartcodierten Pfadtrenner (`/` vs `\`).
   Die JSON-Datei wird im aktuellen Verzeichnis gespeichert, was plattformunabhängig ist.

---

*Review erstellt am: 2026-02-16*
*Aktualisiert am: 2026-02-19 – Persistence implementiert, Windows-Compat hinzugefügt, Parser/Executor-Analyse*
