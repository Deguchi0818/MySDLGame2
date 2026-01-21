#include "EnemyShooter.h"
#include <algorithm>
using namespace std;
void EnemyShooter::update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) 
{
	EnemyWalking::update(dt, playerRect, player, grounds);

	for (auto& b : m_bullets) {
		b->update(dt, grounds);
	}

	m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(),
		[](const unique_ptr<Bullet>& b) { return !b->isActive(); }), m_bullets.end());

	if (!isDead() && !isStunned()) 
	{
		m_shootTimer += dt;
		if (m_shootTimer >= m_shootInterval) 
		{
			shoot(playerRect);
			m_shootTimer = 0.0f;
		}
	}
}

void EnemyShooter::shoot(const SDL_FRect& playerRect) 
{
	float dx = (playerRect.x > m_collider.rect().x) ? 1.0f : -1.0f;
	m_bullets.push_back(make_unique<EnemyBullet>(
		nullptr,
		m_collider.rect().x, m_collider.rect().y, m_bulletTexture, dx, 0.0f
	));
}

void  EnemyShooter::render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) {
	EnemyWalking::render(renderer, cameraOffset); // “G–{‘Ì‚Ì•`‰æ
	for (auto& b : m_bullets) b->render(renderer, cameraOffset); // ’e‚Ì•`‰æ
}

void EnemyShooter::checkPlayerCollision(Player& player)
{
	for (auto& b : m_bullets)
	{
		if (b->isActive() && b->collider().intersect(player.collider()))
		{
			player.takeDamage(5);
			b->deleteBullet();
		}
	}
}