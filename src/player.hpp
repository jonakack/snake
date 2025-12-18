#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "types.hpp"

#define PLAYER_SPEED 4.0f // Pixels per second
#define PLAYER_SIZE 60.0f // X and Y pixel length
#define FOOD_SIZE 25.0f

// Forward declarations
class Tail;
class Food;
struct CornerSegment;

constexpr int framesPerSegment = 10;

class Player : public sf::RectangleShape
{
public:
    Player();

    std::vector<sf::Vector2f> positionHistory;
    std::vector<Tail> tailSegments;
    std::vector<CornerSegment> cornerSegments;
    int framesSinceTurn = 0;
    
    bool collidedWithBorder();
    bool collidedWithSelf();
    bool eat(Food &food);
    void moveSnake(moveDirection direction);
    void createCorner();
    void updateCorners();
    void spawnTail();
    void storePosition();
    void updateTail();
    void incrementFramesSinceTurn();

private:
    float segmentSpacing = static_cast<float>(static_cast<int>(PLAYER_SIZE / PLAYER_SPEED)) * PLAYER_SPEED;
    int frameCount = 0;
    moveDirection previousDirection = moveDirection::Right;
};

class Tail : public sf::RectangleShape
{
public:
    Tail();

private:
    int freezeFrames {0};
    bool isFrozen {true};

    friend class Player;
};

struct CornerSegment {
    sf::RectangleShape shape;
    sf::Vector2f position;
};

class Food : public sf::RectangleShape
{
public:
    Food();

    void spawn(Player& player);
};