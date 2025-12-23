#pragma once
#include "Collider.h"

#include <SDL3/SDL.h>
struct PlayerParams 
{
	float speed = 0.0f;
	float gravity = 1500.0f;
	float jumpPower = -500.0f;
	float fallMultiplier = 12.0f;

	// ホバリング
	float hoverFlapSpeed = -350.0f;
	float hoverGravity = 250.0f;
	float hoverFallMaxSpeed = 150.0f;

	// コヨーテタイム
	float coyoteTimeMax = 0.12f;

	float jumpBufferMax = 0.12f;
};

class Player
{
public:
	Player(SDL_Renderer* rendere,
		float x, float y, float w, float h,
		const char* texturtPath);

	~Player();

	void update(float dt, int m_width, int m_height);
	void render(SDL_Renderer* rendere, const SDL_FPoint& cameraOffset);

	void applyParams(const PlayerParams& p) { m_params = p; }

	BoxCollider& collider() { return m_collider; }
	const BoxCollider& collider() const { return m_collider; }

	void setOnGround(bool on);
	bool isOnGround() const;
	void resetPosition(float x, float y);

	float velX = 0.0f;
	float velY = 0.0f;

	const SDL_FRect& getPrevRect() const { return m_prevRect; }

private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;

	void horizontalMove(const bool* kyes, float dt);
	void jump(const bool* kyes, float dt);
	void applyPhysics(float dt);
	void updateTimers(float dt);
	void checkScreenBounds(float screenW, float screenH);

	PlayerParams m_params;

	float jumpBufferTimer = 0.0f;
	float coyoteTimer = 0.0f;

	bool isHovering;
	bool m_onGround = false;
	bool prevJumpPressed;

	SDL_Texture* m_texture = nullptr;
};

