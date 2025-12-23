#pragma once
#include "Enemy.h"

class WalkingEnemy :public Enemy
{
public:
	using Enemy::Enemy;

	void update(float dt, const SDL_FRect& playerRect, const vector<BoxCollider>& grounds) override;
};

