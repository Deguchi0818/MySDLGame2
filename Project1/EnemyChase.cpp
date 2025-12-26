#include "EnemyChase.h"


void EnemyChase::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds)
{
	float dx = player.collider().rect().x - m_collider.rect().x;
	float distance = abs(dx);

	if (distance < m_detectRange) 
	{
		m_currentState = EnemyState::CHASE;
	}
	else
	{
		m_currentState = EnemyState::PATOROL;
	}

	switch (m_currentState) 
	{
	case EnemyState::PATOROL:
		patrol(dt);
		break;
	case EnemyState::CHASE:
		chase(dt, dx);
	}

	m_velY += 1500.0f * dt;

	SDL_FRect r = m_collider.rect();
	r.x += m_velX * dt;
	r.y += m_velY * dt;
	m_collider.setPosition(r.x, r.y);

	BoxCollider::resolveCollision(m_collider, m_velX, m_velY, grounds);
}

void EnemyChase::patrol(float dt)
{
	m_velX = 0;

}

void EnemyChase::chase(float dt, float dx)
{
	if (dx > 0) {
		m_velX = 1.0f;
	}
	else 
	{
		m_velX = -1.0f;
	}

	float chaseSpeed = speed * 1.5f;

	m_velX *= chaseSpeed;
	
}