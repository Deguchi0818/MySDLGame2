#pragma once
#include "Collider.h"

#include <SDL3/SDL.h>
class Player
{
public:
	Player(SDL_Renderer* rendere,
		float x, float y, float w, float h,
		const char* texturtPath);

	~Player();

	void update(float dt, int m_width, int m_height);
	void render(SDL_Renderer* texturePath, const SDL_FPoint& cameraOffset);

	BoxCollider& collider() { return m_collider; }
	const BoxCollider& collider() const { return m_collider; }

	void setOnGround(bool on);
	bool isOnGround() const;

	float velX = 0.0f;
	float velY = 0.0f;

	const SDL_FRect& getPrevRect() const { return m_prevRect; }

private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;

	float speed = 200.0f;
	float gravity = 1500.0f;
	float jumpPower = -500.0f;

	bool m_onGround = false;

	SDL_Texture* m_texture = nullptr;
};

