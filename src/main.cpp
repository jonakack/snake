#include <SFML/Graphics.hpp>

#define RESOLUTION_WIDTH 1920u
#define RESOLUTION_HEIGHT 1080u
#define PLAYER_SPEED 5.0f
#define PLAYER_SIZE 50.0f

enum class moveDirection
{
    Up,
    Down,
    Left,
    Right
};

int main()
{
    // Create the player square
    sf::RectangleShape player({PLAYER_SIZE, PLAYER_SIZE});
    player.setOrigin({PLAYER_SIZE / 2, PLAYER_SIZE / 2});
    player.setPosition({RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2});

    // Initialize move direction
    moveDirection direction = moveDirection::Right;

    // Create the screen
    auto window = sf::RenderWindow(sf::VideoMode({RESOLUTION_WIDTH, RESOLUTION_HEIGHT}), "Snake");
    window.setFramerateLimit(120);


    // =============== Game loop ===============
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            // Closes program
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

        // Check if player died (collided with something)
        auto playerPos = player.getPosition();

        if (playerPos.y - (PLAYER_SIZE / 2) < 0 ||
            playerPos.y + (PLAYER_SIZE / 2) > RESOLUTION_HEIGHT ||
            playerPos.x - (PLAYER_SIZE / 2) < 0 ||
            playerPos.x + (PLAYER_SIZE / 2) > RESOLUTION_WIDTH)
            return 1;

        // ========== Rendering ==========
        // Clear the screen
        window.clear();

        // Draw everything
        window.draw(player);

        // Display it
        window.display();
    }
}
