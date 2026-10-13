#pragma once

#ifdef _WIN32

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "IConsole.hpp"
#include <windows.h>
#include <conio.h>
#include <iostream>

/**
 * @brief Concrete implementation of IConsole for Windows console environments.
 *
 * Encapsulates Windows-specific APIs:
 * - Win32 Console API (GetStdHandle, SetConsoleCursorInfo, SetConsoleCursorPosition)
 * - MSVC / MinGW CRT console I/O (_kbhit, _getch)
 * - Sleep() from windows.h
 */
class WindowsConsole : public IConsole
{
private:
    HANDLE hConsoleOutput;
    CONSOLE_CURSOR_INFO originalCursorInfo;
    bool initialized = false;

public:
    WindowsConsole()
        : hConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE)),
          originalCursorInfo{}
    {
    }

    ~WindowsConsole() override
    {
        restore();
    }

    void init() override
    {
        if(!initialized)
        {
            if(hConsoleOutput != INVALID_HANDLE_VALUE && hConsoleOutput != NULL)
            {
                GetConsoleCursorInfo(hConsoleOutput, &originalCursorInfo);

                CONSOLE_CURSOR_INFO newCursorInfo = originalCursorInfo;
                newCursorInfo.bVisible = FALSE;
                SetConsoleCursorInfo(hConsoleOutput, &newCursorInfo);
            }
            initialized = true;
        }
        moveCursorToTop();
    }

    void restore() override
    {
        if(initialized)
        {
            if(hConsoleOutput != INVALID_HANDLE_VALUE && hConsoleOutput != NULL)
            {
                SetConsoleCursorInfo(hConsoleOutput, &originalCursorInfo);
            }
            initialized = false;
        }
    }

    void moveCursorToTop() override
    {
        setCursorPosition(0, 0);
    }

    void setCursorPosition(int x, int y) override
    {
        if(hConsoleOutput != INVALID_HANDLE_VALUE && hConsoleOutput != NULL)
        {
            COORD pos;
            pos.X = static_cast<SHORT>(x);
            pos.Y = static_cast<SHORT>(y);
            SetConsoleCursorPosition(hConsoleOutput, pos);
        }
    }

    void setCursorVisible(bool visible) override
    {
        if(hConsoleOutput != INVALID_HANDLE_VALUE && hConsoleOutput != NULL)
        {
            CONSOLE_CURSOR_INFO info;
            GetConsoleCursorInfo(hConsoleOutput, &info);
            info.bVisible = visible ? TRUE : FALSE;
            SetConsoleCursorInfo(hConsoleOutput, &info);
        }
    }

    bool isKeyAvailable() override
    {
        return _kbhit() != 0;
    }

    char readKey() override
    {
        return static_cast<char>(_getch());
    }

    void sleepMs(int milliseconds) override
    {
        Sleep(static_cast<DWORD>(milliseconds));
    }

    void clearScreen() override
    {
        if(hConsoleOutput != INVALID_HANDLE_VALUE && hConsoleOutput != NULL)
        {
            COORD topLeft = {0, 0};
            CONSOLE_SCREEN_BUFFER_INFO screen;
            DWORD written;
            if(GetConsoleScreenBufferInfo(hConsoleOutput, &screen))
            {
                DWORD totalCells = screen.dwSize.X * screen.dwSize.Y;
                FillConsoleOutputCharacterA(hConsoleOutput, ' ', totalCells, topLeft, &written);
                FillConsoleOutputAttribute(hConsoleOutput, screen.wAttributes, totalCells, topLeft, &written);
                SetConsoleCursorPosition(hConsoleOutput, topLeft);
            }
        }
    }
};

#endif // _WIN32
