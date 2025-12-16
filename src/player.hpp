#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#define PLAYER_SPEED 5.0f
#define PLAYER_SIZE 50.0f
#define FOOD_SIZE 25.0f

constexpr int framesPerSegment = 10;

class Food : public sf::RectangleShape
{
public:
    Food();

    void spawn();
};

class Tail : public sf::RectangleShape
{
public:
    Tail();
};

class Player : public sf::RectangleShape
{
public:
    Player();

    std::vector<sf::Vector2f> positionHistory;
    std::vector<Tail> tailSegments;
    

    bool collidedWithBorder();
    bool eat(Food &food);
    void spawnTail();
    void storePosition();
    void updateTail();


};