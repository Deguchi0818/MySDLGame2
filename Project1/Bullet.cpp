#include "Bullet.h"

Bullet::Bullet(SDL_Renderer* renderer,
	float x, float y, float w, float h,
	SDL_Texture* texture, float vx, float vy) : m_vx(vx), m_vy(vy), m_isActive(true), m_collider(x, y, w, h)
{
	m_texture = texture;
}

Bullet::~Bullet() 
{
	if (m_texture)
	{
		m_texture = nullptr;
	}
}


void Bullet::update(float dt, const vector<BoxCollider>& grounds)
{
	if (!m_isActive) return;
	velX = bulletSpeed * m_vx;
	velY = bulletSpeed * m_vy;

	SDL_FRect r = m_collider.rect();
	r.x += velX  * dt;
	r.y += velY  * dt;
	m_collider.setPosition(r.x, r.y);

	for (const auto& wall : grounds) {
		if (m_collider.intersect(wall)) {
			m_isActive = false; // ï«Ç…êGÇÍÇΩÇÁíeÇÕè¡Ç¶ÇÈ
			break;
		}
	}
}
