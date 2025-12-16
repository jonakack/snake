#include <random>

#include "player.hpp"
#include "game.hpp"

// Constructor
Player::Player()
    : sf::RectangleShape({PLAYER_SIZE, PLAYER_SIZE})
{
    setOrigin({PLAYER_SIZE / 2, PLAYER_SIZE / 2});
    setPosition({RESOLUTION_WIDTH / 2, RESOLUTION_HEIGHT / 2});
}

// Constructor
Food::Food()
    : sf::RectangleShape({FOOD_SIZE, FOOD_SIZE})
{
    setOrigin({FOOD_SIZE / 2, FOOD_SIZE / 2});
    spawn();
}

// Constructor
Tail::Tail()
    : sf::RectangleShape({PLAYER_SIZE, PLAYER_SIZE})
{
    setOrigin({PLAYER_SIZE / 2, PLAYER_SIZE / 2});
}

// Check if player died (collided with window borders)
bool Player::collidedWithBorder()
{
        auto playerPos = getPosition();

        if (playerPos.y - (PLAYER_SIZE / 2) < 0 ||
            playerPos.y + (PLAYER_SIZE / 2) > RESOLUTION_HEIGHT ||
            playerPos.x - (PLAYER_SIZE / 2) < 0 ||
            playerPos.x + (PLAYER_SIZE / 2) > RESOLUTION_WIDTH)
            return true;

        else return false;
}

// Check if player collides with food, return true if it did
bool Player::eat(Food &food)
{
    auto playerPos = getPosition();
    auto foodPos = food.getPosition();

    float dx = std::abs(playerPos.x - foodPos.x);
    float dy = std::abs(playerPos.y - foodPos.y);

    // If the distance between centers is less than half the sum of sizes, they overlap
    if (dx < (PLAYER_SIZE + FOOD_SIZE) / 2 && dy < (PLAYER_SIZE + FOOD_SIZE) / 2)
        return true;
    else
        return false;
}

void Player::spawnTail()
{
    Tail tail;
    tail.setPosition(positionHistory.front());
    tailSegments.push_back(tail);
}

// Store position history each frame
void Player::storePosition()
{
    positionHistory.insert(positionHistory.begin(), getPosition());
    if (positionHistory.size() > (tailSegments.size() + 1) * framesPerSegment)
        positionHistory.pop_back();
}

void Player::updateTail()
{
    for (size_t i = 0; i < tailSegments.size(); ++i)
    {
        size_t index = (i + 1) * framesPerSegment;
        if (index < positionHistory.size())
            tailSegments[i].setPosition(positionHistory[index]);
    }
}

void Food::spawn()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(FOOD_SIZE / 2, RESOLUTION_WIDTH - FOOD_SIZE / 2);
    std::uniform_real_distribution<float> distY(FOOD_SIZE / 2, RESOLUTION_HEIGHT - FOOD_SIZE / 2);

    float x = distX(gen);
    float y = distY(gen);

    setPosition({x, y});
}