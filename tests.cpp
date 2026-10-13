#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include "IConsole.hpp"
#include "Map.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Game.hpp"

/**
 * @brief MockConsole for headless, non-interactive automated testing.
 *
 * Implements the abstract IConsole interface. Simulates keyboard inputs
 * and records console operations to verify polymorphism and game logic.
 */
class MockConsole : public IConsole
{
private:
    std::vector<char> inputQueue;
    size_t inputIndex = 0;
    int cursorMoves = 0;
    bool cursorVisible = true;
    bool initialized = false;
    bool restored = false;
    int sleepCalls = 0;

public:
    void feedKey(char key)
    {
        inputQueue.push_back(key);
    }

    void feedKeys(const std::string& keys)
    {
        for(char c : keys)
        {
            inputQueue.push_back(c);
        }
    }

    void init() override
    {
        initialized = true;
        cursorVisible = false;
    }

    void restore() override
    {
        restored = true;
        cursorVisible = true;
    }

    void moveCursorToTop() override
    {
        cursorMoves++;
    }

    void setCursorPosition(int, int) override {}

    void setCursorVisible(bool visible) override
    {
        cursorVisible = visible;
    }

    bool isKeyAvailable() override
    {
        return inputIndex < inputQueue.size();
    }

    char readKey() override
    {
        if(inputIndex < inputQueue.size())
        {
            return inputQueue[inputIndex++];
        }
        return 0;
    }

    void sleepMs(int) override
    {
        sleepCalls++;
    }

    void clearScreen() override {}

    bool isInitialized() const { return initialized; }
    bool isRestored() const { return restored; }
    int getCursorMoves() const { return cursorMoves; }
    int getSleepCalls() const { return sleepCalls; }
};

// Test Runner Helper
static int testsRun = 0;
static int testsPassed = 0;

#define ASSERT_TRUE(condition, message) \
    do { \
        testsRun++; \
        if (!(condition)) { \
            std::cerr << "[-] FAILED: " << message << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
            return false; \
        } else { \
            testsPassed++; \
        } \
    } while(0)

bool testConsolePolymorphism()
{
    std::cout << "[RUN] Testing IConsole runtime polymorphism and abstraction...\n";

    MockConsole mock;
    IConsole* console = &mock;

    // Test pure virtual calls through base pointer
    console->init();
    ASSERT_TRUE(mock.isInitialized(), "Base pointer init() should initialize derived mock");

    console->moveCursorToTop();
    ASSERT_TRUE(mock.getCursorMoves() == 1, "Base pointer moveCursorToTop() should register");

    mock.feedKey('w');
    ASSERT_TRUE(console->isKeyAvailable(), "isKeyAvailable() should detect fed key");
    ASSERT_TRUE(console->readKey() == 'w', "readKey() should return expected key");
    ASSERT_TRUE(!console->isKeyAvailable(), "Queue should be empty after read");

    console->restore();
    ASSERT_TRUE(mock.isRestored(), "Base pointer restore() should restore derived mock");

    return true;
}

bool testMapLoadingAndDimensions()
{
    std::cout << "[RUN] Testing Map loading and dimension validation...\n";

    // 1. Standard map should have dimensions 30x10
    Map defaultMap;
    ASSERT_TRUE(defaultMap.getWidth() == 30, "Map width must be 30");
    ASSERT_TRUE(defaultMap.getHeight() == 10, "Map height must be 10");

    // 2. Borders should be walls
    ASSERT_TRUE(defaultMap.isWall(0, 0), "Corner (0,0) must be a wall");
    ASSERT_TRUE(defaultMap.isWall(29, 0), "Corner (29,0) must be a wall");
    ASSERT_TRUE(defaultMap.isWall(0, 9), "Corner (0,9) must be a wall");
    ASSERT_TRUE(defaultMap.isWall(29, 9), "Corner (29,9) must be a wall");

    // 3. Playable area should not be walls
    ASSERT_TRUE(!defaultMap.isWall(1, 1), "Inner tile (1,1) must be open space");
    ASSERT_TRUE(!defaultMap.isWall(10, 5), "Snake starting tile (10,5) must be open space");

    // 4. Out-of-bounds coordinates must count as walls
    ASSERT_TRUE(defaultMap.isWall(-1, 0), "Negative X must be wall");
    ASSERT_TRUE(defaultMap.isWall(0, -1), "Negative Y must be wall");
    ASSERT_TRUE(defaultMap.isWall(30, 5), "X >= width must be wall");
    ASSERT_TRUE(defaultMap.isWall(10, 10), "Y >= height must be wall");

    return true;
}

bool testMapFallbackOnMissingOrInvalid()
{
    std::cout << "[RUN] Testing Map graceful fallback on missing and invalid files...\n";

    // 1. Non-existent file should gracefully fall back to built-in default
    Map missingMap("non_existent_file_12345.txt");
    ASSERT_TRUE(missingMap.getWidth() == 30, "Fallback map must still be width 30");
    ASSERT_TRUE(missingMap.getHeight() == 10, "Fallback map must still be height 10");
    ASSERT_TRUE(missingMap.isWall(0, 0), "Fallback map must have walls");
    ASSERT_TRUE(!missingMap.isWall(10, 5), "Fallback map must have open space");

    // 2. Corrupted file with wrong dimensions
    std::string invalidFile = "temp_invalid_map_test.txt";
    {
        std::ofstream out(invalidFile);
        out << "###\n###\n"; // Only 2 rows of 3 columns
    }

    Map invalidMap(invalidFile);
    ASSERT_TRUE(invalidMap.getWidth() == 30, "Invalid file fallback must preserve width 30");
    ASSERT_TRUE(invalidMap.getHeight() == 10, "Invalid file fallback must preserve height 10");
    ASSERT_TRUE(invalidMap.isWall(0, 0), "Fallback map must have valid boundary walls");

    // Clean up temporary invalid file
    std::remove(invalidFile.c_str());

    return true;
}

bool testSnakeMovementAndEncapsulation()
{
    std::cout << "[RUN] Testing Snake movement and encapsulation...\n";

    Map map;
    Snake snake;

    // Verify initial state
    ASSERT_TRUE(snake.getBody().size() == 3, "Initial snake length should be 3");
    ASSERT_TRUE(snake.getHead().first == 10, "Initial head X should be 10");
    ASSERT_TRUE(snake.getHead().second == 5, "Initial head Y should be 5");
    ASSERT_TRUE(snake.getDirection() == 'd', "Initial direction should be 'd'");

    // Move right ('d')
    bool moved = snake.move(map);
    ASSERT_TRUE(moved, "Snake should move successfully in open space");
    ASSERT_TRUE(snake.getHead().first == 11, "Head should advance to X=11");
    ASSERT_TRUE(snake.getHead().second == 5, "Head Y should remain 5");

    // Turn up ('w')
    snake.changeDirection('w');
    ASSERT_TRUE(snake.getDirection() == 'w', "Direction should change to 'w'");
    moved = snake.move(map);
    ASSERT_TRUE(moved, "Snake should move up");
    ASSERT_TRUE(snake.getHead().first == 11, "Head X should remain 11");
    ASSERT_TRUE(snake.getHead().second == 4, "Head Y should be 4");

    // Turn left ('a')
    snake.changeDirection('a');
    ASSERT_TRUE(snake.getDirection() == 'a', "Direction should change to 'a'");
    moved = snake.move(map);
    ASSERT_TRUE(moved, "Snake should move left");
    ASSERT_TRUE(snake.getHead().first == 10, "Head X should be 10");
    ASSERT_TRUE(snake.getHead().second == 4, "Head Y should be 4");

    // Turn down ('s')
    snake.changeDirection('s');
    ASSERT_TRUE(snake.getDirection() == 's', "Direction should change to 's'");
    moved = snake.move(map);
    ASSERT_TRUE(moved, "Snake should move down");
    ASSERT_TRUE(snake.getHead().first == 10, "Head X should be 10");
    ASSERT_TRUE(snake.getHead().second == 5, "Head Y should be 5");

    return true;
}

bool testSnakeReverseRejection()
{
    std::cout << "[RUN] Testing 180-degree reversal prevention...\n";

    Snake snake; // Moving 'd'
    ASSERT_TRUE(snake.getDirection() == 'd', "Initial direction is 'd'");

    // Trying to reverse to 'a' directly should be rejected
    snake.changeDirection('a');
    ASSERT_TRUE(snake.getDirection() == 'd', "Direct reversal 'a' must be ignored when moving 'd'");

    // Try uppercase 'A'
    snake.changeDirection('A');
    ASSERT_TRUE(snake.getDirection() == 'd', "Uppercase 'A' must also be ignored when moving 'd'");

    // Turn 'w', then try to reverse to 's'
    snake.changeDirection('w');
    ASSERT_TRUE(snake.getDirection() == 'w', "Turn to 'w' must succeed");
    snake.changeDirection('s');
    ASSERT_TRUE(snake.getDirection() == 'w', "Direct reversal 's' must be ignored when moving 'w'");

    return true;
}

bool testSnakeWallCollision()
{
    std::cout << "[RUN] Testing wall collision detection...\n";

    Map map;
    Snake snake;

    // Steer snake towards top wall (Y=0)
    snake.changeDirection('w');
    // Start head is at (10, 5). Move up: (10,4), (10,3), (10,2), (10,1)
    for(int i = 0; i < 4; i++)
    {
        bool moved = snake.move(map);
        ASSERT_TRUE(moved, "Movement within boundary should succeed");
    }

    ASSERT_TRUE(snake.getHead().second == 1, "Head should be right before top wall (Y=1)");

    // Next move into top wall (Y=0) must fail
    bool hitWall = !snake.move(map);
    ASSERT_TRUE(hitWall, "Moving into top wall must be detected and return false");

    return true;
}

bool testSnakeGrowthAndSelfCollision()
{
    std::cout << "[RUN] Testing snake growth and self-collision...\n";

    Map map;
    Snake snake;

    ASSERT_TRUE(!snake.hitSelf(), "New snake must not collide with itself");

    // Grow snake several times
    snake.grow();
    snake.move(map);
    snake.grow();
    snake.move(map);
    snake.grow();
    snake.move(map);
    ASSERT_TRUE(snake.getBody().size() == 6, "Snake body size should now be 6");

    // Circle into itself: currently moving 'd'
    snake.changeDirection('s');
    snake.move(map);
    snake.changeDirection('a');
    snake.move(map);
    snake.changeDirection('w');
    snake.move(map); // Moves back onto its own body

    ASSERT_TRUE(snake.hitSelf(), "Snake colliding with its own body must be detected");

    return true;
}

bool testFoodSpawningAndCollection()
{
    std::cout << "[RUN] Testing food spawning and collection...\n";

    Map map;
    Snake snake;
    Food food;

    // Test respawn does not land on walls or snake
    for(int trial = 0; trial < 50; trial++)
    {
        food.respawn(snake, map);
        int fx = food.getX();
        int fy = food.getY();

        ASSERT_TRUE(!map.isWall(fx, fy), "Food must never spawn on a wall");

        for(const auto& seg : snake.getBody())
        {
            ASSERT_TRUE(!(seg.first == fx && seg.second == fy), "Food must never spawn on snake");
        }
    }

    return true;
}

bool testGameLoopWithDependencyInjection()
{
    std::cout << "[RUN] Testing Game integration with injected MockConsole...\n";

    MockConsole mockConsole;
    Game game(&mockConsole);

    ASSERT_TRUE(game.getScore() == 0, "Initial score should be 0");
    ASSERT_TRUE(!game.isGameOver(), "Initial game state should not be game over");
    ASSERT_TRUE(game.getIsRunning(), "Game should be running initially");

    // Position food right in front of the snake: head is at (10, 5), moving 'd' -> (11, 5)
    game.getFoodMutable().setPosition(11, 5);

    // Single step
    game.step();

    // Snake should have moved to (11, 5), eaten the food, grown, and incremented score
    ASSERT_TRUE(game.getScore() == 1, "Score should increase to 1 after eating food");
    ASSERT_TRUE(game.getSnake().getBody().size() == 4, "Snake should grow to length 4");

    // Feed 'q' into mock console and step
    mockConsole.feedKey('q');
    game.step();
    ASSERT_TRUE(!game.getIsRunning(), "Pressing 'q' should set isRunning to false");

    // Test resetGame
    game.resetGame();
    ASSERT_TRUE(game.getScore() == 0, "Score should reset to 0");
    ASSERT_TRUE(!game.isGameOver(), "GameOver flag should reset to false");
    ASSERT_TRUE(game.getSnake().getBody().size() == 3, "Snake size should reset to 3");

    return true;
}

int main()
{
    std::cout << "========================================\n";
    std::cout << " TerminalGame Automated Test Suite\n";
    std::cout << "========================================\n\n";

    bool allPassed = true;

    allPassed &= testConsolePolymorphism();
    allPassed &= testMapLoadingAndDimensions();
    allPassed &= testMapFallbackOnMissingOrInvalid();
    allPassed &= testSnakeMovementAndEncapsulation();
    allPassed &= testSnakeReverseRejection();
    allPassed &= testSnakeWallCollision();
    allPassed &= testSnakeGrowthAndSelfCollision();
    allPassed &= testFoodSpawningAndCollection();
    allPassed &= testGameLoopWithDependencyInjection();

    std::cout << "\n========================================\n";
    std::cout << " Test Summary: " << testsPassed << "/" << testsRun << " assertions passed.\n";

    if(allPassed && (testsPassed == testsRun))
    {
        std::cout << " [SUCCESS] All automated tests passed!\n";
        std::cout << "========================================\n";
        return 0;
    }
    else
    {
        std::cerr << " [FAILURE] Some tests failed!\n";
        std::cout << "========================================\n";
        return 1;
    }
}
