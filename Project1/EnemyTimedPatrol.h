#pragma once
#include "EnemyWalking.h"

class EnemyTimedPatrol : public EnemyWalking
{
public:
	using EnemyWalking::EnemyWalking;

	void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) override;
	void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) override;

	virtual void turn(float dt);

protected:
	float m_turnTimer = 0.0f;
	float m_turnInterval = static_cast<float>(rand() % 300) / 100.0f;

};

