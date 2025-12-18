#include <random>
#include <algorithm>

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
    setFillColor(sf::Color::Red);
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

    else
        return false;
}

bool Player::collidedWithSelf()
{
    if (tailSegments.size() < 3)
        return false;

    auto playerPos = getPosition();

    for (size_t i = 0; i < tailSegments.size(); ++i)
    {
        if (tailSegments[i].isFrozen)
            continue;

        // Skip the first 2 segments to prevent instant collision after turning
        if (i < 2)
            continue;

        auto segmentPos = tailSegments[i].getPosition();

        float dx = std::abs(playerPos.x - segmentPos.x);
        float dy = std::abs(playerPos.y - segmentPos.y);

        if (dx < PLAYER_SIZE * 1.0f && dy < PLAYER_SIZE * 1.0f)
            return true;
    }

    return false;
}

// Check if player collides with food, return true if it did
bool Player::eat(Food &food)
{
    auto playerPos = getPosition();
    auto foodPos = food.getPosition();

    float dx = std::abs(playerPos.x - foodPos.x);
    float dy = std::abs(playerPos.y - foodPos.y);

    if (dx < (PLAYER_SIZE + FOOD_SIZE) / 2 && dy < (PLAYER_SIZE + FOOD_SIZE) / 2)
        return true;
    else
        return false;
}

void Player::moveSnake(moveDirection direction)
{
    // Create corner if direction changed
    if (direction != previousDirection && tailSegments.size() > 0)
    {
        createCorner();
        framesSinceTurn = 0;
    }
    previousDirection = direction;

    switch (direction)
    {
    case moveDirection::Up:
        move({0.0f, -PLAYER_SPEED});
        break;
    case moveDirection::Down:
        move({0.0f, +PLAYER_SPEED});
        break;
    case moveDirection::Left:
        move({-PLAYER_SPEED, 0.0f});
        break;
    case moveDirection::Right:
        move({+PLAYER_SPEED, 0.0f});
        break;
    }
}

void Player::incrementFramesSinceTurn()
{
    framesSinceTurn++;
}

void Player::createCorner()
{
    // Create a corner segment at the current player position
    sf::RectangleShape corner({PLAYER_SIZE, PLAYER_SIZE});
    corner.setOrigin({PLAYER_SIZE / 2, PLAYER_SIZE / 2});
    corner.setPosition(getPosition());
    corner.setFillColor(getFillColor());

    CornerSegment cornerData;
    cornerData.shape = corner;
    cornerData.position = getPosition();

    cornerSegments.push_back(cornerData);
}

void Player::updateCorners()
{
    if (tailSegments.empty())
    {
        cornerSegments.clear();
        return;
    }

    // Remove corners when the last tail segment has fully passed them
    sf::Vector2f lastTailPos = tailSegments.back().getPosition();

    cornerSegments.erase(
        std::remove_if(cornerSegments.begin(), cornerSegments.end(), [lastTailPos](const CornerSegment &corner)
                       {
                float dx = std::abs(lastTailPos.x - corner.position.x);
                float dy = std::abs(lastTailPos.y - corner.position.y);
                return (dx <= 1.0f && dy <= 1.0f); }),
        cornerSegments.end());
}

void Player::spawnTail()
{
    Tail tail;

    // Spawn at the exact position of the last segment (or player if first)
    if (!tailSegments.empty())
    {
        tail.setPosition(tailSegments.back().getPosition());
    }
    else
    {
        tail.setPosition(getPosition());
    }

    // Freeze until a whole segment has passed to look like it grows
    int framesPerSegment = static_cast<int>(segmentSpacing / PLAYER_SPEED);
    tail.freezeFrames = framesPerSegment;
    tail.isFrozen = true;

    tailSegments.push_back(tail);
}

void Player::storePosition()
{
    positionHistory.insert(positionHistory.begin(), getPosition());
    size_t maxHistory = (tailSegments.size() + 5) * (segmentSpacing / PLAYER_SPEED) + 50;
    if (positionHistory.size() > maxHistory)
        positionHistory.pop_back();
}

void Player::updateTail()
{
    int framesPerSegment = static_cast<int>(segmentSpacing / PLAYER_SPEED);

    for (size_t i = 0; i < tailSegments.size(); ++i)
    {
        if (tailSegments[i].isFrozen)
        {
            tailSegments[i].freezeFrames--;
            if (tailSegments[i].freezeFrames <= 0)
            {
                tailSegments[i].isFrozen = false;
            }
            // Don't update position while frozen
            continue;
        }

        size_t index = (i + 1) * framesPerSegment;
        if (index < positionHistory.size())
        {
            tailSegments[i].setPosition(positionHistory[index]);
        }
    }
}

void Food::spawn(Player &player)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(FOOD_SIZE / 2, RESOLUTION_WIDTH - FOOD_SIZE / 2);
    std::uniform_real_distribution<float> distY(FOOD_SIZE / 2, RESOLUTION_HEIGHT - FOOD_SIZE / 2);

    bool validPosition = false;
    sf::Vector2f newPos;

    // Make sure the food doesn't spawn where the snake is
    while (!validPosition)
    {
        float x = distX(gen);
        float y = distY(gen);
        newPos = {x, y};

        // Check head overlap
        sf::Vector2f playerPos = player.getPosition();
        float dx = std::abs(playerPos.x - newPos.x);
        float dy = std::abs(playerPos.y - newPos.y);
        if (dx < (PLAYER_SIZE + FOOD_SIZE) / 2 && dy < (PLAYER_SIZE + FOOD_SIZE) / 2)
            continue;

        // Check tail overlap
        bool overlap = false;
        for (const auto &segment : player.tailSegments)
        {
            sf::Vector2f segPos = segment.getPosition();
            float sdx = std::abs(segPos.x - newPos.x);
            float sdy = std::abs(segPos.y - newPos.y);
            if (sdx < (PLAYER_SIZE + FOOD_SIZE) / 2 && sdy < (PLAYER_SIZE + FOOD_SIZE) / 2)
            {
                overlap = true;
                break;
            }
        }
        if (!overlap)
            validPosition = true;
    }

    setPosition({newPos});
}