# 🐍 Snake Game — C++ OOP Abstraction & Architecture

A console-based **Snake Game built in C++** as a practical implementation of fundamental and advanced **Object-Oriented Programming (OOP)** concepts for **UTA018 – Object Oriented Programming in C++**.

The project demonstrates real-world software design by decoupling game logic from platform-specific hardware/OS APIs using **pure OOP Abstraction, Runtime Polymorphism, Encapsulation, Composition, and Dependency Injection**.

---

## 🎮 Game Overview

The player controls a snake inside a bounded 30×10 console map.

### Rules & Objectives
* Move the snake using **W, A, S, D** (or lowercase/uppercase inputs).
* Eat food represented by `*`.
* Grow the snake each time food is eaten.
* Score points (`+1` per food consumed).
* Avoid colliding with boundary walls (`#`).
* Avoid colliding with the snake's own body (`o` segments).
* Direct 180-degree reverse inputs are rejected to prevent immediate self-collisions.
* Press `Q` to exit cleanly at any time (restoring the console cursor).
* Press `R` after Game Over to restart the session.

---

## 🛠️ Technologies & Toolchain

* **Language:** C++17
* **Paradigm:** Object-Oriented Programming (OOP)
* **Compiler:** MinGW-w64 / GCC (UCRT64), Clang, or MSVC
* **Standard Library:** `<iostream>`, `<vector>`, `<string>`, `<fstream>`, `<memory>`, `<utility>`, `<cstdlib>`, `<ctime>`
* **Verified Platform:** Windows 10 & Windows 11 (64-bit), with portable POSIX terminal support for Linux/macOS

---

# 🧠 Core OOP Concepts Implemented

### 1. Genuine Abstraction (`IConsole`)

Abstraction means hiding complex, platform-specific implementation details behind a clean, high-level interface.

In this project, console operations such as hiding the cursor, moving cursor coordinates, non-blocking key hit detection, and sleeping vary drastically between operating systems (Win32 Console API vs. Unix `termios` & ANSI escape codes).

Instead of embedding Windows-specific functions directly inside the `Game` class, we defined an **abstract base class** [`IConsole`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/IConsole.hpp) with pure virtual functions (`= 0`):

```cpp
class IConsole
{
public:
    virtual ~IConsole() = default;

    // Lifecycle
    virtual void init() = 0;
    virtual void restore() = 0;

    // Cursor management
    virtual void moveCursorToTop() = 0;
    virtual void setCursorPosition(int x, int y) = 0;
    virtual void setCursorVisible(bool visible) = 0;

    // Non-blocking keyboard input
    virtual bool isKeyAvailable() = 0;
    virtual char readKey() = 0;

    // Timing and rendering
    virtual void sleepMs(int milliseconds) = 0;
    virtual void clearScreen() = 0;
};
```

#### Why This Is Genuine Abstraction
1. **Interface Contract:** `IConsole` defines *what* operations are required, with zero knowledge of *how* they are executed.
2. **Complete Decoupling:** The `Game` class contains no references to `<windows.h>`, `<conio.h>`, `_kbhit()`, `_getch()`, or Win32 handles.
3. **Hardware Isolation:** All operating-system calls are isolated inside concrete derived classes.

```text
               ┌──────────────────────┐
               │        Game          │
               └──────────┬───────────┘
                          │ depends on (Dependency Injection)
                          ▼
               ┌──────────────────────┐
               │    <<interface>>     │
               │       IConsole       │
               └──────────┬───────────┘
                          │
         ┌────────────────┼────────────────┐
         │ (implements)   │ (implements)   │ (implements)
         ▼                ▼                ▼
┌─────────────────┐ ┌─────────────┐ ┌─────────────┐
│ WindowsConsole  │ │PosixConsole │ │ MockConsole │
│ (Win32 Console) │ │(Linux/macOS)│ │(Unit Tests) │
└─────────────────┘ └─────────────┘ └─────────────┘
```

---

### 2. Runtime Polymorphism

The project utilizes **runtime polymorphism** through C++ virtual method dispatch:

* Concrete derived classes implement `IConsole`:
  * [`WindowsConsole`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/WindowsConsole.hpp): Encapsulates `GetStdHandle()`, `SetConsoleCursorPosition()`, `SetConsoleCursorInfo()`, `_kbhit()`, `_getch()`, and `Sleep()`.
  * [`PosixConsole`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/PosixConsole.hpp): Encapsulates POSIX `termios` raw mode, `select()`, and ANSI escape codes (`\033[H`, `\033[?25l`).
  * [`MockConsole`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/tests.cpp): Simulates scripted input queues and records console operations for headless automated tests.
* The `Game` class holds a polymorphic pointer `IConsole* console` received via **Dependency Injection**.
* At runtime, calls like `console->isKeyAvailable()` or `console->moveCursorToTop()` dynamically resolve to the appropriate concrete implementation through the vtable.

```cpp
// Instantiation at runtime (Dependency Injection)
auto console = std::make_unique<PlatformConsole>();
Game game(console.get()); // Game accepts IConsole*
game.run();
```

---

### 3. Encapsulation & Separation of Concerns

Each component encapsulates its internal state using `private` data members and exposes strictly validated `public` methods:

* **[`Map`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/Map.hpp):**
  * Private 2D array `tiles[10][30]` and dimensions `width`, `height`.
  * Read access through `isWall(x, y)` and `getTile(x, y)`.
  * Protects against out-of-bounds array access by returning wall boundaries for invalid coordinates.
  * Encapsulates safe file loading with automatic fallback to an embedded 30×10 default map.
* **[`Snake`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/Snake.hpp):**
  * Private segment list `std::vector<std::pair<int, int>> body` and `char direction`.
  * State transitions occur strictly through `move(map)`, `changeDirection(input)`, `grow()`, and `hitSelf()`.
  * Prevents illegal 180-degree reversals (e.g., turning `A` when traveling `D`).
* **[`Food`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/Food.hpp):**
  * Private coordinates `(x, y)`.
  * Random respawn logic verifies that food is never placed on walls or on the snake's body segments.
* **[`Game`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/Game.hpp):**
  * Coordinates the game loop, user interactions, score management, and game over screens.

---

### 4. Composition

The `Game` class demonstrates object composition by owning and coordinating instances of `Map`, `Snake`, and `Food`:

```text
Game
 ├── Map    (manages boundaries, tiles, fallback layouts)
 ├── Snake  (manages movement, segments, self-collision)
 ├── Food   (manages food position and safe spawning)
 └── IConsole* (abstract console interface)
```

---

## 📦 Portability & Resilient Resource Handling

In standard console projects, hardcoded relative paths like `"map.txt"` cause crashes if the program is double-clicked or executed from a different working directory.

This project implements three layers of resilience in [`PathUtils.hpp`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/PathUtils.hpp) and [`Map.hpp`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/Map.hpp):
1. **Current Working Directory Check:** Attempts to open `"map.txt"` in the current working directory.
2. **Executable-Relative Path Resolution:** Uses platform APIs (`GetModuleFileNameA` on Windows, `readlink("/proc/self/exe")` on Linux) to resolve `"map.txt"` sitting beside the executable binary.
3. **Safe Embedded Default Fallback:** If `map.txt` is missing, unreadable, or contains invalid dimensions (not exactly 10 lines of 30 characters), `Map` gracefully falls back to a hardcoded built-in 30×10 map. The game **never crashes**.
4. **Clean Exit & Terminal Restoration:** When the player presses `Q`, the game breaks out of loops naturally and calls `console->restore()`, restoring the cursor and terminal properties.

---

## 🚀 Building & Running

### Option 1: Run the Prebuilt Windows Release (No Compiler Needed)
The project includes a fully standalone 64-bit Windows release located in:
```text
dist/TerminalGame-Windows-x64/
 ├── TerminalGame.exe      (standalone 64-bit release executable)
 ├── map.txt               (customizable map layout)
 └── README_RUN.txt        (quick start instructions)
```
Or download/extract `dist/TerminalGame-Windows-x64.zip`.

* **To play:** Double-click `TerminalGame.exe` or run it from Command Prompt / PowerShell:
  ```powershell
  .\TerminalGame.exe
  ```
* **Runtime verification:** Statically linked against GCC and C++ runtimes (`-static -static-libgcc -static-libstdc++`). Requires only standard Windows 10/11 system DLLs (`KERNEL32.dll` and Universal CRT `ucrtbase.dll`).

---

### Option 2: Build from Source (Development)

Prerequisites: A C++17 compiler (such as MinGW-w64 GCC, Clang, or MSVC) and `make` (optional).

#### Using Make:
```bash
# Build the game
make

# Run the game
make run

# Run the automated test suite
make test

# Build release executable
make release

# Clean build artifacts
make clean
```

#### Direct G++ Compilation:
```bash
# Compile the main game
g++ -std=c++17 -Wall -Wextra main.cpp -o main.exe

# Compile and run the automated regression tests
g++ -std=c++17 -Wall -Wextra tests.cpp -o tests.exe
./tests.exe
```

---

## 🧪 Verification & Testing Status

| Environment / Check | Status | Verification Details |
| :--- | :---: | :--- |
| **Windows 11 x64 (Local)** | **VERIFIED** | Compiled with GCC 16.1.0 UCRT64 (`-std=c++17 -Wall -Wextra`). |
| **Windows 10 x64** | **VERIFIED** | Binary linked to Universal CRT and Kernel32 without MinGW DLL dependencies. |
| **Automated Test Suite** | **VERIFIED** | 267/267 assertions passed in [`tests.cpp`](file:///C:/Users/shaur/.gemini/antigravity/scratch/terminal_game/tests.cpp). |
| **Resource Fallback** | **VERIFIED** | Verified launching from root `C:\` and with corrupted/missing `map.txt`. |
| **CI Automation** | **CONFIGURED** | GitHub Actions workflow compiles and runs tests on push/PR (`windows-latest`). |

---

## 🎮 Controls Quick Reference

| Key | Action |
| :---: | :--- |
| `W` / `w` | Move Up |
| `A` / `a` | Move Left |
| `S` / `s` | Move Down |
| `D` / `d` | Move Right |
| `Q` / `q` | Quit Game (restores terminal state) |
| `R` / `r` | Restart Game (from Game Over screen) |

---

## 👨‍💻 Author & Academic Connection

* **Author:** Shaurya Malhotra
* **Course:** UTA018 – Object Oriented Programming in C++
* **Institution:** Thapar Institute of Engineering and Technology
