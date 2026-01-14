#pragma once
#include "GroundedState.h"

class Player;

class RunState :public GroundedState
{
public:
	void handleInput(Player& player, const bool* keys) override;
	void update(Player& player, float dt) override;
};

