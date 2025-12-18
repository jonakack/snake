#pragma once

#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "player.hpp"

#define MAX_FPS 120
#define RESOLUTION_WIDTH 1920u
#define RESOLUTION_HEIGHT 1080u
#define FONT_ARIAL "../../fonts/ARIAL.TTF"

class Scoreboard
{
public:
    Scoreboard();

    void increaseScore(int amount);
    sf::Text text;

private:
    sf::Font font;
    int currentScore {0};
};

class Game
{
public:
    Game();

    void gameLoop();
    void menuLoop();
    void handleInput();

private:
    sf::RenderWindow window;
    Player player;
    Food food;
    Scoreboard scoreboard;
    moveDirection direction;
};