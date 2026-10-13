#pragma once

#include <vector>
#include <utility>
#include "Map.hpp"

/**
 * @brief Represents the snake entity in the game.
 *
 * Encapsulation:
 * - Coordinates of the snake body and current movement direction are private.
 * - Modifications happen through public member functions: move(), changeDirection(), grow(), reset().
 */
class Snake
{
private:
    std::vector<std::pair<int, int>> body;
    char direction;

public:
    Snake()
    {
        reset();
    }

    void reset()
    {
        body.clear();
        body.push_back({10, 5});
        body.push_back({9, 5});
        body.push_back({8, 5});
        direction = 'd';
    }

    const std::vector<std::pair<int, int>>& getBody() const
    {
        return body;
    }

    std::pair<int, int> getHead() const
    {
        return body.empty() ? std::make_pair(0, 0) : body[0];
    }

    char getDirection() const
    {
        return direction;
    }

    bool move(const Map& map)
    {
        int newX = body[0].first;
        int newY = body[0].second;

        if(direction == 'w')
        {
            newY--;
        }
        else if(direction == 's')
        {
            newY++;
        }
        else if(direction == 'a')
        {
            newX--;
        }
        else if(direction == 'd')
        {
            newX++;
        }

        if(map.isWall(newX, newY))
        {
            return false;
        }

        for(int i = static_cast<int>(body.size()) - 1; i > 0; i--)
        {
            body[i] = body[i - 1];
        }

        body[0] = {newX, newY};

        return true;
    }

    void changeDirection(char input)
    {
        if((input == 'w' || input == 'W') && direction != 's')
        {
            direction = 'w';
        }
        else if((input == 's' || input == 'S') && direction != 'w')
        {
            direction = 's';
        }
        else if((input == 'a' || input == 'A') && direction != 'd')
        {
            direction = 'a';
        }
        else if((input == 'd' || input == 'D') && direction != 'a')
        {
            direction = 'd';
        }
    }

    void grow()
    {
        body.push_back(body.back());
    }

    bool hitSelf() const
    {
        for(size_t i = 1; i < body.size(); i++)
        {
            if(body[0] == body[i])
            {
                return true;
            }
        }
        return false;
    }
};
