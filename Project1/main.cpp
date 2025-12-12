#include"Game.h"
#include<iostream>

int main(int argc, char* argv[])
{
    Game game;

    if (!game.init("SDL3 Game", 800, 600)) {
        cerr << "Failed to initialize game.\n";
        return 1;
    }

    game.run();

    return 0;
}