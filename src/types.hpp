#pragma once

enum class moveDirection
{
    Up,
    Down,
    Left,
    Right
};

enum class GameState
{
    MENU,
    USERNAME_INPUT,
    PLAYING,
    PAUSED,
    GAME_OVER,
    QUIT
};