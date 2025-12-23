#pragma once
#include "Collider.h"
#include <SDL3/SDL.h>
#include<iostream>
#include<vector>

using namespace std;

class Enemy
{
public:
	Enemy(float x, float y, float w, float h)
		: m_collider(x, y, w, h), m_isDead(false) {}

	virtual ~Enemy() = default;
	virtual void update(float dt, const SDL_FRect& playerRect, const vector<BoxCollider>& grounds) = 0;

	virtual void render(SDL_Renderer* renderer, const SDL_FPoint& cameraOffset) 
	{
		if (m_isDead) {
			return;
		}
		SDL_FRect drawRect = m_collider.rect();
		drawRect.x -= cameraOffset.x;
		drawRect.y -= cameraOffset.y;

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &drawRect);
	}

	bool isDead() const { return m_isDead; }
	void die() { m_isDead = true; }
	const BoxCollider& collider() const { return m_collider; }

protected:
	BoxCollider m_collider;
	bool m_isDead;
	float m_velX = 0;
	float m_velY = 0;
	float gravity = 1500.0f;
};

