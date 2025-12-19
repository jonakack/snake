#include <iostream>

#include "game.hpp"
#include "player.hpp"

sf::Font g_font(FONT);

Game::Game()
    : window(sf::RenderWindow(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "Snake")), // Add "sf::State::Fullscreen" for fullscren mode
      currentState(GameState::MENU),
      nextState(GameState::MENU),
      stateChanged(false),
      direction(moveDirection::Right),
      font(FONT),
      menuBackgroundTexture("../../textures/mountain/fullmountain.png"),
      menuBackgroundSprite(menuBackgroundTexture),
      gameBackgroundTexture("../../textures/greenpixels.jpg"),
      gameBackgroundSprite(gameBackgroundTexture),
      gameOverText(font, "Game Over!", 80),
      scoreText(font),
      instructionText(font, "Press   R   to   Restart   or   M   for   Menu", 40)
{
    window.setFramerateLimit(MAX_FPS);
    
    // Initialize UI elements
    startButton = new Button({400.0f, 100.0f}, "Start");
    startButton->setOrigin({startButton->getSize().x / 2, startButton->getSize().y / 2});
    startButton->setPosition({RESOLUTION_WIDTH / 2, 500.0f});
    startButton->updateText();
    
    exitButton = new Button({400.0f, 100.0f}, "Exit Game");
    exitButton->setOrigin({exitButton->getSize().x / 2, exitButton->getSize().y / 2});
    exitButton->setPosition({RESOLUTION_WIDTH / 2, 600.0f});
    exitButton->updateText();
    
    restartButton = new Button({400.0f, 100.0f}, "Restart");
    restartButton->setOrigin({restartButton->getSize().x / 2, restartButton->getSize().y / 2});
    restartButton->setPosition({RESOLUTION_WIDTH / 2, 550.0f});
    restartButton->updateText();
    
    menuButton = new Button({400.0f, 100.0f}, "Main Menu");
    menuButton->setOrigin({menuButton->getSize().x / 2, menuButton->getSize().y / 2});
    menuButton->setPosition({RESOLUTION_WIDTH / 2, 650.0f});
    menuButton->updateText();
    
    resumeButton = new Button({400.0f, 100.0f}, "Resume");
    resumeButton->setOrigin({resumeButton->getSize().x / 2, resumeButton->getSize().y / 2});
    resumeButton->setPosition({RESOLUTION_WIDTH / 2, 400.0f});
    resumeButton->updateText();
    
    // Position text elements
    gameOverText.setPosition({RESOLUTION_WIDTH / 2 - 200, 200});
    instructionText.setPosition({RESOLUTION_WIDTH / 2 - 400, 700});
    instructionText.setOutlineThickness(2);
    instructionText.setOutlineColor(sf::Color::Black);
    
    // Set up background sprites
    sf::Vector2u menuTextureSize = menuBackgroundTexture.getSize();
    menuBackgroundSprite.setScale({static_cast<float>(RESOLUTION_WIDTH) / menuTextureSize.x, static_cast<float>(RESOLUTION_HEIGHT) / menuTextureSize.y});
    
    sf::Vector2u gameTextureSize = gameBackgroundTexture.getSize();
    gameBackgroundSprite.setScale({static_cast<float>(RESOLUTION_WIDTH) / gameTextureSize.x, static_cast<float>(RESOLUTION_HEIGHT) / gameTextureSize.y});
    
    // Initialize username and high score system
    currentUsername = "";
    inputUsername = "";
    highScore = 0;
    highScoreUsername = "Unknown";
    isNewHighScore = false;
    
    loadHighScore();
}

Scoreboard::Scoreboard()
    : font(FONT), text(font, "Current score   0", 50)
{
    text.setPosition({RESOLUTION_WIDTH / 15, RESOLUTION_HEIGHT / 15});
}

Button::Button(sf::Vector2f size, std::string buttonText)
    : sf::RectangleShape(size), buttonText(buttonText), font(FONT), text(font, buttonText, 36)
{
    setFillColor(normalColor);
    setOutlineThickness(3.0f);
    setOutlineColor(borderColor);
    
    text.setFillColor(textColor);
    text.setOutlineThickness(1.0f);
    text.setOutlineColor(sf::Color::Black);
    
    updateText();
}

void Button::draw(sf::RenderWindow& window)
{
    window.draw(*this);  
    window.draw(text);   
}

void Button::setHovered(bool hovered)
{
    isHovered = hovered;
    
    if (isPressed) {
        setFillColor(pressedColor);
    } else if (isHovered) {
        setFillColor(hoverColor);
    } else {
        setFillColor(normalColor);
    }
}

void Button::setPressed(bool pressed)
{
    isPressed = pressed;
    
    if (isPressed) {
        setFillColor(pressedColor);
        // Slight offset for pressed effect
        text.setPosition({text.getPosition().x + 2, text.getPosition().y + 2});
    } else {
        if (isHovered) {
            setFillColor(hoverColor);
        } else {
            setFillColor(normalColor);
        }
        updateText(); // Reset text position
    }
}

void Button::updateText()
{
    // Center text inside button
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::FloatRect buttonBounds = getGlobalBounds();
    text.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f + textBounds.position.y});
    
    // Position text at center of button
    text.setPosition({
        buttonBounds.position.x + buttonBounds.size.x / 2.0f,
        buttonBounds.position.y + buttonBounds.size.y / 2.0f
    });
}

bool Button::contains(sf::Vector2f point) const
{
    return getGlobalBounds().contains(point);
}

void Scoreboard::increaseScore(int amount)
{
    currentScore += amount;

    std::string currentScoreStr = std::to_string(currentScore);
    text.setString("Current score   " + currentScoreStr);
}

void Scoreboard::resetScore()
{
    currentScore = 0;
    text.setString("Current score   0");
}

// ==================== STATE MACHINE ====================

void Game::run()
{
    // Load initial menu music
    loadBackgroundMusic("../../soundfx/dualofthefates.mp3");
    backgroundMusic.play();
    
    while (window.isOpen() && currentState != GameState::QUIT) {
        if (stateChanged) {
            GameState previousState = currentState;
            currentState = nextState;
            stateChanged = false;
            
            switch (currentState) {
                case GameState::MENU:
                    if (previousState != GameState::MENU) {
                        loadBackgroundMusic("../../soundfx/dualofthefates.mp3");
                        backgroundMusic.play();
                    }
                    break;
                case GameState::PLAYING:
                    loadBackgroundMusic("../../soundfx/magicmamaliga.mp3");
                    backgroundMusic.play();
                    food.spawn(player);
                    break;
                case GameState::PAUSED:
                    backgroundMusic.pause();
                    break;
                case GameState::GAME_OVER:
                    backgroundMusic.stop();
                    checkAndUpdateHighScore();
                    break;
            }
        }
        
        // Handle current state
        switch (currentState) {
            case GameState::MENU:
                handleMenuState();
                break;
            case GameState::USERNAME_INPUT:
                handleUsernameInputState();
                break;
            case GameState::PLAYING:
                handlePlayingState();
                break;
            case GameState::PAUSED:
                handlePausedState();
                break;
            case GameState::GAME_OVER:
                handleGameOverState();
                break;
        }
    }
}

void Game::changeState(GameState newState)
{
    nextState = newState;
    stateChanged = true;
}

void Game::resetGame()
{
    // Reset basics
    direction = moveDirection::Right;
    scoreboard.resetScore();
    player.setPosition({RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2});
    player.tailSegments.clear();
    player.cornerSegments.clear();
    player.positionHistory.clear();
    player.framesSinceTurn = 0;
}

bool Game::isGameOver()
{
    return player.collidedWithBorder() || player.collidedWithSelf();
}

void Game::loadBackgroundMusic(const std::string& filename)
{
    // Stop current music
    backgroundMusic.stop();
    
    if (!backgroundMusic.openFromFile(filename)) {
        std::cerr << "Error loading music file: " << filename << std::endl;
        return;
    }
    
    backgroundMusic.setVolume(MUSIC_VOLUME);
    backgroundMusic.setLooping(true);
}

// ========== STATE HANDLERS ==========

void Game::handleMenuState()
{
    handleMenuInput();
    
    // Update background zoom effect
    if (zoomingIn) {
        backgroundZoom += zoomSpeed;
        if (backgroundZoom >= maxZoom) {
            backgroundZoom = maxZoom;
            zoomingIn = false;
        }
    } else {
        backgroundZoom -= zoomSpeed;
        if (backgroundZoom <= minZoom) {
            backgroundZoom = minZoom;
            zoomingIn = true;
        }
    }
    
    drawMenu();
}

void Game::handlePlayingState()
{
    // Handle input including movement
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }
        
        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                changeState(GameState::PAUSED);
                return;
            }
            
            // Add movement controls
            const int minFramesBetweenTurns = 2 * framesPerSegment;
            switch (keyPressed->code) {
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
    
    if (isGameOver()) {
        changeState(GameState::GAME_OVER);
        return;
    }
    
    if (player.eat(food)) {
        player.spawnTail();
        food.spawn(player);
        scoreboard.increaseScore(10);
    }
    
    // Update player movement
    switch (direction) {
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
    
    player.incrementFramesSinceTurn();
    player.storePosition();
    player.updateTail();
    player.updateCorners();
    
    // Draw everything
    window.clear();
    window.draw(gameBackgroundSprite);
    window.draw(player);
    
    for (const auto &segment : player.tailSegments)
        window.draw(segment);

    for (const auto &corner : player.cornerSegments)
        window.draw(corner.shape);
        
    window.draw(food);
    window.draw(scoreboard.text);
    window.display();
}

void Game::handlePausedState()
{
    handlePauseInput();
    drawPause();
}

void Game::handleGameOverState()
{
    handleGameOverInput();
    drawGameOver();
}

// ========== INPUT HANDLERS ==========

void Game::handleMenuInput()
{
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }

        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                changeState(GameState::USERNAME_INPUT);
                return;
            }
        }

        // Handle mouse button press
        if (auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                if (startButton->contains(mouseWorldPos)) {
                    startButton->setPressed(true);
                }
                if (exitButton->contains(mouseWorldPos)) {
                    exitButton->setPressed(true);
                }
            }
        }
        
        // Handle mouse button release
        if (auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                startButton->setPressed(false);
                exitButton->setPressed(false);
                
                if (startButton->contains(mouseWorldPos)) {
                    changeState(GameState::USERNAME_INPUT);
                    return;
                }
                if (exitButton->contains(mouseWorldPos)) {
                    changeState(GameState::QUIT);
                    return;
                }
            }
        }
    }

    // Handle button hover effects
    if (startButton != nullptr) {
        bool startMouseOver = startButton->contains(mouseWorldPos);
        startButton->setHovered(startMouseOver);
    }
    if (exitButton != nullptr) {
        bool exitMouseOver = exitButton->contains(mouseWorldPos);
        exitButton->setHovered(exitMouseOver);
    }
}

void Game::handleGameInput()
{
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }

        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            const int minFramesBetweenTurns = 2 * framesPerSegment;

            switch (keyPressed->code) {
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
            case sf::Keyboard::Key::Escape:
                changeState(GameState::PAUSED);
                return;
            }
        }
    }

    // Update player movement
    switch (direction) {
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

void Game::handlePauseInput()
{
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }

        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
            case sf::Keyboard::Key::P:
                backgroundMusic.play();
                changeState(GameState::PLAYING);
                return;
            case sf::Keyboard::Key::M:
                changeState(GameState::MENU);
                return;
            case sf::Keyboard::Key::Q:
                changeState(GameState::QUIT);
                return;
            }
        }
    }
}

void Game::handleGameOverInput()
{
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }

        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyPressed->code) {
            case sf::Keyboard::Key::R:
                resetGame();
                changeState(GameState::PLAYING);
                return;
            case sf::Keyboard::Key::M:
                changeState(GameState::MENU);
                return;
            case sf::Keyboard::Key::Enter:
                resetGame();
                changeState(GameState::PLAYING);
                return;
            }
        }

        // Handle mouse button press
        if (auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                if (restartButton->contains(mouseWorldPos)) {
                    restartButton->setPressed(true);
                }
                if (menuButton->contains(mouseWorldPos)) {
                    menuButton->setPressed(true);
                }
            }
        }
        
        // Handle mouse button release
        if (auto *mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                restartButton->setPressed(false);
                menuButton->setPressed(false);
                
                if (restartButton->contains(mouseWorldPos)) {
                    resetGame();
                    changeState(GameState::PLAYING);
                    return;
                }
                
                if (menuButton->contains(mouseWorldPos)) {
                    changeState(GameState::MENU);
                    return;
                }
            }
        }
    }

    // Handle button hover effects
    bool restartHovered = restartButton->contains(mouseWorldPos);
    restartButton->setHovered(restartHovered);

    bool menuHovered = menuButton->contains(mouseWorldPos);
    menuButton->setHovered(menuHovered);
}

// ========== DRAWING METHODS ==========

void Game::drawMenu()
{
    window.clear();
    
    // Apply zoom effect to background
    menuBackgroundSprite.setScale({backgroundZoom, backgroundZoom});
    
    // Center the scaled background
    sf::FloatRect bgBounds = menuBackgroundSprite.getGlobalBounds();
    menuBackgroundSprite.setPosition({
        (RESOLUTION_WIDTH - bgBounds.size.x) / 2.0f,
        (RESOLUTION_HEIGHT - bgBounds.size.y) / 2.0f
    });
    
    window.draw(menuBackgroundSprite);
    startButton->draw(window);
    exitButton->draw(window);
    
    // Add title text
    sf::Text titleText(font, "SNAKE  GAME", 200);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({RESOLUTION_WIDTH / 2.0f, 150});
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineThickness(4);
    titleText.setOutlineColor(sf::Color::Black);
    window.draw(titleText);
    
    // Add instruction text
    sf::Text startText(font, "Press   Enter   to   Start   or   Click   the   Button", 40);
    sf::FloatRect startBounds = startText.getLocalBounds();
    startText.setOrigin({startBounds.size.x / 2.0f, startBounds.size.y / 2.0f});
    startText.setPosition({RESOLUTION_WIDTH / 2.0f, 350});
    startText.setFillColor(sf::Color::White);
    startText.setOutlineThickness(2);
    startText.setOutlineColor(sf::Color::Black);
    window.draw(startText);
    
    // Add high score display
    std::string highScoreStr = "High   Score   " + std::to_string(highScore) + "   by   " + highScoreUsername;
    sf::Text highScoreText(font, highScoreStr, 35);
    sf::FloatRect highScoreBounds = highScoreText.getLocalBounds();
    highScoreText.setOrigin({highScoreBounds.size.x / 2.0f, highScoreBounds.size.y / 2.0f});
    highScoreText.setPosition({RESOLUTION_WIDTH / 2.0f, 750});
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setOutlineThickness(2);
    highScoreText.setOutlineColor(sf::Color::Black);
    window.draw(highScoreText);
    
    window.display();
}

void Game::drawGame()
{
    window.clear();
    window.draw(gameBackgroundSprite);

    // Draw everything
    window.draw(player);

    for (const auto &segment : player.tailSegments)
        window.draw(segment);

    for (const auto &corner : player.cornerSegments)
        window.draw(corner.shape);

    window.draw(food);
    window.draw(scoreboard.text);

    window.display();
}

void Game::drawPause()
{
    window.clear();
    window.draw(menuBackgroundSprite);
    
    // PAUSED text
    sf::Text pauseText(font, "PAUSED", 100);
    sf::FloatRect pauseBounds = pauseText.getLocalBounds();
    pauseText.setOrigin({pauseBounds.size.x / 2.0f, pauseBounds.size.y / 2.0f});
    pauseText.setPosition({RESOLUTION_WIDTH / 2.0f, 300});
    pauseText.setFillColor(sf::Color::White);
    pauseText.setOutlineThickness(2);
    pauseText.setOutlineColor(sf::Color::Black);
    window.draw(pauseText);
    
    // Resume instructions
    sf::Text resumeText(font, "Press   Esc   or   P   to   Resume", 40);
    sf::FloatRect resumeBounds = resumeText.getLocalBounds();
    resumeText.setOrigin({resumeBounds.size.x / 2.0f, resumeBounds.size.y / 2.0f});
    resumeText.setPosition({RESOLUTION_WIDTH / 2.0f, 450});
    resumeText.setFillColor(sf::Color::White);
    resumeText.setOutlineThickness(2);
    resumeText.setOutlineColor(sf::Color::Black);
    window.draw(resumeText);
    
    // Menu instructions
    sf::Text menuText(font, "Press   M   for   Main   Menu", 40);
    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setOrigin({menuBounds.size.x / 2.0f, menuBounds.size.y / 2.0f});
    menuText.setPosition({RESOLUTION_WIDTH / 2.0f, 520});
    menuText.setFillColor(sf::Color::White);
    menuText.setOutlineThickness(2);
    menuText.setOutlineColor(sf::Color::Black);
    window.draw(menuText);
    
    // Exit instructions
    sf::Text exitText(font, "Press   Q   to   Exit   Game", 40);
    sf::FloatRect exitBounds = exitText.getLocalBounds();
    exitText.setOrigin({exitBounds.size.x / 2.0f, exitBounds.size.y / 2.0f});
    exitText.setPosition({RESOLUTION_WIDTH / 2.0f, 590});
    exitText.setFillColor(sf::Color::White);
    exitText.setOutlineThickness(2);
    exitText.setOutlineColor(sf::Color::Black);
    window.draw(exitText);
    
    window.display();
}

void Game::drawGameOver()
{
    window.clear();
    window.draw(menuBackgroundSprite);
    
    // Draw game over text
    sf::Text gameOverDisplay = gameOverText;
    sf::FloatRect gameOverBounds = gameOverDisplay.getLocalBounds();
    gameOverDisplay.setOrigin({gameOverBounds.size.x / 2.0f, gameOverBounds.size.y / 2.0f});
    gameOverDisplay.setPosition({RESOLUTION_WIDTH / 2.0f, 200});
    gameOverDisplay.setOutlineThickness(2);
    gameOverDisplay.setOutlineColor(sf::Color::Black);
    window.draw(gameOverDisplay);
    
    // Update and draw final score
    scoreText = sf::Text(font, "Final  Score  " + std::to_string(scoreboard.getCurrentScore()), 60);
    sf::FloatRect scoreBounds = scoreText.getLocalBounds();
    scoreText.setOrigin({scoreBounds.size.x / 2.0f, scoreBounds.size.y / 2.0f});
    scoreText.setPosition({RESOLUTION_WIDTH / 2.0f, 350});
    scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineThickness(2);
    scoreText.setOutlineColor(sf::Color::Black);
    window.draw(scoreText);
    
    // Show new high score message if applicable
    if (isNewHighScore) {
        sf::Text newHighScoreText(font, "NEW HIGH SCORE!", 50);
        sf::FloatRect newHighScoreBounds = newHighScoreText.getLocalBounds();
        newHighScoreText.setOrigin({newHighScoreBounds.size.x / 2.0f, newHighScoreBounds.size.y / 2.0f});
        newHighScoreText.setPosition({RESOLUTION_WIDTH / 2.0f, 420});
        newHighScoreText.setFillColor(sf::Color::Yellow);
        newHighScoreText.setOutlineThickness(2);
        newHighScoreText.setOutlineColor(sf::Color::Black);
        window.draw(newHighScoreText);
    }
    
    // Draw instruction text 
    sf::Text instructionsDisplay = instructionText;
    sf::FloatRect instructionBounds = instructionsDisplay.getLocalBounds();
    instructionsDisplay.setOrigin({instructionBounds.size.x / 2.0f, instructionBounds.size.y / 2.0f});
    instructionsDisplay.setPosition({RESOLUTION_WIDTH / 2.0f, 750});
    window.draw(instructionsDisplay);
    
    restartButton->draw(window);
    menuButton->draw(window);
    
    window.display();
}

// ========== USERNAME INPUT AND HIGH SCORE METHODS ==========

void Game::handleUsernameInputState()
{
    handleUsernameInput();
    drawUsernameInput();
}

void Game::handleUsernameInput()
{
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            changeState(GameState::QUIT);
            return;
        }

        if (auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (!inputUsername.empty()) {
                    currentUsername = inputUsername;
                    resetGame();
                    changeState(GameState::PLAYING);
                    return;
                }
            } else if (keyPressed->code == sf::Keyboard::Key::Escape) {
                changeState(GameState::MENU);
                return;
            } else if (keyPressed->code == sf::Keyboard::Key::Backspace) {
                if (!inputUsername.empty()) {
                    inputUsername.pop_back();
                }
            }
        }

        if (auto *textEntered = event->getIf<sf::Event::TextEntered>()) {
            char unicode = static_cast<char>(textEntered->unicode);
            if (unicode >= 32 && unicode < 127 && inputUsername.length() < 15) { // Printable ASCII characters
                inputUsername += unicode;
            }
        }
    }
}

void Game::drawUsernameInput()
{
    window.clear();
    
    // Apply zoom effect to background
    menuBackgroundSprite.setScale({backgroundZoom, backgroundZoom});
    
    // Center the scaled background
    sf::FloatRect bgBounds = menuBackgroundSprite.getGlobalBounds();
    menuBackgroundSprite.setPosition({
        (RESOLUTION_WIDTH - bgBounds.size.x) / 2.0f,
        (RESOLUTION_HEIGHT - bgBounds.size.y) / 2.0f
    });
    
    window.draw(menuBackgroundSprite);
    
    // Title text
    sf::Text titleText(font, "Enter   Username", 120);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({RESOLUTION_WIDTH / 2.0f, 200});
    titleText.setFillColor(sf::Color::White);
    titleText.setOutlineThickness(3);
    titleText.setOutlineColor(sf::Color::Black);
    window.draw(titleText);
    
    // Username input box
    sf::RectangleShape inputBox({600, 80});
    inputBox.setPosition({(RESOLUTION_WIDTH - 600) / 2.0f, 350});
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineThickness(3);
    inputBox.setOutlineColor(sf::Color::Black);
    window.draw(inputBox);
    
    // Username text
    std::string displayText = inputUsername + "_"; // Add cursor
    sf::Text usernameText(font, displayText, 60);
    usernameText.setPosition({(RESOLUTION_WIDTH - 600) / 2.0f + 20, 355});
    usernameText.setFillColor(sf::Color::Black);
    window.draw(usernameText);
    
    // Instructions
    sf::Text instructionText(font, "Type   your   username   and   press   Enter   to   start", 40);
    sf::FloatRect instructionBounds = instructionText.getLocalBounds();
    instructionText.setOrigin({instructionBounds.size.x / 2.0f, instructionBounds.size.y / 2.0f});
    instructionText.setPosition({RESOLUTION_WIDTH / 2.0f, 500});
    instructionText.setFillColor(sf::Color::White);
    instructionText.setOutlineThickness(2);
    instructionText.setOutlineColor(sf::Color::Black);
    window.draw(instructionText);
    
    sf::Text escText(font, "Press   Escape   to   go   back", 30);
    sf::FloatRect escBounds = escText.getLocalBounds();
    escText.setOrigin({escBounds.size.x / 2.0f, escBounds.size.y / 2.0f});
    escText.setPosition({RESOLUTION_WIDTH / 2.0f, 600});
    escText.setFillColor(sf::Color::White);
    escText.setOutlineThickness(2);
    escText.setOutlineColor(sf::Color::Black);
    window.draw(escText);
    
    window.display();
}

void Game::loadHighScore()
{
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        std::getline(file, highScoreUsername);
        // Remove leading whitespace from username
        size_t start = highScoreUsername.find_first_not_of(" \t");
        if (start != std::string::npos) {
            highScoreUsername = highScoreUsername.substr(start);
        }
        if (highScoreUsername.empty()) {
            highScoreUsername = "Unknown";
        }
        file.close();
    }
}

void Game::saveHighScore()
{
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore << " " << highScoreUsername;
        file.close();
    }
}

void Game::checkAndUpdateHighScore()
{
    int currentScore = scoreboard.getCurrentScore();
    if (currentScore > highScore) {
        highScore = currentScore;
        highScoreUsername = currentUsername;
        isNewHighScore = true;
        saveHighScore();
    } else {
        isNewHighScore = false;
    }
}