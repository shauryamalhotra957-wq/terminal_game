# Testing & Quality Assurance Guide

This document describes the testing protocols, automated regression test suite, manual verification checklist, and packaging verification procedures for the C++ Snake Game.

---

## 1. Automated Test Suite

An automated headless test suite is implemented in [`tests.cpp`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/tests.cpp). It leverages the abstract `IConsole` interface and a `MockConsole` implementation to execute deterministic unit and integration tests without requiring manual keyboard interaction or a terminal emulator.

### Running the Automated Tests

Using `make`:
```bash
make test
```

Or compiling directly with G++:
```bash
g++ -std=c++17 -Wall -Wextra tests.cpp -o tests.exe
./tests.exe
```

### What the Automated Suite Verifies (267 Assertions)
1. **Abstraction & Polymorphism:**
   - Calls to `init()`, `moveCursorToTop()`, `isKeyAvailable()`, `readKey()`, and `restore()` through the `IConsole*` base pointer correctly dispatch to the derived mock class.
2. **Map Loading & Validation:**
   - Standard 30×10 map dimensions are preserved.
   - Boundaries are verified as walls (`#`) and interior cells as open spaces.
   - Out-of-bounds coordinates return true for `isWall()`.
3. **Resilient Resource Loading:**
   - Non-existent map file gracefully falls back to the embedded default 30×10 map.
   - Corrupted map file (wrong row count or improper line length) is rejected and safe default map is loaded without crashing.
4. **Snake Movement & Encapsulation:**
   - Initial snake state (length 3, head at `{10, 5}`, direction `'d'`).
   - Movement in all four directions (`W`, `A`, `S`, `D`).
   - Reversal prevention: immediate 180-degree reverses (`D` to `A`, `W` to `S`, etc.) are rejected.
5. **Collision Detection:**
   - Moving the snake head into boundary walls returns `false` (wall collision).
   - Moving the snake head onto its own body segments returns `true` for `hitSelf()` (self collision).
6. **Food Logic:**
   - Food respawning logic verifies that food never spawns on a wall or on any snake segment over 50 randomized iterations.
7. **Game Integration & Dependency Injection:**
   - Feeding input keys through the mock console verifies game loop transitions.
   - Eating food increases score by 1 and grows the snake.
   - Pressing `Q` cleanly terminates the game loop and triggers console restoration.
   - Restarting with `resetGame()` resets score, clears game over state, and restores snake segments.

---

## 2. Manual Verification Checklist

Before release or demonstration to the professor, verify the interactive game in a Windows console terminal:

| Item | Verification Step | Expected Behavior |
| :--- | :--- | :--- |
| **Startup** | Launch `main.exe` or `TerminalGame.exe` | Map renders with `#` border, snake `@oo`, and food `*`. Cursor is hidden. |
| **Movement** | Press `W`, `A`, `S`, `D` | Snake changes direction smoothly. |
| **Reverse Rejection** | Press `A` while moving right (`D`) | Input is ignored; snake continues moving right. |
| **Eating Food** | Guide snake to eat food `*` | Score counter increments; snake gains 1 segment; new food spawns in open space. |
| **Wall Collision** | Steer snake into any boundary wall | Game halts; Game Over screen displays with final score. |
| **Self Collision** | Grow snake to length 5+ and steer into body | Game Over screen displays immediately. |
| **Restart** | Press `R` on Game Over screen | Game resets: score resets to 0, snake resets to length 3 at `{10,5}`, new food spawns. |
| **Clean Exit** | Press `Q` during game or Game Over screen | Game exits gracefully to shell prompt, and cursor visibility is restored. |

---

## 3. Packaging & Portability Verification

To verify that the release build runs on another Windows computer without requiring development tools:

1. **Working Directory Independence:**
   - Execute the binary from a different directory:
     ```powershell
     & "dist\TerminalGame-Windows-x64\TerminalGame.exe"
     ```
   - Verify that the game starts without errors.
2. **Missing Map Resilience:**
   - Temporarily rename `map.txt` in the folder and launch `TerminalGame.exe`.
   - Verify the game launches using the safe embedded default 30×10 map.
3. **Standalone Runtime Check:**
   - Run dependency inspection on `TerminalGame.exe`:
     ```powershell
     objdump -p dist\TerminalGame-Windows-x64\TerminalGame.exe | Select-String -Pattern "DLL Name"
     ```
   - Verify that the only dependencies are standard Windows system DLLs (`KERNEL32.dll` and `api-ms-win-crt-*.dll`).
   - Confirm there are **no dependencies** on `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, or `libwinpthread-1.dll`.
4. **ZIP Extraction Test:**
   - Extract `dist/TerminalGame-Windows-x64.zip` into an empty folder.
   - Confirm that double-clicking `TerminalGame.exe` inside the extracted folder starts the game immediately.
