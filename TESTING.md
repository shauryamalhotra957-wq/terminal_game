# Regression testing

The game uses Windows console APIs (<conio.h> and <windows.h>), so run these checks on Windows with a MinGW/G++ build.

## Build

~~~text
make
~~~

The build should complete with -std=c++17 -Wall -Wextra and produce main.exe. Remove generated output with:

~~~text
make clean
~~~

## Manual checks

Start the game with main.exe and verify:

- The map renders with walls, a snake, and one food item.
- W, A, S, and D move the snake in the expected direction.
- A direct reverse input is rejected when it would immediately collide with the snake.
- Eating food increases the score and grows the snake.
- Food never appears on a wall or on a snake segment.
- Hitting a wall ends the round and shows the game-over screen.
- Moving into the snake's body ends the round.
- Pressing R after game over resets the snake, score, and food.
- Pressing Q exits without leaving the console in an unusable state.

Repeat the collision checks with the snake near each edge of the map. If the map layout in map.txt changes, rerun the full checklist because collision coordinates and valid food positions are coupled to that file.
