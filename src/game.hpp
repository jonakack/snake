#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <fstream>

#include "types.hpp"
#include "player.hpp"

#define MUSIC_VOLUME 50.0f
#define MAX_FPS 120
#define RESOLUTION_WIDTH 1920u
#define RESOLUTION_HEIGHT 1080u
#define FONT "fonts/ARCADECLASSIC.TTF"

class Button;

class Scoreboard
{
public:
    Scoreboard();

    void increaseScore(int amount);
    void resetScore();
    int getCurrentScore() const { return currentScore; }
    sf::Text text;

private:
    sf::Font font;
    int currentScore{0};
};

class Game
{
public:
    Game();

    void run(); // Main game loop with state machine
    
    // State-specific methods
    void handleMenuState();
    void handleUsernameInputState();
    void handlePlayingState();
    void handlePausedState();
    void handleGameOverState();
    
    void changeState(GameState newState);
    void resetGame();
    
    // Username and high score methods
    void loadHighScore();
    void saveHighScore();
    void checkAndUpdateHighScore();
    
    // Rendering methods
    void drawMenu();
    void drawUsernameInput();
    void drawGame();
    void drawPause();
    void drawGameOver();
    
    // Input handling methods
    void handleMenuInput();
    void handleUsernameInput();
    void handleGameInput();
    void handlePauseInput();
    void handleGameOverInput();
    
    // Utility methods
    bool isGameOver();
    void loadBackgroundMusic(const std::string& filename);

private:
    sf::RenderWindow window;
    Player player;
    Food food;
    Scoreboard scoreboard;
    moveDirection direction;
    
    // State machine variables
    GameState currentState;
    GameState nextState;
    bool stateChanged;
    
    // Resources
    sf::Music backgroundMusic;
    sf::Texture menuBackgroundTexture;
    sf::Sprite menuBackgroundSprite;
    sf::Texture gameBackgroundTexture;
    sf::Sprite gameBackgroundSprite;
    sf::Font font;
    
    // UI Elements
    Button* startButton;
    Button* exitButton;
    Button* restartButton;
    Button* menuButton;
    Button* resumeButton;
    
    // Background zoom effect for menu
    float backgroundZoom = 1.0f;
    bool zoomingIn = true;
    const float minZoom = 1.0f;
    const float maxZoom = 1.10f;
    const float zoomSpeed = 0.00005f;
    
    // Game Over text
    sf::Text gameOverText;
    sf::Text scoreText;
    sf::Text instructionText;
    
    // Username and high score system
    std::string currentUsername;
    std::string inputUsername;
    int highScore;
    std::string highScoreUsername;
    bool isNewHighScore;
};

class Button : public sf::RectangleShape
{
public:
    Button(sf::Vector2f size, std::string buttonText);
    
    void draw(sf::RenderWindow& window);
    void setHovered(bool hovered);
    void setPressed(bool pressed);
    void updateText();
    bool contains(sf::Vector2f point) const;
    
    sf::Text text;
    
private:
    sf::Font font;
    std::string buttonText;
    bool isHovered = false;
    bool isPressed = false;
    
    // Colors for different states
    sf::Color normalColor = sf::Color(34, 139, 34);       // Forest green
    sf::Color hoverColor = sf::Color(144, 238, 144);      // Light green
    sf::Color pressedColor = sf::Color(0, 100, 0);        // Dark green
    sf::Color textColor = sf::Color::White;
    sf::Color borderColor = sf::Color::Black;
};