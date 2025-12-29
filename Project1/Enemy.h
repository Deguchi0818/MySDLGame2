#pragma once
#include "Collider.h"
#include "Player.h"
#include <SDL3/SDL.h>
#include<iostream>
#include<vector>

using namespace std;

class Enemy
{
public:
	Enemy(float x, float y, float w, float h)
		: m_collider(x, y, w, h), m_isDead(false) 
	{
		speed = 100;
	}

	virtual ~Enemy() = default;
	virtual void update(float dt, const SDL_FRect& playerRect, const Player& player, const vector<BoxCollider>& grounds) = 0;

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
	virtual void applyKnockback(float forceX, float forceY) {
		m_velX = forceX;
		m_velY = forceY;
		setOnGround(false);
		m_stunTimer = 1.0f;
	}
	void setOnGround(bool on) {
		if (on)
		{
			m_velY = 0.0f;
		}
		m_onGround = on;
	}
	bool isOnGround()  const
	{
		return m_onGround;
	}
	bool isStunned() const { return m_stunTimer > 0; }

protected:
	BoxCollider m_collider;
	bool m_isDead;
	float m_velX = 0;
	float m_velY = 0;
	float gravity = 1500.0f;
	float speed = 100;
	float m_stunTimer = 0.0f;
	bool m_onGround = false;

};

