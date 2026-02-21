# Proposal: Spieler-Menü & GUI-Refactoring-Analyse

## Inhaltsverzeichnis

1. [Spieler-Menü: Konzept und Design](#1-spieler-menü-konzept-und-design)
2. [Datenmodell-Erweiterungen](#2-datenmodell-erweiterungen)
3. [Neue Befehle](#3-neue-befehle)
4. [Persistenz-Erweiterungen](#4-persistenz-erweiterungen)
5. [TUI-Mockups](#5-tui-mockups)
6. [GUI-Refactoring-Analyse](#6-gui-refactoring-analyse)

---

## 1. Spieler-Menü: Konzept und Design

### Idee

Ein neuer Kontext (Context 3: `player`) wird eingeführt, der über das Hauptmenü oder das Formation-Menü erreichbar ist. In diesem Kontext kann man einzelne Spieler aufrufen, deren Attribute bearbeiten, Freitext-Notizen hinzufügen und Statistiken einsehen bzw. pflegen.

### Menü-Struktur

```
Hauptmenü (Context 0)
├── formation  → Formation-Menü (Context 1)
├── load       → Saves-Menü (Context 2)
└── players    → Spieler-Menü (Context 3)  ← NEU
```

Im Spieler-Menü navigiert man zunächst durch eine Spielerliste und wählt dann einen Spieler aus, um ihn im Detail zu betrachten oder zu bearbeiten.

### Befehle im Spieler-Menü

| Befehl                             | Beschreibung                                           |
|------------------------------------|--------------------------------------------------------|
| `help`                             | Hilfe für das Spieler-Menü anzeigen                    |
| `listP`                            | Alle Spieler mit Kurzinfo auflisten                    |
| `showP <name>`                     | Detailansicht eines Spielers (Attribute + Notizen + Statistiken) |
| `editP <name> <attribut> <wert>`   | Spieler-Attribut bearbeiten (existiert bereits!)       |
| `note <name> <text...>`            | Notiz zu einem Spieler hinzufügen                      |
| `clearNotes <name>`                | Alle Notizen eines Spielers löschen                    |
| `stat <name> <saison> <key> <wert>`| Statistik-Wert setzen (z.B. Tore, Assists)             |
| `showStats <name>`                 | Alle Statistiken eines Spielers anzeigen               |
| `search <text>`                    | Spieler nach Name durchsuchen                          |
| `back`                             | Zurück zum Hauptmenü                                   |

---

## 2. Datenmodell-Erweiterungen

### Aktuelle `player`-Struktur

```c
typedef struct player {
    char* name;
    int age;
    int overall_rating;
    int potential_rating;
    int own_rating;
    struct player* next;
} player;
```

### Vorgeschlagene Erweiterung

```c
/* Neue Struktur: Notizen als verkettete Liste */
typedef struct player_note {
    char* text;                   /* Freitext-Notiz */
    char* timestamp;              /* Zeitstempel (ISO 8601) */
    struct player_note* next;
} player_note;

/* Neue Struktur: Saisonstatistiken */
typedef struct player_stat {
    char* season;                 /* z.B. "2024/25" */
    int games;                    /* Spiele */
    int goals;                    /* Tore */
    int assists;                  /* Assists */
    int yellow_cards;             /* Gelbe Karten */
    int red_cards;                /* Rote Karten */
    int minutes_played;           /* Gespielte Minuten */
    int rating_avg;               /* Durchschnittsbewertung (0-100) */
    struct player_stat* next;     /* Nächste Saison */
} player_stat;

/* Erweiterte player-Struktur */
typedef struct player {
    char* name;
    int age;
    int overall_rating;
    int potential_rating;
    int own_rating;

    /* NEU: Notizen */
    player_note* notes;           /* Verkettete Liste von Notizen */

    /* NEU: Statistiken */
    player_stat* stats;           /* Verkettete Liste von Saisonstatistiken */

    struct player* next;
} player;
```

### Begründung der Designentscheidungen

- **Notizen als verkettete Liste**: Ermöglicht beliebig viele Notizen pro Spieler, chronologisch sortiert. Einfach erweiterbar.
- **Statistiken pro Saison**: Erlaubt historische Vergleiche. Die Struktur deckt die wichtigsten Fußballstatistiken ab und kann leicht um weitere Felder ergänzt werden.
- **Zeitstempel bei Notizen**: Ermöglicht späteres Sortieren und gibt Kontext, wann die Notiz erstellt wurde.

---

## 3. Neue Befehle

### 3.1 `listP` – Spieler auflisten

```
squad-planner - players> listP

Spieler (3 gesamt):
  Name             Alter  OVR  POT  OWN
  ─────────────────────────────────────────
  Messi            36     88   88   95
  Lewandowski      35     87   87   90
  Ter Stegen       31     89   89   88
```

### 3.2 `showP <name>` – Spieler-Detailansicht

```
squad-planner - players> showP Messi

╔══════════════════════════════════════════╗
║  SPIELER: Messi                          ║
╠══════════════════════════════════════════╣
║  Alter:       36                         ║
║  Overall:     88                         ║
║  Potential:   88                         ║
║  Eigene Bew.: 95                         ║
╠══════════════════════════════════════════╣
║  POSITIONEN:                             ║
║  - RS  in Formation '4-3-3'             ║
║  - ST  in Formation '4-4-2'             ║
╠══════════════════════════════════════════╣
║  NOTIZEN:                                ║
║  [2025-01-15] Hervorragend im Dribbling  ║
║  [2025-02-01] Leichte Verletzung am Knie ║
╠══════════════════════════════════════════╣
║  STATISTIKEN (2024/25):                  ║
║  Spiele: 28  Tore: 22  Assists: 14      ║
║  Gelb: 3  Rot: 0  Minuten: 2340         ║
║  Ø-Bewertung: 91                         ║
╚══════════════════════════════════════════╝
```

### 3.3 `note <name> <text...>` – Notiz hinzufügen

```
squad-planner - players> note Messi Hervorragend im Dribbling
Notiz hinzugefügt für 'Messi'.
```

### 3.4 `stat <name> <saison> <key> <wert>` – Statistik setzen

```
squad-planner - players> stat Messi 2024/25 goals 22
Statistik aktualisiert: Messi - 2024/25 - goals = 22

squad-planner - players> stat Messi 2024/25 assists 14
Statistik aktualisiert: Messi - 2024/25 - assists = 14
```

Gültige Schlüssel: `games`, `goals`, `assists`, `yellow`, `red`, `minutes`, `rating`

### 3.5 `search <text>` – Spieler suchen

```
squad-planner - players> search Mes

Suchergebnisse für 'Mes':
  - Messi (OVR: 88, Alter: 36)
```

---

## 4. Persistenz-Erweiterungen

### Erweitertes JSON-Format

```json
{
  "players": [
    {
      "name": "Messi",
      "age": 36,
      "overall": 88,
      "potential": 88,
      "own_rating": 95,
      "notes": [
        {
          "text": "Hervorragend im Dribbling",
          "timestamp": "2025-01-15T14:30:00"
        },
        {
          "text": "Leichte Verletzung am Knie",
          "timestamp": "2025-02-01T09:00:00"
        }
      ],
      "stats": [
        {
          "season": "2024/25",
          "games": 28,
          "goals": 22,
          "assists": 14,
          "yellow_cards": 3,
          "red_cards": 0,
          "minutes_played": 2340,
          "rating_avg": 91
        }
      ]
    }
  ],
  "formations": [...]
}
```

### Abwärtskompatibilität

Die Deserialisierung muss prüfen, ob die neuen Felder (`notes`, `stats`) im JSON vorhanden sind. Falls nicht (alte Speicherdateien), werden leere Listen angenommen. So bleibt die Kompatibilität mit bestehenden `.json`-Dateien erhalten:

```c
/* Beim Laden: optionale Felder prüfen */
const cJSON* j_notes = cJSON_GetObjectItemCaseSensitive(p_obj, "notes");
if (cJSON_IsArray(j_notes)) {
    /* Notizen deserialisieren */
} /* sonst: notes bleibt NULL → keine Notizen */
```

---

## 5. TUI-Mockups

### Hauptmenü (erweitert)

```
squad-planner - main> help

Verfügbare Befehle:
  formation  - Formationsplanung öffnen
  players    - Spielerverwaltung öffnen      ← NEU
  load       - Speicherverwaltung öffnen
  help       - Diese Hilfe anzeigen
  exit       - Programm beenden
```

### Spieler-Menü Prompt

```
squad-planner - players> _
```

### Navigation innerhalb einer Session

```
squad-planner - main> players
squad-planner - players> listP
  Name             Alter  OVR  POT  OWN
  ─────────────────────────────────────────
  Messi            36     88   88   95
  Lewandowski      35     87   87   90

squad-planner - players> showP Messi
  [Detailansicht wie oben]

squad-planner - players> note Messi Formkurve steigend
  Notiz hinzugefügt für 'Messi'.

squad-planner - players> stat Messi 2024/25 goals 25
  Statistik aktualisiert: Messi - 2024/25 - goals = 25

squad-planner - players> back
squad-planner - main>
```

---

## 6. GUI-Refactoring-Analyse

### 6.1 Ist-Zustand

Das Projekt ist aktuell ein textbasiertes CLI/TUI in reinem C, das über `stdin`/`stdout` interagiert. Es gibt keine Abhängigkeit von Terminal-Bibliotheken (kein ncurses, kein termios) – es nutzt ausschließlich `printf`/`fgets`. Die Architektur ist bereits sauber in Module getrennt (Parser → Executor → Domain-Logik → Persistenz).

### 6.2 Aufwandsschätzung

| Aspekt | Geschätzter Aufwand |
|--------|-------------------|
| GUI-Framework einbinden und Build-System anpassen | 1–2 Tage |
| Hauptfenster mit Menüleiste / Sidebar | 2–3 Tage |
| Formation-Ansicht (taktische Feldansicht als GUI) | 3–5 Tage |
| Spieler-Formulare (Erstellen/Bearbeiten/Notizen) | 2–3 Tage |
| Drag-and-Drop für Spieler auf Positionen | 3–5 Tage |
| Speichern/Laden-Dialog | 1 Tag |
| Anbindung der bestehenden Domain-Logik | 2–3 Tage |
| Testen und Bugfixing | 3–5 Tage |
| **Gesamt** | **~17–27 Tage** (für eine Person) |

**Fazit**: Der Aufwand ist **mittel bis hoch**. Die gute Nachricht: Die Domain-Logik (`formation.c`, `persistence.c`) kann größtenteils wiederverwendet werden. Nur die Eingabe- und Ausgabeschicht (Parser/Exec/Main) müsste ersetzt werden.

### 6.3 Bibliotheksempfehlungen

#### Option A: **raylib** ⭐ (Empfehlung)

| Eigenschaft | Bewertung |
|-------------|-----------|
| Sprache | C (passt perfekt zum Projekt) |
| Lizenz | zlib/libpng (frei, auch kommerziell) |
| Plattformen | Windows, macOS, Linux, Web (Emscripten), RPi |
| Lernkurve | Niedrig – einfache API, exzellente Dokumentation |
| GUI-Widgets | Keine eingebauten – man nutzt [raygui](https://github.com/raysan5/raygui) als Ergänzung |
| Besonderheit | Ideal für grafische Darstellungen (Taktiktafel!) |

**Vorteile**: Perfekt für C-Projekte, sehr einfache Integration (Single-Header möglich), hervorragend für die grafische Darstellung eines Fußballfelds mit Drag-and-Drop. Minimale Abhängigkeiten.

**Nachteile**: Keine nativen OS-Widgets (Buttons/Textfelder sehen nicht systemtypisch aus). Für komplexe Formulare muss man raygui nutzen oder eigene Widgets bauen.

#### Option B: **GTK 4**

| Eigenschaft | Bewertung |
|-------------|-----------|
| Sprache | C (native API) |
| Lizenz | LGPL |
| Plattformen | Linux (nativ), Windows (via MSYS2), macOS (eingeschränkt) |
| Lernkurve | Mittel bis hoch |
| GUI-Widgets | Vollständig – Buttons, Listen, Eingabefelder, Dialoge |
| Besonderheit | Professionelle, native Desktop-Anwendung |

**Vorteile**: Vollwertiges Widget-Toolkit, native Dateisystem-Dialoge, professionelles Look-and-Feel. Perfekt für Formulare (Spieler bearbeiten, Notizen).

**Nachteile**: Deutlich komplexer als raylib, schwierigere Windows-Installation. Auf macOS nicht ideal. Viele Abhängigkeiten bei der Installation.

#### Option C: **SDL2 + Dear ImGui**

| Eigenschaft | Bewertung |
|-------------|-----------|
| Sprache | SDL2 = C, Dear ImGui = C++ (C-Wrapper: cimgui) |
| Lizenz | SDL2: zlib, ImGui: MIT |
| Plattformen | Windows, macOS, Linux, Mobile |
| Lernkurve | Niedrig (ImGui), Mittel (SDL2-Setup) |
| GUI-Widgets | Immediate-Mode GUI – Buttons, Tabellen, Inputs, Plots |
| Besonderheit | Schnelles Prototyping, ideal für Tools |

**Vorteile**: Dear ImGui ist extrem produktiv – Tabellen, Formulare und interaktive Elemente lassen sich in wenigen Zeilen erstellen. Perfekt für ein Tool wie den Squad-Planner.

**Nachteile**: Dear ImGui ist C++, für reines C benötigt man den `cimgui`-Wrapper. Look-and-Feel ist nicht nativ, sondern eher wie ein Entwickler-Tool.

#### Option D: **ncurses** (TUI-Verbesserung statt GUI)

| Eigenschaft | Bewertung |
|-------------|-----------|
| Sprache | C |
| Lizenz | MIT |
| Plattformen | Linux, macOS (nativ), Windows (PDCurses) |
| Lernkurve | Niedrig bis mittel |
| GUI-Widgets | Text-basiert: Fenster, Menüs, Farben, Maus-Support |
| Besonderheit | Bleibt im Terminal, aber deutlich verbessertes UI |

**Vorteile**: Minimaler Refactoring-Aufwand (kein Wechsel von Terminal zu Fenster-System). Farbige Darstellung, Fensteraufteilung, Tastaturnavigation. Läuft überall, wo ein Terminal verfügbar ist.

**Nachteile**: Kein grafisches Fußballfeld möglich, kein Drag-and-Drop, eingeschränkte Maus-Unterstützung.

### 6.4 Empfehlung

Für dieses Projekt empfehle ich einen **zweistufigen Ansatz**:

1. **Kurzfristig**: Das Spieler-Menü (wie oben beschrieben) als TUI implementieren – passt zur aktuellen Architektur, geringer Aufwand.
2. **Mittelfristig**: Bei GUI-Wunsch **raylib + raygui** verwenden. Die Kombination ist ideal für ein C-Projekt und ermöglicht eine grafische Taktiktafel mit Drag-and-Drop.

Falls ein professionelleres Desktop-Programm gewünscht ist (mit nativen Dateisystem-Dialogen, Menüleisten usw.), wäre **GTK 4** die bessere Wahl – allerdings mit deutlich mehr Aufwand und eingeschränkter Windows/macOS-Kompatibilität.

### 6.5 Auswirkungen auf die Betriebssystem-Kompatibilität

| Bibliothek | Linux | Windows | macOS | Web |
|------------|-------|---------|-------|-----|
| Aktuell (reines C) | ✅ Nativ | ✅ MinGW/MSVC | ✅ Nativ | ❌ |
| raylib | ✅ | ✅ | ✅ | ✅ (Emscripten) |
| GTK 4 | ✅ Nativ | ⚠️ MSYS2 nötig | ⚠️ Eingeschränkt | ❌ |
| SDL2 + ImGui | ✅ | ✅ | ✅ | ✅ (Emscripten) |
| ncurses | ✅ Nativ | ⚠️ PDCurses | ✅ Nativ | ❌ |

**Zusammenfassung der Kompatibilitätsauswirkungen**:

- **raylib** und **SDL2**: Breite plattformübergreifende Unterstützung, sogar Web-Export möglich. Kaum Kompatibilitätseinbußen gegenüber dem aktuellen Stand.
- **GTK 4**: Stark Linux-zentriert. Windows-Support erfordert MSYS2-Toolchain. macOS-Support ist möglich, aber nicht erstklassig. Die Kompatibilität würde sich gegenüber dem Ist-Zustand **verschlechtern**.
- **ncurses**: Gute Kompatibilität, auf Windows jedoch PDCurses als Ersatz nötig (API-kompatibel, aber separates Paket).

### 6.6 Architektonische Empfehlungen für den GUI-Umbau

Unabhängig von der gewählten Bibliothek sollte vor dem Umbau:

1. **Domain-Logik isolieren**: `formation.c` hat bereits eine saubere API. Diese sollte als "Backend" beibehalten werden – sowohl TUI als auch GUI rufen dieselben Funktionen auf.
2. **Parser/Exec durch Event-Handler ersetzen**: In einer GUI gibt es keine Texteingabe → der Parser wird durch Button-Callbacks und Formular-Events ersetzt.
3. **Main-Loop ersetzen**: Der `fgets`-Loop in `main.c` wird durch die GUI-Event-Loop der jeweiligen Bibliothek ersetzt (z.B. `while (!WindowShouldClose())` bei raylib).
4. **Persistenz beibehalten**: Das JSON-basierte Speichersystem bleibt unverändert – es ist UI-unabhängig.

---

*Dieses Dokument ist ein Entwurf und dient als Diskussionsgrundlage. Kein Code wurde im Rahmen dieses Vorschlags geändert.*
