#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "PathUtils.hpp"

/**
 * @brief Represents the game board, boundaries, and tile layout.
 *
 * Encapsulation:
 * - Tile array and dimensions are private.
 * - Accessed safely via getters and boundary checking methods.
 * - Resource resolution looks in working directory, beside executable,
 *   and safely falls back to a built-in 30x10 default map if missing or invalid.
 */
class Map
{
public:
    static constexpr int WIDTH = 30;
    static constexpr int HEIGHT = 10;

private:
    int width = WIDTH;
    int height = HEIGHT;
    char tiles[HEIGHT][WIDTH];
    bool customMapLoaded = false;

    void loadDefaultMap()
    {
        static const char* const DEFAULT_MAP[HEIGHT] = {
            "##############################",
            "#                            #",
            "#                            #",
            "#                            #",
            "#                            #",
            "#                            #",
            "#                            #",
            "#                            #",
            "#                            #",
            "##############################"
        };

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                tiles[y][x] = DEFAULT_MAP[y][x];
            }
        }
        customMapLoaded = false;
    }

public:
    explicit Map(const std::string& filename = "map.txt")
    {
        if(!loadWithFallback(filename))
        {
            loadDefaultMap();
        }
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isCustomMap() const { return customMapLoaded; }

    char getTile(int x, int y) const
    {
        if(x < 0 || x >= width || y < 0 || y >= height)
        {
            return '#';
        }
        return tiles[y][x];
    }

    bool isWall(int x, int y) const
    {
        if(x < 0 || x >= width || y < 0 || y >= height)
        {
            return true;
        }
        return tiles[y][x] == '#';
    }

    bool loadFromFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if(!file.is_open())
        {
            return false;
        }

        char tempTiles[HEIGHT][WIDTH];
        std::string line;
        int rowCount = 0;

        while(std::getline(file, line))
        {
            if(!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            if(rowCount == height && line.empty())
            {
                continue;
            }

            if(rowCount >= height)
            {
                return false;
            }

            if(line.length() != static_cast<size_t>(width))
            {
                return false;
            }

            for(int x = 0; x < width; x++)
            {
                tempTiles[rowCount][x] = line[x];
            }

            rowCount++;
        }

        if(rowCount != height)
        {
            return false;
        }

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                tiles[y][x] = tempTiles[y][x];
            }
        }

        customMapLoaded = true;
        return true;
    }

    bool loadWithFallback(const std::string& filename = "map.txt")
    {
        // 1. Try directly from current working directory
        if(loadFromFile(filename))
        {
            return true;
        }

        // 2. Try relative to the executable directory
        std::string exeDir = getExecutableDirectory();
        if(!exeDir.empty())
        {
            std::string pathNextToExe = exeDir + "/" + filename;
            if(loadFromFile(pathNextToExe))
            {
                return true;
            }
        }

        // 3. Fall back to embedded default map
        loadDefaultMap();
        return false;
    }
};
