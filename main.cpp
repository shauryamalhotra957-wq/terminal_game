#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <vector>
#include <windows.h>
#include <ctime>

using namespace std;

class Map
{
public:
    int width = 30;
    int height = 10;

    char tiles[10][30];

    Map()
    {
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                if(x == 0 || x == width - 1 ||
                   y == 0 || y == height - 1)
                {
                    tiles[y][x] = '#';
                }
                else
                {
                    tiles[y][x] = ' ';
                }
            }
        }
    }

    bool isWall(int x, int y)
    {
        return tiles[y][x] == '#';
    }
};

class Snake
{
public:
    vector<pair<int, int>> body;
    char direction;

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

    bool move(Map& map)
    {
        int newX = body[0].first;
        int newY = body[0].second;

        if(direction == 'w')
            newY--;

        else if(direction == 's')
            newY++;

        else if(direction == 'a')
            newX--;

        else if(direction == 'd')
            newX++;

        if(map.isWall(newX, newY))
            return false;

        for(int i = body.size() - 1; i > 0; i--)
        {
            body[i] = body[i - 1];
        }

        body[0] = {newX, newY};

        return true;
    }

    void changeDirection(char input)
    {
        if(input == 'w' && direction != 's')
            direction = 'w';

        else if(input == 's' && direction != 'w')
            direction = 's';

        else if(input == 'a' && direction != 'd')
            direction = 'a';

        else if(input == 'd' && direction != 'a')
            direction = 'd';
    }

    void grow()
    {
        body.push_back(body.back());
    }

    bool hitSelf()
    {
        for(int i = 1; i < body.size(); i++)
        {
            if(body[0] == body[i])
                return true;
        }

        return false;
    }
};

class Food
{
public:
    int x;
    int y;

    Food()
    {
        x = 1;
        y = 1;
    }

    void respawn(const Snake& snake, const Map& map)
    {
        bool validPosition = false;

        while(!validPosition)
        {
            x = rand() % (map.width - 2) + 1;
            y = rand() % (map.height - 2) + 1;

            validPosition = true;

            for(const auto& segment : snake.body)
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

class Game
{
public:
    Map map;
    Snake snake;
    Food food;

    int score = 0;
    bool gameOver = false;

    Game()
    {
        food.respawn(snake, map);
    }

    void setupConsole()
    {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(console, &cursorInfo);
        cursorInfo.bVisible = false;
        SetConsoleCursorInfo(console, &cursorInfo);

        COORD position;
        position.X = 0;
        position.Y = 0;
        SetConsoleCursorPosition(console, position);
    }

    void moveCursorToTop()
    {
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD position;
        position.X = 0;
        position.Y = 0;

        SetConsoleCursorPosition(console, position);
    }

    void resetGame()
    {
        snake.reset();

        score = 0;
        gameOver = false;

        food.respawn(snake, map);

        moveCursorToTop();
    }

    void draw()
    {
        moveCursorToTop();

        for(int y = 0; y < map.height; y++)
        {
            for(int x = 0; x < map.width; x++)
            {
                bool snakePart = false;

                for(int i = 0; i < snake.body.size(); i++)
                {
                    if(snake.body[i].first == x &&
                       snake.body[i].second == y)
                    {
                        snakePart = true;

                        if(i == 0)
                            cout << "@";
                        else
                            cout << "o";

                        break;
                    }
                }

                if(!snakePart)
                {
                    if(x == food.x && y == food.y)
                        cout << "*";
                    else
                        cout << map.tiles[y][x];
                }
            }

            cout << '\n';
        }

        cout << '\n';
        cout << "Score: " << score << "     \n";
        cout << "WASD = Move | Q = Quit          \n";

        cout.flush();
    }

    void input()
    {
        if(_kbhit())
        {
            char input = _getch();

            if(input == 'q')
                exit(0);

            snake.changeDirection(input);
        }
    }

    void checkFood()
    {
        if(snake.body[0].first == food.x &&
           snake.body[0].second == food.y)
        {
            snake.grow();
            score++;

            food.respawn(snake, map);
        }
    }

    void play()
    {
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
    }

    void gameOverScreen()
    {
        moveCursorToTop();

        cout << "================================\n";
        cout << "           GAME OVER\n";
        cout << "================================\n\n";
        cout << "Score: " << score << "\n\n";
        cout << "R = Restart\n";
        cout << "Q = Quit\n";
        cout.flush();
    }

    void run()
    {
        setupConsole();

        while(true)
        {
            play();

            gameOverScreen();

            while(true)
            {
                char input = _getch();

                if(input == 'r')
                {
                    resetGame();
                    break;
                }

                if(input == 'q')
                {
                    return;
                }
            }
        }
    }
};

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    Game game;
    game.run();

    return 0;
}