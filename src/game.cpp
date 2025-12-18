#include <iostream>

#include <SFML/Audio.hpp>

#include "game.hpp"
#include "player.hpp"

sf::Font g_font(FONT);

Game::Game()
    : window(sf::RenderWindow(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "Snake"))
{
    window.setFramerateLimit(MAX_FPS);
    direction = moveDirection::Right;
}

Scoreboard::Scoreboard()
    : font(FONT), text(font, "Current score: 0", 50)
{
    text.setPosition({RESOLUTION_WIDTH / 15, RESOLUTION_HEIGHT / 15});
}

Button::Button(sf::Vector2f size, std::string text)
    : sf::RectangleShape(size)
{
    setFillColor(sf::Color::Red);
}

void Scoreboard::increaseScore(int amount)
{
    currentScore += amount;

    std::string currentScoreStr = std::to_string(currentScore);
    text.setString("Current score: " + currentScoreStr);
}

void Game::menuLoop()
{
    // Play music
    sf::Music backgroundMusic("../../soundfx/dualofthefates.mp3");
    backgroundMusic.setVolume(MUSIC_VOLUME);
    backgroundMusic.play();

    // Set background
    sf::Texture backgroundTex("../../textures/mountain/fullmountain.png");
    sf::Sprite backgroundSprite(backgroundTex);
    sf::Vector2u textureSize = backgroundTex.getSize();
    sf::Vector2u windowSize = window.getSize();
    backgroundSprite.setScale({static_cast<float>(RESOLUTION_WIDTH) / textureSize.x, static_cast<float>(RESOLUTION_HEIGHT) / textureSize.y});

    // Load clickable buttons
    Button startButton({400.0f, 100.0f}, "Start");
    startButton.setOrigin({startButton.getSize().x / 2, startButton.getSize().y / 2});
    startButton.setPosition({RESOLUTION_WIDTH / 2, 500.0f});

    // Run game loop
    while (window.isOpen())
    {
        handleMenuInput(startButton);

        // ========== Rendering ==========
        // Clear the screen
        window.clear();

        // Draw everything
        window.draw(backgroundSprite);
        window.draw(startButton);

        // Display it
        window.display();
    }
    return;
}

void Game::handleMenuInput(Button &startButton)
{
    sf::Vector2i localMousePosition = sf::Mouse::getPosition(window);

    while (const std::optional event = window.pollEvent())
    {
        // Close program
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        // Listen for mouse on start button
        auto startButtonPos = startButton.getPosition();
        auto startButtonSize = startButton.getSize();
        auto prevButtonColor = startButton.getFillColor();

        if (localMousePosition.x < startButtonPos.x + (startButtonSize.x / 2) &&
            localMousePosition.x > startButtonPos.x - (startButtonSize.x / 2) &&
            localMousePosition.y < startButtonPos.y + (startButtonSize.y / 2) &&
            localMousePosition.y > startButtonPos.y - (startButtonSize.y / 2))
        {
            startButton.setFillColor(sf::Color::Cyan);
            // Listen for click
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) gameLoop();
            
        } else startButton.setFillColor(sf::Color::Red);
    }
}

void Game::gameLoop()
{
    sf::Music backgroundMusic("../../soundfx/magicmamaliga.mp3");
    backgroundMusic.setVolume(MUSIC_VOLUME);
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

        handleGameInput();

        player.incrementFramesSinceTurn();
        player.storePosition();
        player.updateTail();
        player.updateCorners();

        // ========== Rendering ==========
        // Clear the screen
        window.clear();

        // Draw everything
        window.draw(player);

        for (const auto &segment : player.tailSegments)
            window.draw(segment);

        for (const auto &corner : player.cornerSegments)
            window.draw(corner.shape);

        window.draw(food);

        window.draw(scoreboard.text);

        // Display it
        window.display();
    }
    return;
}

void Game::handleGameInput()
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
    case moveDirection::Up:
        player.moveSnake(moveDirection::Up);
        break;
    case moveDirection::Down:
        player.moveSnake(moveDirection::Down);
        break;
    case moveDirection::Left:
        player.moveSnake(moveDirection::Left);
        break;
    case moveDirection::Right:
        player.moveSnake(moveDirection::Right);
        break;
    }
}