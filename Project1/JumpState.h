#pragma once
#include "AirborneState.h"

class Player;

class JumpState :public AirborneState
{
public:
	void handleInput(Player& player, const bool* keys) override;
	void update(Player& player, float dt) override;
};
