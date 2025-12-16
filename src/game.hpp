#pragma once

#include <SFML/Graphics.hpp>

#include "player.hpp"

#define MAX_FPS 120
#define RESOLUTION_WIDTH 1920u
#define RESOLUTION_HEIGHT 1080u

enum class moveDirection
{
    Up,
    Down,
    Left,
    Right
};

class Game
{
    public:
        Game();

        void gameLoop();
        void handleInput();

    private:
        sf::RenderWindow window;
        Player player;
        Food food;
        moveDirection direction = moveDirection::Right;
};