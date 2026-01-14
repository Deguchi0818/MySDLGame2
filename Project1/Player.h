#pragma once
#include "Collider.h"
#include "PlayerState.h"
#include "IdleState.h"

#include <SDL3/SDL.h>
using namespace std;

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

struct AimDir {
	float vx;
	float vy;
};

class Player
{
public:
	Player(SDL_Renderer* renderer,
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
	void setIsHovering(bool hover) { isHovering = hover; }
	bool getIsHovering() const { return isHovering; }
	float getCoyoteTimer() const { return coyoteTimer; }
	void setCoyoteTimer(float time) { coyoteTimer = time; }
	float getJumpBufferTimer() const { return jumpBufferTimer; }
	void setJumpBufferTimer(float time) { jumpBufferTimer = time; }
	void resetPosition(float x, float y);

	bool wantsToShoot() const { return m_wantsToShoot; }
	void consumeShootFlag() { m_wantsToShoot = false; } // フラグを消費（リセット）
	float getFacingDir() const { return m_facingDir; }  // 向きを教える
	AimDir getAimDir() const { return m_currentAim; }
	void applyKnockback(float forceX, float forceY);

	float velX = 0.0f;
	float velY = 0.0f;
	float m_facingDir = 1.0f;

	const SDL_FRect& getPrevRect() const { return m_prevRect; }


	void changeState(unique_ptr<PlayerState> newState);

	const PlayerParams& getParams() const { return m_params; }

private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;

	void horizontalMove(const bool* kyes, float dt);
	void jump(const bool* kyes, float dt);
	void applyPhysics(float dt);
	void updateTimers(float dt);
	void checkScreenBounds(float screenW, float screenH);
	void attack(const bool* kyes, float dt);
	

	PlayerParams m_params;

	float jumpBufferTimer = 0.0f;
	float coyoteTimer = 0.0f;

	bool isHovering;
	bool m_onGround = false;
	bool prevJumpPressed;

	float m_fireCooldown = 0.7f; // 発射間隔（秒）
	float m_fireTimer = 0.0f;    // 残り待ち時間
	bool m_wantsToShoot = false; // 弾を撃ちたいフラグ


	AimDir m_currentAim = { 1.0f, 0.0f };

	SDL_Texture* m_texture = nullptr;

	unique_ptr<PlayerState> m_currentState;
};

