#pragma once

#include <cstdlib>
#include "Map.hpp"
#include "Snake.hpp"

/**
 * @brief Represents the food item the snake consumes.
 *
 * Encapsulation:
 * - Position (x, y) is private.
 * - Coordinates are exposed through getters.
 * - Spawning logic ensures food does not land on walls or the snake's body.
 */
class Food
{
private:
    int x;
    int y;

public:
    Food()
        : x(1), y(1)
    {
    }

    int getX() const { return x; }
    int getY() const { return y; }

    void setPosition(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    void respawn(const Snake& snake, const Map& map)
    {
        bool validPosition = false;

        while(!validPosition)
        {
            x = rand() % (map.getWidth() - 2) + 1;
            y = rand() % (map.getHeight() - 2) + 1;

            validPosition = true;

            if(map.isWall(x, y))
            {
                validPosition = false;
                continue;
            }

            for(const auto& segment : snake.getBody())
            {
                if(segment.first == x && segment.second == y)
                {
                    validPosition = false;
                    break;
                }
            }
        }
    }
};
