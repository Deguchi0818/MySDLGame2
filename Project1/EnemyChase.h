#pragma once
#include "Enemy.h"


enum class EnemyState {
	PATROL,
	CHASE
};

class EnemyChase : public Enemy
{
public:
	EnemyChase(float x, float y, float w, float h, SDL_Texture* texture)
		: Enemy(x, y, w, h, texture)
	{
		speed = 100.0f;       // スピード
		m_detectRange = 400.0f;// 追いかける範囲 
	}

	void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)override;
	void patrol(float dt);
	void chase(float dt, float dx);


private:
	EnemyState m_currentState = EnemyState::PATROL;
	float m_detectRange = 300.0f;

};

