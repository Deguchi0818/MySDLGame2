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

	int m_hp = 100.0f;
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
	int getCurrentHp() const { return m_currentHp; }
	int getMaxHp() const { return m_params.m_hp; }
	

	void updateInputTrigger(const bool* keys) {
		static bool lastJump = false;
		bool currentJump = keys[SDL_SCANCODE_SPACE];
		m_jumpTriggered = (currentJump && !lastJump);
		lastJump = currentJump;
	}

	bool isJumpTriggered() const { return m_jumpTriggered; }

	
	void updateInputState(const bool* keys) {
		bool current = keys[SDL_SCANCODE_SPACE];
		m_jumpTriggered = (current && !m_prevJumpKey); 
		m_prevJumpKey = current;
	}

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

	void takeDamage(int damage);

private:
	BoxCollider m_collider;

	SDL_FRect m_prevRect;

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

	bool m_jumpTriggered = false;
	bool m_prevJumpKey = false;

	int m_currentHp;
	float m_invincibleTimer = 0.0f;
	const float INVINCIBLE_TIME = 1.0f;

	AimDir m_currentAim = { 1.0f, 0.0f };

	SDL_Texture* m_texture = nullptr;

	unique_ptr<PlayerState> m_currentState;
};

