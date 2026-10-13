#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory>

#include "IConsole.hpp"

#ifdef _WIN32
#include "WindowsConsole.hpp"
using PlatformConsole = WindowsConsole;
#else
#include "PosixConsole.hpp"
using PlatformConsole = PosixConsole;
#endif

#include "Map.hpp"
#include "Snake.hpp"
#include "Food.hpp"
#include "Game.hpp"

int main()
{
    // Seed random number generator for food placement
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Instantiate concrete platform console polymorphically through IConsole
    auto console = std::make_unique<PlatformConsole>();

    // Inject console dependency into Game
    Game game(console.get());

    // Run the main game loop
    game.run();

    return 0;
}
