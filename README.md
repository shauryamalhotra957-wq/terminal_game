# 🐍 Snake Game — C++ OOP Project

A console-based **Snake Game built in C++** as a practical implementation of the fundamental concepts of **Object-Oriented Programming (OOP)** covered in **UTA018 – Object Oriented Programming in C++**.

The project was designed to go beyond simply learning OOP syntax by applying concepts such as **classes, objects, encapsulation, constructors, member functions, composition, vectors, references, and object interaction** in a complete working application.

---

## 🎮 Game Overview

The player controls a snake inside a bounded console map.

### Objective

* Move the snake using **W, A, S, D**
* Eat food represented by `*`
* Grow the snake after eating food
* Increase the score
* Avoid hitting the walls
* Avoid colliding with the snake's own body
* Press `Q` to quit
* Press `R` after Game Over to restart

---

## 🛠️ Technologies Used

* **Language:** C++
* **Paradigm:** Object-Oriented Programming
* **Compiler:** MinGW / G++
* **Platform:** Windows
* **Libraries:**

  * `<iostream>`
  * `<vector>`
  * `<cstdlib>`
  * `<ctime>`
  * `<conio.h>`
  * `<windows.h>`

---

# 🧠 OOP Concepts Implemented

The main purpose of this project was to understand how OOP concepts can be used to structure an actual program.

## 1. Classes and Objects

The game is divided into multiple classes, each responsible for a specific part of the system.

```cpp
class Map
{
    ...
};

class Snake
{
    ...
};

class Food
{
    ...
};

class Game
{
    ...
};
```

Objects are then created from these classes:

```cpp
Game game;
```

This allows the game to be represented as a collection of interacting objects instead of one large procedural program.

---

## 2. Encapsulation

Each class contains the data and operations associated with that particular component.

For example, the `Snake` class contains:

```cpp
vector<pair<int, int>> body;
char direction;
```

along with functions that operate on the snake:

```cpp
move()
changeDirection()
grow()
hitSelf()
reset()
```

Similarly, the `Food` class manages its own position and spawning logic.

This keeps related data and functionality together.

---

## 3. Constructors

Constructors are used to automatically initialize objects when they are created.

For example:

```cpp
Snake()
{
    reset();
}
```

and:

```cpp
Game()
{
    food.respawn(snake, map);
}
```

The `Map` constructor also initializes the complete game board.

This demonstrates how constructors can establish the initial state of objects.

---

## 4. Composition

The `Game` class contains objects of other classes:

```cpp
Map map;
Snake snake;
Food food;
```

This is an important OOP relationship.

Instead of putting everything inside one class, the game is composed of several specialized objects.

Conceptually:

```text
Game
 ├── Map
 ├── Snake
 └── Food
```

The `Game` object coordinates the interaction between these components.

---

## 5. Member Functions

The behavior of each object is implemented through member functions.

### Snake

```cpp
move()
changeDirection()
grow()
hitSelf()
reset()
```

### Food

```cpp
respawn()
```

### Game

```cpp
draw()
input()
checkFood()
play()
gameOverScreen()
resetGame()
run()
```

This demonstrates how object behavior can be encapsulated within the class that owns the relevant data.

---

## 6. Objects Communicating With Each Other

The classes interact with one another.

For example:

```cpp
snake.move(map);
```

The `Snake` object receives a reference to the `Map` object so that it can determine whether its next position is a wall.

Similarly:

```cpp
food.respawn(snake, map);
```

The `Food` object receives information about the snake and map to generate a valid food position.

This demonstrates practical object-to-object communication.

---

## 7. References

References are used to avoid unnecessary copying of objects.

For example:

```cpp
bool move(Map& map)
```

and:

```cpp
void respawn(const Snake& snake, const Map& map)
```

The `&` allows the function to work with the existing object.

`const` is also used when the function should not modify the object:

```cpp
const Snake& snake
const Map& map
```

This improves both efficiency and safety.

---

## 8. STL Vector

The snake's body is dynamically sized because the snake grows whenever it eats food.

Therefore, the project uses:

```cpp
vector<pair<int, int>> body;
```

Each pair represents:

```text
(x, y)
```

coordinates of a snake segment.

When the snake grows:

```cpp
body.push_back(body.back());
```

This demonstrates practical use of the C++ Standard Template Library.

---

# 🏗️ Project Architecture

The project follows a simple object-oriented architecture.

```text
                 ┌─────────────┐
                 │    Game     │
                 └──────┬──────┘
                        │
          ┌─────────────┼─────────────┐
          │             │             │
          ▼             ▼             ▼
     ┌─────────┐   ┌─────────┐   ┌─────────┐
     │   Map   │   │  Snake  │   │  Food   │
     └─────────┘   └─────────┘   └─────────┘
          │             │             │
          └─────────────┼─────────────┘
                        ▼
                  Game Logic
```

### Responsibilities

| Class   | Responsibility                                             |
| ------- | ---------------------------------------------------------- |
| `Map`   | Creates and manages the game boundaries                    |
| `Snake` | Stores and controls the snake                              |
| `Food`  | Generates valid food positions                             |
| `Game`  | Controls the overall game loop and coordinates all objects |

---

# 🔄 Game Loop

The main gameplay is controlled by:

```cpp
while(!gameOver)
{
    draw();
    input();

    if(!snake.move(map))
    {
        gameOver = true;
        break;
    }

    checkFood();

    if(snake.hitSelf())
    {
        gameOver = true;
        break;
    }

    Sleep(120);
}
```

Each iteration performs the following operations:

```text
Draw
  ↓
Read Input
  ↓
Move Snake
  ↓
Check Food
  ↓
Check Collision
  ↓
Delay
  ↓
Repeat
```

---

# 💥 Collision Detection

Two major types of collision are implemented.

### Wall Collision

The `Map` class determines whether a coordinate contains a wall:

```cpp
bool isWall(int x, int y)
{
    return tiles[y][x] == '#';
}
```

The snake uses this information before moving.

### Self Collision

The snake checks whether its head overlaps with any body segment:

```cpp
bool hitSelf()
{
    for(int i = 1; i < body.size(); i++)
    {
        if(body[0] == body[i])
            return true;
    }

    return false;
}
```

---

# 🍎 Food System

Food is randomly generated inside the playable area:

```cpp
x = rand() % (map.width - 2) + 1;
y = rand() % (map.height - 2) + 1;
```

The position is repeatedly generated until it does not overlap with the snake.

This prevents food from spawning directly on the snake.

---

# 🖥️ Console Features

The project uses Windows console functionality to improve the gameplay experience.

### Hidden Cursor

```cpp
cursorInfo.bVisible = false;
```

### Cursor Positioning

```cpp
SetConsoleCursorPosition(console, position);
```

Instead of continuously clearing the terminal, the cursor is moved back to the top and the game board is redrawn.

This creates a smoother console animation.

---

# ▶️ How to Run

### 1. Clone the repository

```bash
git clone <your-repository-url>
```

### 2. Navigate into the project

```bash
cd snake-game
```

### 3. Compile

Using G++:

```bash
g++ main.cpp -o snake
```

### 4. Run

```bash
snake
```

> This project uses Windows-specific libraries such as `<conio.h>` and `<windows.h>`, so it is intended to run on Windows.

---

# 🎮 Controls

| Key | Action                  |
| --- | ----------------------- |
| `W` | Move Up                 |
| `A` | Move Left               |
| `S` | Move Down               |
| `D` | Move Right              |
| `Q` | Quit                    |
| `R` | Restart after Game Over |

---

# 📚 Course Connection — UTA018

This project was created as a practical application of the concepts studied in **UTA018 – Object Oriented Programming in C++**.

Instead of treating OOP concepts as isolated theoretical topics, the project demonstrates how they can work together in a complete application.

### Concepts applied

* Classes and Objects
* Constructors
* Encapsulation
* Member Functions
* Object Composition
* Object Interaction
* References
* `const` correctness
* STL `vector`
* `pair`
* Control structures
* Functions
* Dynamic data structures
* Random number generation

The project demonstrates how a real program can be broken down into independent objects with clearly defined responsibilities.

---

# 🚀 Future Improvements

Possible extensions include:

* Difficulty levels
* Increasing snake speed
* High-score system
* Colored console graphics
* Obstacles
* Multiple food types
* Pause functionality
* Sound effects
* Persistent leaderboard
* Improved collision handling
* Cross-platform terminal support

---

# 👨‍💻 Author

**Shaurya Malhotra**

Computer Engineering Student
Thapar Institute of Engineering and Technology

---

## ⭐ Learning Objective

The primary objective of this project was not just to build a game, but to understand how **Object-Oriented Programming can be used to structure and manage a real application**.

Building the game provided a practical way to understand how individual classes can work together to create a complete system.
