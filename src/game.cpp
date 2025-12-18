#include <SFML/Audio.hpp>

#include "game.hpp"
#include "player.hpp"

Game::Game()
    : window(sf::RenderWindow(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "Snake"))
{
    window.setFramerateLimit(MAX_FPS);
    direction = moveDirection::Right;
}

Scoreboard::Scoreboard()
    : font(FONT_ARIAL)
    , text(font, "Current score: 0", 50)
{
    text.setPosition({RESOLUTION_WIDTH / 15, RESOLUTION_HEIGHT / 15});
}

void Scoreboard::increaseScore(int amount)
{
    currentScore += amount;

    std::string currentScoreStr = std::to_string(currentScore);
    text.setString("Current score: " + currentScoreStr);
}

void Game::gameLoop()
{
    sf::Music backgroundMusic("../../soundfx/magicmamaliga.mp3");
    backgroundMusic.setVolume(50.0f);
    backgroundMusic.play();

    food.spawn(player);
    
    // Run game loop
    while (window.isOpen())
    {
        if (player.collidedWithBorder() || player.collidedWithSelf())
            return; // Change to "die" later

        if (player.eat(food))
        {
            player.spawnTail();
            food.spawn(player);
            scoreboard.increaseScore(10);
        }

        handleInput();

        player.incrementFramesSinceTurn();
        player.storePosition(); 
        player.updateTail();
        player.updateCorners();

        // ========== Rendering ==========
        // Clear the screen
        window.clear();

        // Draw everything
        window.draw(player);

        for (const auto& segment : player.tailSegments)
            window.draw(segment);

        for (const auto& corner : player.cornerSegments)
            window.draw(corner.shape);

        window.draw(food);

        window.draw(scoreboard.text);

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
            const int minFramesBetweenTurns = 2 * framesPerSegment;
            
            switch (keyPressed->code)
            {
                case sf::Keyboard::Key::Up:     
                    if (direction != moveDirection::Down && player.framesSinceTurn >= minFramesBetweenTurns) 
                        direction = moveDirection::Up; 
                    break;
                case sf::Keyboard::Key::Down:   
                    if (direction != moveDirection::Up && player.framesSinceTurn >= minFramesBetweenTurns)
                        direction = moveDirection::Down; 
                    break;
                case sf::Keyboard::Key::Left:   
                    if (direction != moveDirection::Right && player.framesSinceTurn >= minFramesBetweenTurns)
                        direction = moveDirection::Left; 
                    break;
                case sf::Keyboard::Key::Right:  
                    if (direction != moveDirection::Left && player.framesSinceTurn >= minFramesBetweenTurns)
                        direction = moveDirection::Right; 
                    break;
            }
        }
    }

    // Change player's move direction
    switch (direction)
    {
        case moveDirection::Up:     player.moveSnake(moveDirection::Up);
            break;
        case moveDirection::Down:   player.moveSnake(moveDirection::Down);
            break;
        case moveDirection::Left:   player.moveSnake(moveDirection::Left);
            break;
        case moveDirection::Right:  player.moveSnake(moveDirection::Right);
            break;
    }
}