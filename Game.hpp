#pragma once

#include <iostream>
#include "IConsole.hpp"
#include "Map.hpp"
#include "Snake.hpp"
#include "Food.hpp"

/**
 * @brief Coordinates the game loop, user input, drawing, and gameplay rules.
 *
 * Demonstrates:
 * - Composition: Game HAS-A Map, Snake, and Food.
 * - Abstraction & Dependency Injection: Game depends on the abstract IConsole interface,
 *   never directly touching Windows-specific or POSIX-specific APIs.
 * - Runtime Polymorphism: Calls pure virtual functions on IConsole at runtime.
 */
class Game
{
private:
    Map map;
    Snake snake;
    Food food;

    int score = 0;
    bool gameOver = false;
    bool isRunning = true;
    IConsole* console = nullptr;

public:
    explicit Game(IConsole* consolePtr = nullptr)
        : console(consolePtr)
    {
        food.respawn(snake, map);
    }

    void setConsole(IConsole* consolePtr)
    {
        console = consolePtr;
    }

    IConsole* getConsole() const { return console; }
    int getScore() const { return score; }
    bool isGameOver() const { return gameOver; }
    bool getIsRunning() const { return isRunning; }
    const Map& getMap() const { return map; }
    const Snake& getSnake() const { return snake; }
    const Food& getFood() const { return food; }

    // Direct mutators for automated testing
    void setScore(int s) { score = s; }
    void setGameOver(bool go) { gameOver = go; }
    void setIsRunning(bool running) { isRunning = running; }
    Snake& getSnakeMutable() { return snake; }
    Food& getFoodMutable() { return food; }
    Map& getMapMutable() { return map; }

    void resetGame()
    {
        snake.reset();
        score = 0;
        gameOver = false;
        food.respawn(snake, map);

        if(console)
        {
            console->moveCursorToTop();
        }
    }

    void draw()
    {
        if(console)
        {
            console->moveCursorToTop();
        }

        const auto& body = snake.getBody();

        for(int y = 0; y < map.getHeight(); y++)
        {
            for(int x = 0; x < map.getWidth(); x++)
            {
                bool snakePart = false;

                for(size_t i = 0; i < body.size(); i++)
                {
                    if(body[i].first == x && body[i].second == y)
                    {
                        snakePart = true;
                        if(i == 0)
                        {
                            std::cout << "@";
                        }
                        else
                        {
                            std::cout << "o";
                        }
                        break;
                    }
                }

                if(!snakePart)
                {
                    if(x == food.getX() && y == food.getY())
                    {
                        std::cout << "*";
                    }
                    else
                    {
                        std::cout << map.getTile(x, y);
                    }
                }
            }
            std::cout << '\n';
        }

        std::cout << '\n';
        std::cout << "Score: " << score << "     \n";
        std::cout << "WASD = Move | Q = Quit          \n";
        std::cout.flush();
    }

    void handleInput()
    {
        if(!console)
        {
            return;
        }

        if(console->isKeyAvailable())
        {
            char input = console->readKey();

            if(input == 'q' || input == 'Q')
            {
                isRunning = false;
                gameOver = true;
                return;
            }

            snake.changeDirection(input);
        }
    }

    void checkFood()
    {
        if(snake.getHead().first == food.getX() &&
           snake.getHead().second == food.getY())
        {
            snake.grow();
            score++;
            food.respawn(snake, map);
        }
    }

    void step()
    {
        handleInput();
        if(!isRunning)
        {
            return;
        }

        if(!snake.move(map))
        {
            gameOver = true;
            return;
        }

        checkFood();

        if(snake.hitSelf())
        {
            gameOver = true;
            return;
        }
    }

    void play()
    {
        while(!gameOver && isRunning)
        {
            draw();
            step();

            if(gameOver || !isRunning)
            {
                break;
            }

            if(console)
            {
                console->sleepMs(120);
            }
        }
    }

    void gameOverScreen()
    {
        if(console)
        {
            console->moveCursorToTop();
        }

        std::cout << "================================\n";
        std::cout << "           GAME OVER\n";
        std::cout << "================================\n\n";
        std::cout << "Score: " << score << "\n\n";
        std::cout << "R = Restart\n";
        std::cout << "Q = Quit\n";
        std::cout.flush();
    }

    void run()
    {
        if(console)
        {
            console->init();
        }

        while(isRunning)
        {
            play();

            if(!isRunning)
            {
                break;
            }

            gameOverScreen();

            while(isRunning)
            {
                if(console && console->isKeyAvailable())
                {
                    char input = console->readKey();

                    if(input == 'r' || input == 'R')
                    {
                        resetGame();
                        break;
                    }

                    if(input == 'q' || input == 'Q')
                    {
                        isRunning = false;
                        break;
                    }
                }
                else if(console)
                {
                    console->sleepMs(50);
                }
            }
        }

        if(console)
        {
            console->restore();
        }
    }
};
