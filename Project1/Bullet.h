#pragma once
#include "Collider.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
class Bullet
{
public:
	Bullet(SDL_Renderer* renderer,
		float x, float y, float w, float h,
		SDL_Texture* texture, float vx, float vy);

	~Bullet();

	void update(float dt, const vector<BoxCollider>& grounds);

	void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset)
	{
		if (!m_isActive) {
			return;
		}
		SDL_FRect drawRect = m_collider.rect();
		drawRect.x -= cameraOffset.x;
		drawRect.y -= cameraOffset.y;

		SDL_RenderTexture(renderer, m_texture, nullptr, &drawRect);
	}

	bool isActive() const { return m_isActive; }
	void deleteBullet() { m_isActive = false; }
	const BoxCollider& collider() const { return m_collider; }

private:
	BoxCollider m_collider;
	bool m_isActive;
	float m_vx;
	float m_vy;
	float velX = 0.0f;
	float velY = 0.0f;
	float bulletSpeed = 400.0f;

	float deletTimer;
	float deletMaxTime = 2.0f;

	SDL_Texture* m_texture = nullptr;
};

