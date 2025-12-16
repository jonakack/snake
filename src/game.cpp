#include "game.hpp"
#include "player.hpp"

Game::Game()
    : window(sf::RenderWindow(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "Snake"))
{
    // Create the screen
    window.setFramerateLimit(MAX_FPS);
}

void Game::gameLoop()
{
    // Run game loop
    while (window.isOpen())
    {
        player.storePosition();
        player.updateTail();

        if (player.collidedWithBorder())
            return; // Change to "die" later

        if (player.eat(food))
        {
            player.spawnTail();
            food.spawn();
        }

        handleInput();

        // ========== Rendering ==========
        // Clear the screen
        window.clear();

        // Draw everything
        window.draw(player);

        for (const auto& segment : player.tailSegments)
            window.draw(segment);

        window.draw(food);

        // Display it
        window.display();
    }
    return;
}

void Game::handleInput()
{
    while (const std::optional event = window.pollEvent())
        {
            // Close program
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            // Check if arrow button input
            if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                switch (keyPressed->code)
                {
                    case sf::Keyboard::Key::Up:     direction = moveDirection::Up; 
                        break;
                    case sf::Keyboard::Key::Down:   direction = moveDirection::Down; 
                        break;
                    case sf::Keyboard::Key::Left:   direction = moveDirection::Left; 
                        break;
                    case sf::Keyboard::Key::Right:  direction = moveDirection::Right; 
                        break;
                }
            }
        }

        // Change player's move direction
        switch (direction)
        {
            case moveDirection::Up:     player.move({0.0f, -PLAYER_SPEED}); 
                break;
            case moveDirection::Down:   player.move({0.0f, +PLAYER_SPEED});
                break;
            case moveDirection::Left:   player.move({-PLAYER_SPEED, 0.0f});
                break;
            case moveDirection::Right:  player.move({+PLAYER_SPEED, 0.0f});
                break;
        }
}