#pragma once
#include "Enemy.h"

class EnemyWalking :public Enemy
{
public:
	using Enemy::Enemy;

	void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) override;
};

