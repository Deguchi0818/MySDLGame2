#pragma once
#include "PlayerState.h"

class Player;

class GroundedState : public PlayerState
{
public:
	void handleInput(Player& player, const bool* keys) override;
	void update(Player& player, float dt) override;
};

