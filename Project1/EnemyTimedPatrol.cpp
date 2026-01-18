#include "EnemyTimedPatrol.h"

void EnemyTimedPatrol::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
	EnemyWalking::update(dt, playerRect, player, grounds);
	turn(dt);

}

void EnemyTimedPatrol::turn(float dt)
{
	m_turnTimer += dt;

	if (m_turnTimer > m_turnInterval) 
	{
		m_turnTimer = 0.0f;
		m_velX *= -1;
	}
}

void EnemyTimedPatrol::render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset)
{
	EnemyWalking::render(renderer, cameraOffset);
}
