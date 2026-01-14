#pragma once
#include <SDL3/SDL.h>

class Player;

class PlayerState
{
public:
	virtual ~PlayerState() = default;
	virtual void handleInput(Player& player, const bool* keys) = 0;
	virtual void update(Player& player, float dt) = 0;
};

