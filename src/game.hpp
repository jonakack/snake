#pragma once

#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "player.hpp"

#define MUSIC_VOLUME 50.0f
#define MAX_FPS 120
#define RESOLUTION_WIDTH 1920u
#define RESOLUTION_HEIGHT 1080u
#define FONT "../../fonts/ARCADECLASSIC.TTF"

class Button;

class Scoreboard
{
public:
    Scoreboard();

    void increaseScore(int amount);
    sf::Text text;

private:
    sf::Font font;
    int currentScore{0};
};

class Game
{
public:
    Game();

    void gameLoop();
    void handleGameInput();

    void menuLoop();
    void handleMenuInput(Button &startButton);

private:
    sf::RenderWindow window;
    Player player;
    Food food;
    Scoreboard scoreboard;
    moveDirection direction;
};

class Button : public sf::RectangleShape
{
public:
    Button(sf::Vector2f size, std::string text);
};