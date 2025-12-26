#pragma once
#include "Enemy.h"


enum class EnemyState {
	PATOROL,
	CHASE
};

class EnemyChase : public Enemy
{
public:
	EnemyChase(float x, float y, float w, float h)
		: Enemy(x, y, w, h)
	{
		speed = 100.0f;       
		m_detectRange = 400.0f; 
	}

	void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)override;
	void patrol(float dt);
	void chase(float dt, float dx);

private:
	EnemyState m_currentState = EnemyState::PATOROL;
	float m_detectRange = 300.0f;

};

