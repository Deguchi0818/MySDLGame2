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

		if (m_flashTimer > 0) 
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		}

		SDL_RenderFillRect(renderer, &drawRect);
	}

	bool isDead() const { return m_isDead; }
	void die() { m_isDead = true; }
	const BoxCollider& collider() const { return m_collider; }
	void handleStunState(float dt);
	virtual void updatePhysics(float dt, const vector<BoxCollider>& grounds);
	
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

	virtual void takeDamage() 
	{
		if (m_isDead || m_flashTimer > 0) return;

		--m_hp;
		m_flashTimer = 0.1f;
		applyKnockback(1.0f, 0.0f);

		if (m_hp <= 0) 
		{
			die();
		}
	}

	void updateFlashTimer(float dt) {
		if (m_flashTimer > 0) {
			m_flashTimer -= dt;
		}
	}

	virtual void checkPlayerCollision(Player& player) {}

protected:
	BoxCollider m_collider;
	bool m_isDead = false;
	float m_velX = 0;
	float m_velY = 0;
	float gravity = 1500.0f;
	float speed = 100;
	float m_stunTimer = 0.0f;
	int m_hp = 3;
	bool m_onGround = false;
	float m_flashTimer = 0.0f;

};

