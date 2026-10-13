#pragma once

#include <string>

/**
 * @brief Abstract interface defining console operations required by the game.
 *
 * Demonstrates C++ OOP Abstraction:
 * - Pure virtual functions (`= 0`) declare the interface contract.
 * - Decouples game logic from OS-specific terminal/console handling (Windows vs POSIX vs Mock).
 * - Enables runtime polymorphism and dependency injection.
 */
class IConsole
{
public:
    virtual ~IConsole() = default;

    // Console lifecycle management
    virtual void init() = 0;
    virtual void restore() = 0;

    // Cursor operations
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
