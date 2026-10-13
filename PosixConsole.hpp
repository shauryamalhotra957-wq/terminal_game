#pragma once

#ifndef _WIN32

#include "IConsole.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

/**
 * @brief Concrete implementation of IConsole for POSIX (Linux / macOS) terminals.
 *
 * Encapsulates:
 * - termios raw mode (non-blocking, non-canonical, no-echo)
 * - select() on STDIN_FILENO for key availability check
 * - ANSI escape codes for cursor positioning and visibility
 * - std::this_thread::sleep_for for frame timing
 */
class PosixConsole : public IConsole
{
private:
    struct termios originalTermios;
    bool initialized = false;

public:
    PosixConsole() = default;

    ~PosixConsole() override
    {
        restore();
    }

    void init() override
    {
        if(!initialized)
        {
            if(tcgetattr(STDIN_FILENO, &originalTermios) == 0)
            {
                struct termios raw = originalTermios;
                // Disable canonical mode (line buffering) and echo
                raw.c_lflag &= ~(ICANON | ECHO);
                tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            }
            // Hide cursor via ANSI escape sequence
            std::cout << "\033[?25l" << std::flush;
            initialized = true;
        }
        moveCursorToTop();
    }

    void restore() override
    {
        if(initialized)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
            // Show cursor via ANSI escape sequence
            std::cout << "\033[?25h" << std::flush;
            initialized = false;
        }
    }

    void moveCursorToTop() override
    {
        // ANSI escape sequence: move cursor to row 1, col 1
        std::cout << "\033[H" << std::flush;
    }

    void setCursorPosition(int x, int y) override
    {
        // ANSI escape sequence: \033[<line>;<col>H (1-indexed)
        std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H" << std::flush;
    }

    void setCursorVisible(bool visible) override
    {
        if(visible)
        {
            std::cout << "\033[?25h" << std::flush;
        }
        else
        {
            std::cout << "\033[?25l" << std::flush;
        }
    }

    bool isKeyAvailable() override
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        int result = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);
        return result > 0;
    }

    char readKey() override
    {
        char ch = 0;
        ssize_t bytesRead = read(STDIN_FILENO, &ch, 1);
        return (bytesRead > 0) ? ch : 0;
    }

    void sleepMs(int milliseconds) override
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void clearScreen() override
    {
        std::cout << "\033[2J\033[H" << std::flush;
    }
};

#endif // !_WIN32
