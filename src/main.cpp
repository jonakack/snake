#include <SFML/Graphics.hpp>

#include "game.hpp"

int main()
{
    Game game;

    game.menuLoop();
    
    game.gameLoop();

    return 0;
}